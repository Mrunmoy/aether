"""
C99 aether-lite code emitter: generates dispatch tables and handler stubs
for bare-metal MCU firmware.

Each service produces two flat files:
  {ServiceName}.h  -- macros, extern handler decls, register function,
                      notification senders
  {ServiceName}.c  -- dispatch wrappers (unmarshal -> call user handler -> marshal)
"""

import re
from typing import Optional

from .parser import IdlFile, Param, StructDef
from .types import TYPE_MAP, cpp_type, fnv1a_32


# ---- C99 wire sizes (bytes) for each IDL built-in type -----------------

_C99_WIRE_SIZE = {
    "uint8":   1,
    "int8":    1,
    "bool":    1,
    "uint16":  2,
    "int16":   2,
    "uint32":  4,
    "int32":   4,
    "float32": 4,
    "uint64":  8,
    "int64":   8,
    "float64": 8,
}


def _c_type(idl_type: str) -> str:
    """Map IDL type to C99 type.  Built-ins go through TYPE_MAP; user
    types (enums/structs) pass through unchanged."""
    if idl_type in TYPE_MAP:
        return cpp_type(idl_type)  # same mapping works for C99
    return idl_type


def _find_struct(idl: IdlFile, type_name: str) -> Optional[StructDef]:
    """Look up a struct definition by name in the IDL, or None."""
    for s in idl.structs:
        if s.name == type_name:
            return s
    return None


def _is_struct_type(idl: IdlFile, type_name: str) -> bool:
    """Return True if *type_name* refers to a struct defined in the IDL."""
    return _find_struct(idl, type_name) is not None


def _struct_wire_size_int(sd: StructDef, idl: IdlFile) -> Optional[int]:
    """Compute the packed wire size of a struct as the sum of its field
    wire sizes.  Returns None if any field has an unknown size."""
    total = 0
    for f in sd.fields:
        if f.type_name == "string":
            if f.array_size is None:
                return None
            total += f.array_size + 1
        elif f.array_size is not None:
            base = _C99_WIRE_SIZE.get(f.type_name)
            if base is not None:
                total += base * f.array_size
            else:
                nested = _find_struct(idl, f.type_name)
                if nested is not None:
                    n = _struct_wire_size_int(nested, idl)
                    if n is None:
                        return None
                    total += n * f.array_size
                else:
                    return None
        else:
            base = _C99_WIRE_SIZE.get(f.type_name)
            if base is not None:
                total += base
            else:
                nested = _find_struct(idl, f.type_name)
                if nested is not None:
                    n = _struct_wire_size_int(nested, idl)
                    if n is None:
                        return None
                    total += n
                else:
                    return None
    return total


def _wire_size_int(p: Param, idl: Optional[IdlFile] = None) -> Optional[int]:
    """Return the fixed wire size in bytes for *p*, or None if it
    cannot be expressed as a plain integer (e.g. unknown user type)."""
    if p.type_name == "string":
        return (p.array_size + 1) if p.array_size else None
    if p.array_size is not None:
        base = _C99_WIRE_SIZE.get(p.type_name)
        if base is not None:
            return base * p.array_size
        if idl is not None:
            sd = _find_struct(idl, p.type_name)
            if sd is not None:
                n = _struct_wire_size_int(sd, idl)
                if n is not None:
                    return n * p.array_size
        return None  # user-defined element type
    base = _C99_WIRE_SIZE.get(p.type_name)
    if base is not None:
        return base
    if idl is not None:
        sd = _find_struct(idl, p.type_name)
        if sd is not None:
            return _struct_wire_size_int(sd, idl)
    return None


def _wire_size_expr(p: Param, idl: Optional[IdlFile] = None) -> str:
    """C expression for the wire size of a single param."""
    n = _wire_size_int(p, idl)
    if n is not None:
        return str(n)
    # Fall back to sizeof for user-defined types.
    ct = _c_type(p.type_name)
    if p.array_size is not None:
        return f"{p.array_size} * sizeof({ct})"
    return f"sizeof({ct})"


def _total_wire_size(params: list, idl: Optional[IdlFile] = None) -> tuple:
    """Return (int_total_or_None, c_expression_str) for a list of params.

    When all params have known integer sizes the expression is a single
    integer literal (e.g. ``10``).  When some sizes require sizeof() the
    integer portions are collapsed and combined with the symbolic parts.
    """
    int_accum = 0
    sym_parts = []
    all_int = True
    for p in params:
        n = _wire_size_int(p, idl)
        if n is not None:
            int_accum += n
        else:
            all_int = False
            sym_parts.append(_wire_size_expr(p, idl))
    if not params:
        return (0, "0")
    if all_int:
        return (int_accum, str(int_accum))
    # Mix of integer and symbolic parts.
    parts = []
    if int_accum:
        parts.append(str(int_accum))
    parts.extend(sym_parts)
    return (None, " + ".join(parts))


def _to_upper_snake(name: str) -> str:
    """Convert CamelCase to UPPER_SNAKE_CASE."""
    s = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    s = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", s)
    return s.upper()


def _handler_name(service: str, method_name: str) -> str:
    return f"{service}_handle{method_name}"


def _dispatch_name(service: str, method_name: str) -> str:
    return f"{service}_dispatch{method_name}"


def _notify_sender_name(service: str, notify_name: str) -> str:
    return f"{service}_notify{notify_name}"


# ---- Field-by-field struct marshaling helpers --------------------------

def _emit_struct_marshal(w, sd: StructDef, idl: IdlFile, src_var: str,
                         buf_var: str, offset_expr: str) -> str:
    """Emit field-by-field memcpy to marshal a struct into a byte buffer.
    Returns the updated offset expression after all fields."""
    field_offset_int = 0
    if offset_expr.isdigit():
        field_offset_int = int(offset_expr)
    else:
        field_offset_int = None

    for f in sd.fields:
        if field_offset_int is not None:
            cur_offset = str(field_offset_int)
        else:
            cur_offset = offset_expr

        if f.type_name == "string":
            wire = f.array_size + 1
            w(f"    memcpy({buf_var} + {cur_offset}, {src_var}.{f.name}, {wire});")
            if field_offset_int is not None:
                field_offset_int += wire
            else:
                offset_expr = f"{cur_offset} + {wire}"
        elif f.array_size is not None:
            base = _C99_WIRE_SIZE.get(f.type_name)
            nested = _find_struct(idl, f.type_name) if base is None else None
            if nested is not None:
                elem_sz = _struct_wire_size_int(nested, idl)
                if elem_sz is not None:
                    # Array of structs: loop and marshal each element
                    w(f"    {{ unsigned _ai; for (_ai = 0; _ai < {f.array_size}; ++_ai) {{")
                    elem_offset = f"{cur_offset} + {elem_sz} * _ai"
                    _emit_struct_marshal(w, nested, idl,
                                         f"{src_var}.{f.name}[_ai]",
                                         buf_var, elem_offset)
                    w(f"    }} }}")
                    wire = elem_sz * f.array_size
                    if field_offset_int is not None:
                        field_offset_int += wire
                    else:
                        offset_expr = f"{cur_offset} + {wire}"
                else:
                    sz = f"{f.array_size} * sizeof({_c_type(f.type_name)})"
                    w(f"    memcpy({buf_var} + {cur_offset}, {src_var}.{f.name}, {sz});")
                    offset_expr = f"{cur_offset} + {sz}"
                    field_offset_int = None
            else:
                if base is not None:
                    wire = base * f.array_size
                else:
                    wire = None
                sz = str(wire) if wire is not None else f"{f.array_size} * sizeof({_c_type(f.type_name)})"
                w(f"    memcpy({buf_var} + {cur_offset}, {src_var}.{f.name}, {sz});")
                if wire is not None and field_offset_int is not None:
                    field_offset_int += wire
                else:
                    offset_expr = f"{cur_offset} + {sz}"
                    field_offset_int = None
        else:
            base = _C99_WIRE_SIZE.get(f.type_name)
            if base is not None:
                sz = str(base)
                w(f"    memcpy({buf_var} + {cur_offset}, &{src_var}.{f.name}, {sz});")
                if field_offset_int is not None:
                    field_offset_int += base
                else:
                    offset_expr = f"{cur_offset} + {sz}"
            else:
                nested = _find_struct(idl, f.type_name)
                if nested is not None:
                    n = _struct_wire_size_int(nested, idl)
                    if n is not None:
                        # Recursively marshal nested struct
                        new_offset = _emit_struct_marshal(
                            w, nested, idl, f"{src_var}.{f.name}",
                            buf_var, cur_offset)
                        if new_offset.isdigit():
                            field_offset_int = int(new_offset)
                        else:
                            offset_expr = new_offset
                            field_offset_int = None
                        continue
                    else:
                        sz = f"sizeof({_c_type(f.type_name)})"
                        w(f"    memcpy({buf_var} + {cur_offset}, &{src_var}.{f.name}, {sz});")
                        offset_expr = f"{cur_offset} + {sz}"
                        field_offset_int = None
                else:
                    sz = f"sizeof({_c_type(f.type_name)})"
                    w(f"    memcpy({buf_var} + {cur_offset}, &{src_var}.{f.name}, {sz});")
                    offset_expr = f"{cur_offset} + {sz}"
                    field_offset_int = None

    if field_offset_int is not None:
        return str(field_offset_int)
    return offset_expr


def _emit_struct_unmarshal(w, sd: StructDef, idl: IdlFile, dst_var: str,
                           buf_var: str, offset_expr: str) -> str:
    """Emit field-by-field memcpy to unmarshal a struct from a byte buffer.
    Returns the updated offset expression after all fields."""
    field_offset_int = 0
    if offset_expr.isdigit():
        field_offset_int = int(offset_expr)
    else:
        field_offset_int = None

    for f in sd.fields:
        if field_offset_int is not None:
            cur_offset = str(field_offset_int)
        else:
            cur_offset = offset_expr

        if f.type_name == "string":
            wire = f.array_size + 1
            w(f"    memcpy({dst_var}.{f.name}, {buf_var} + {cur_offset}, {wire});")
            if field_offset_int is not None:
                field_offset_int += wire
            else:
                offset_expr = f"{cur_offset} + {wire}"
        elif f.array_size is not None:
            base = _C99_WIRE_SIZE.get(f.type_name)
            nested = _find_struct(idl, f.type_name) if base is None else None
            if nested is not None:
                elem_sz = _struct_wire_size_int(nested, idl)
                if elem_sz is not None:
                    # Array of structs: loop and unmarshal each element
                    w(f"    {{ unsigned _ai; for (_ai = 0; _ai < {f.array_size}; ++_ai) {{")
                    elem_offset = f"{cur_offset} + {elem_sz} * _ai"
                    _emit_struct_unmarshal(w, nested, idl,
                                           f"{dst_var}.{f.name}[_ai]",
                                           buf_var, elem_offset)
                    w(f"    }} }}")
                    wire = elem_sz * f.array_size
                    if field_offset_int is not None:
                        field_offset_int += wire
                    else:
                        offset_expr = f"{cur_offset} + {wire}"
                else:
                    sz = f"{f.array_size} * sizeof({_c_type(f.type_name)})"
                    w(f"    memcpy({dst_var}.{f.name}, {buf_var} + {cur_offset}, {sz});")
                    offset_expr = f"{cur_offset} + {sz}"
                    field_offset_int = None
            else:
                if base is not None:
                    wire = base * f.array_size
                else:
                    wire = None
                sz = str(wire) if wire is not None else f"{f.array_size} * sizeof({_c_type(f.type_name)})"
                w(f"    memcpy({dst_var}.{f.name}, {buf_var} + {cur_offset}, {sz});")
                if wire is not None and field_offset_int is not None:
                    field_offset_int += wire
                else:
                    offset_expr = f"{cur_offset} + {sz}"
                    field_offset_int = None
        else:
            base = _C99_WIRE_SIZE.get(f.type_name)
            if base is not None:
                sz = str(base)
                w(f"    memcpy(&{dst_var}.{f.name}, {buf_var} + {cur_offset}, {sz});")
                if field_offset_int is not None:
                    field_offset_int += base
                else:
                    offset_expr = f"{cur_offset} + {sz}"
            else:
                nested = _find_struct(idl, f.type_name)
                if nested is not None:
                    n = _struct_wire_size_int(nested, idl)
                    if n is not None:
                        new_offset = _emit_struct_unmarshal(
                            w, nested, idl, f"{dst_var}.{f.name}",
                            buf_var, cur_offset)
                        if new_offset.isdigit():
                            field_offset_int = int(new_offset)
                        else:
                            offset_expr = new_offset
                            field_offset_int = None
                        continue
                    else:
                        sz = f"sizeof({_c_type(f.type_name)})"
                        w(f"    memcpy(&{dst_var}.{f.name}, {buf_var} + {cur_offset}, {sz});")
                        offset_expr = f"{cur_offset} + {sz}"
                        field_offset_int = None
                else:
                    sz = f"sizeof({_c_type(f.type_name)})"
                    w(f"    memcpy(&{dst_var}.{f.name}, {buf_var} + {cur_offset}, {sz});")
                    offset_expr = f"{cur_offset} + {sz}"
                    field_offset_int = None

    if field_offset_int is not None:
        return str(field_offset_int)
    return offset_expr


# ---- Header (.h) emitter ----------------------------------------------

def emit_aether_lite_h(idl: IdlFile) -> str:
    name = idl.service_name
    prefix = _to_upper_snake(name)
    service_id = fnv1a_32(name)
    method_count = len(idl.methods)

    lines = []
    w = lines.append

    w("/* Generated by ipcgen --backend aether_lite -- do not edit. */")
    guard = f"{prefix}_H"
    w(f"#ifndef {guard}")
    w(f"#define {guard}")
    w("")
    w('#include "aether_lite_dispatch.h"')
    w('#include "aether_lite_types.h"')
    w("#include <string.h>")
    w("")

    # Emit C99 type definitions for enums and structs inline
    if idl.enums or idl.structs:
        w("/* ---- IDL type definitions ------------------------------------- */")
        for enum_def in idl.enums:
            w("")
            w(f"/* enum {enum_def.name} */")
            for val in enum_def.values:
                w(f"#define {val.name}  {val.value}")
            w(f"typedef uint32_t {enum_def.name};")
        for struct_def in idl.structs:
            w("")
            w(f"typedef struct {{")
            for f in struct_def.fields:
                ct = _c_type(f.type_name)
                if f.type_name == "string":
                    buf_size = (f.array_size or 0) + 1
                    w(f"    char {f.name}[{buf_size}];")
                elif f.array_size is not None:
                    w(f"    {ct} {f.name}[{f.array_size}];")
                else:
                    w(f"    {ct} {f.name};")
            w(f"}} {struct_def.name};")
        w("")

    # Service ID and method count
    w(f"#define {prefix}_SERVICE_ID  0x{service_id:08x}u")
    w(f"#define {prefix}_METHOD_COUNT {method_count}")
    w("")

    # Method IDs
    if idl.methods:
        w("/* Method IDs */")
        for m in idl.methods:
            macro = f"{prefix}_{_to_upper_snake(m.name)}"
            w(f"#define {macro}  {m.method_id}")
        w("")

    # Notification IDs
    if idl.notifications:
        w("/* Notification IDs */")
        for n in idl.notifications:
            macro = f"{prefix}_{_to_upper_snake(n.name)}"
            w(f"#define {macro}  {n.notify_id}")
        w("")

    # User-implemented handler declarations
    w("/* ---- User-implemented handlers -------------------------------- */")
    w("/* Implement these in your application code. */")
    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]
        parts = _handler_param_list(in_params, out_params)
        w(f"extern int {_handler_name(name, m.name)}({', '.join(parts)});")
    w("")

    # Dispatch function forward declarations (extern in register body)
    # and the register function
    w("/* ---- Generated dispatch table --------------------------------- */")
    w("/* Call this from main() after al_init(). */")
    w(f"static inline int {name}_register(void)")
    w("{")
    if not idl.methods:
        w("    /* This service has no methods; aether-lite does not allow")
        w("     * registering services with method_count == 0. */")
        w("    return AL_ERR_INVALID_ARGUMENT;")
        w("}")
        w("")
    else:
        for m in idl.methods:
            dn = _dispatch_name(name, m.name)
            w(f"    extern int {dn}(const uint8_t *, uint32_t, uint8_t *, uint32_t, uint32_t *);")
        w("")
        w(f"#if {prefix}_METHOD_COUNT > AL_MAX_METHODS")
        w(f'#error "{prefix}_METHOD_COUNT exceeds AL_MAX_METHODS"')
        w("#endif")
        w(f"    static const al_method_entry_t methods[{prefix}_METHOD_COUNT] = {{")
        for m in idl.methods:
            macro = f"{prefix}_{_to_upper_snake(m.name)}"
            dn = _dispatch_name(name, m.name)
            w(f"        {{ {macro}, {dn} }},")
        w("    };")
        w(f"    return al_register_service({prefix}_SERVICE_ID, methods, (uint8_t){prefix}_METHOD_COUNT);")
        w("}")
        w("")

    # Notification senders
    if idl.notifications:
        w("#if AL_ENABLE_NOTIFICATIONS")
        w("/* ---- Notification senders ------------------------------------- */")
        for n in idl.notifications:
            _emit_notify_sender(w, name, prefix, n, idl)
        w("#endif /* AL_ENABLE_NOTIFICATIONS */")
        w("")

    w(f"#endif /* {guard} */")
    w("")

    return "\n".join(lines)


def _handler_param_list(in_params, out_params) -> list:
    """Build the C parameter list for the user-facing handler function."""
    parts = []
    for p in in_params:
        ct = _c_type(p.type_name)
        if p.type_name == "string":
            parts.append(f"const char *{p.name}")
        elif p.array_size is not None:
            parts.append(f"const {ct} *{p.name}")
        else:
            parts.append(f"{ct} {p.name}")
    for p in out_params:
        ct = _c_type(p.type_name)
        if p.type_name == "string":
            parts.append(f"char *{p.name}")
        elif p.array_size is not None:
            parts.append(f"{ct} *{p.name}")
        else:
            parts.append(f"{ct} *{p.name}")
    if not parts:
        parts.append("void")
    return parts


def _emit_notify_sender(w, service_name, prefix, n, idl):
    """Emit a static inline notification sender into the header."""
    sender = _notify_sender_name(service_name, n.name)
    macro = f"{prefix}_{_to_upper_snake(n.name)}"

    # Build parameter list
    parts = []
    for p in n.params:
        ct = _c_type(p.type_name)
        if p.type_name == "string":
            parts.append(f"const char *{p.name}")
        elif p.array_size is not None:
            parts.append(f"const {ct} *{p.name}")
        else:
            parts.append(f"{ct} {p.name}")

    sig = ", ".join(parts) if parts else "void"
    w(f"static inline int {sender}({sig})")
    w("{")

    if not n.params:
        w(f"    return al_send_notify({prefix}_SERVICE_ID, {macro}, (void *)0, 0);")
        w("}")
        return

    # Calculate total buffer size
    _, size_expr = _total_wire_size(n.params, idl)
    w(f"    uint8_t buf[{size_expr}];")

    offset_int = 0
    offset_expr = "0"
    for i, p in enumerate(n.params):
        sz_int = _wire_size_int(p, idl)
        sz_expr = _wire_size_expr(p, idl)
        ct = _c_type(p.type_name)

        if p.type_name == "string":
            # Copy the fixed-size string buffer including null terminator space
            buf_size = p.array_size + 1
            w(f"    memset(buf + {offset_expr}, 0, {buf_size});")
            # Use a length-limited copy. strncpy is available in <string.h>.
            w(f"    if ({p.name}) {{ unsigned _i; for (_i = 0; _i < {p.array_size} && {p.name}[_i]; ++_i) buf[{offset_expr} + _i] = (uint8_t){p.name}[_i]; }}")
        elif _is_struct_type(idl, p.type_name) and p.array_size is None:
            # Field-by-field marshal for struct params
            sd = _find_struct(idl, p.type_name)
            new_offset = _emit_struct_marshal(w, sd, idl, p.name, "buf", offset_expr)
            if new_offset.isdigit():
                offset_int = int(new_offset)
                offset_expr = new_offset
            else:
                offset_int = None
                offset_expr = new_offset
            continue
        elif p.array_size is not None and _is_struct_type(idl, p.type_name):
            # Array of structs: loop and marshal each element field-by-field
            sd = _find_struct(idl, p.type_name)
            elem_sz = _struct_wire_size_int(sd, idl)
            if elem_sz is not None:
                w(f"    {{ unsigned _ai; for (_ai = 0; _ai < {p.array_size}; ++_ai) {{")
                elem_offset = f"{offset_expr} + {elem_sz} * _ai"
                _emit_struct_marshal(w, sd, idl, f"{p.name}[_ai]", "buf", elem_offset)
                w(f"    }} }}")
            else:
                w(f"    memcpy(buf + {offset_expr}, {p.name}, {sz_expr});")
        elif p.array_size is not None:
            w(f"    memcpy(buf + {offset_expr}, {p.name}, {sz_expr});")
        else:
            w(f"    memcpy(buf + {offset_expr}, &{p.name}, {sz_expr});")

        # Advance offset
        if sz_int is not None:
            if isinstance(offset_int, int):
                offset_int += sz_int
                offset_expr = str(offset_int)
            else:
                offset_expr = f"{offset_expr} + {sz_expr}"
                offset_int = None
        else:
            offset_expr = f"{offset_expr} + {sz_expr}"
            offset_int = None

    w(f"    return al_send_notify({prefix}_SERVICE_ID, {macro}, buf, sizeof(buf));")
    w("}")


# ---- Implementation (.c) emitter --------------------------------------

def emit_aether_lite_c(idl: IdlFile) -> str:
    name = idl.service_name
    prefix = _to_upper_snake(name)

    lines = []
    w = lines.append

    w("/* Generated by ipcgen --backend aether_lite -- do not edit. */")
    h_name = f"{name}.h"
    w(f'#include "{h_name}"')
    w("#include <string.h>")
    w("")

    w("/* ---- Dispatch wrappers (unmarshal -> call user handler -> marshal) ---- */")

    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]
        _emit_dispatch_wrapper(w, name, prefix, m, in_params, out_params, idl)

    w("")

    return "\n".join(lines)


def _emit_dispatch_wrapper(w, service_name, prefix, method, in_params, out_params, idl):
    dn = _dispatch_name(service_name, method.name)
    hn = _handler_name(service_name, method.name)

    w("")
    w(f"int {dn}(")
    w("    const uint8_t *req, uint32_t req_len,")
    w("    uint8_t *resp, uint32_t resp_cap, uint32_t *resp_len)")
    w("{")

    # Suppress unused-parameter warnings
    if not in_params:
        w("    (void)req; (void)req_len;")
    if not out_params:
        w("    (void)resp; (void)resp_cap;")

    # Validate request size for [in] params
    if in_params:
        _, req_size_expr = _total_wire_size(in_params, idl)
        w(f"    if (req_len < {req_size_expr}) return AL_ERR_INVALID_ARGUMENT;")

    # Validate response capacity for [out] params
    if out_params:
        _, resp_size_expr = _total_wire_size(out_params, idl)
        w(f"    if (resp_cap < {resp_size_expr}) return AL_ERR_OVERFLOW;")

    w("")

    # Unmarshal [in] params
    offset_int = 0
    offset_expr = "0"
    for p in in_params:
        ct = _c_type(p.type_name)
        sz_int = _wire_size_int(p, idl)
        sz_expr = _wire_size_expr(p, idl)

        if p.type_name == "string":
            buf_size = p.array_size + 1
            w(f"    char {p.name}[{buf_size}];")
            w(f"    memcpy({p.name}, req + {offset_expr}, {p.array_size});")
            w(f"    {p.name}[{p.array_size}] = '\\0';")
        elif _is_struct_type(idl, p.type_name) and p.array_size is None:
            # Field-by-field unmarshal for struct params
            sd = _find_struct(idl, p.type_name)
            w(f"    {ct} {p.name};")
            w(f"    memset(&{p.name}, 0, sizeof({p.name}));")
            new_offset = _emit_struct_unmarshal(w, sd, idl, p.name, "req", offset_expr)
            if new_offset.isdigit():
                offset_int = int(new_offset)
                offset_expr = new_offset
            else:
                offset_int = None
                offset_expr = new_offset
            continue
        elif p.array_size is not None and _is_struct_type(idl, p.type_name):
            # Array of structs: loop and unmarshal each element field-by-field
            sd = _find_struct(idl, p.type_name)
            elem_sz = _struct_wire_size_int(sd, idl)
            w(f"    {ct} {p.name}[{p.array_size}];")
            w(f"    memset({p.name}, 0, sizeof({p.name}));")
            if elem_sz is not None:
                w(f"    {{ unsigned _ai; for (_ai = 0; _ai < {p.array_size}; ++_ai) {{")
                elem_offset = f"{offset_expr} + {elem_sz} * _ai"
                _emit_struct_unmarshal(w, sd, idl, f"{p.name}[_ai]", "req", elem_offset)
                w(f"    }} }}")
            else:
                w(f"    memcpy({p.name}, req + {offset_expr}, {sz_expr});")
        elif p.array_size is not None:
            w(f"    {ct} {p.name}[{p.array_size}];")
            w(f"    memcpy({p.name}, req + {offset_expr}, {sz_expr});")
        else:
            w(f"    {ct} {p.name} = 0;")
            w(f"    memcpy(&{p.name}, req + {offset_expr}, {sz_expr});")

        if sz_int is not None and isinstance(offset_int, int):
            offset_int += sz_int
            offset_expr = str(offset_int)
        else:
            offset_expr = f"{offset_expr} + {sz_expr}"
            offset_int = None

    # Declare [out] params (zero-initialized)
    for p in out_params:
        ct = _c_type(p.type_name)
        if p.type_name == "string":
            buf_size = p.array_size + 1
            w(f"    char {p.name}[{buf_size}];")
            w(f"    memset({p.name}, 0, {buf_size});")
        elif _is_struct_type(idl, p.type_name) and p.array_size is None:
            w(f"    {ct} {p.name};")
            w(f"    memset(&{p.name}, 0, sizeof({p.name}));")
        elif p.array_size is not None:
            w(f"    {ct} {p.name}[{p.array_size}];")
            w(f"    memset({p.name}, 0, sizeof({p.name}));")
        else:
            w(f"    {ct} {p.name} = 0;")

    # Call the user handler
    call_args = []
    for p in in_params:
        if p.type_name == "string" or p.array_size is not None:
            call_args.append(p.name)  # decays to pointer
        elif _is_struct_type(idl, p.type_name):
            call_args.append(p.name)  # pass struct by value
        else:
            call_args.append(p.name)
    for p in out_params:
        if p.type_name == "string" or p.array_size is not None:
            call_args.append(p.name)
        else:
            call_args.append(f"&{p.name}")

    args_str = ", ".join(call_args) if call_args else ""
    w(f"    int rc = {hn}({args_str});")

    # Marshal [out] params
    if out_params:
        w("")
        offset_int = 0
        offset_expr = "0"
        for p in out_params:
            ct = _c_type(p.type_name)
            sz_int = _wire_size_int(p, idl)
            sz_expr = _wire_size_expr(p, idl)

            if _is_struct_type(idl, p.type_name) and p.array_size is None:
                # Field-by-field marshal for struct params
                sd = _find_struct(idl, p.type_name)
                new_offset = _emit_struct_marshal(w, sd, idl, p.name, "resp", offset_expr)
                if new_offset.isdigit():
                    offset_int = int(new_offset)
                    offset_expr = new_offset
                else:
                    offset_int = None
                    offset_expr = new_offset
                continue

            if p.array_size is not None and _is_struct_type(idl, p.type_name):
                # Array of structs: loop and marshal each element field-by-field
                sd = _find_struct(idl, p.type_name)
                elem_sz = _struct_wire_size_int(sd, idl)
                if elem_sz is not None:
                    w(f"    {{ unsigned _ai; for (_ai = 0; _ai < {p.array_size}; ++_ai) {{")
                    elem_offset = f"{offset_expr} + {elem_sz} * _ai"
                    _emit_struct_marshal(w, sd, idl, f"{p.name}[_ai]", "resp", elem_offset)
                    w(f"    }} }}")
                else:
                    w(f"    memcpy(resp + {offset_expr}, {p.name}, {sz_expr});")
            elif p.type_name == "string" or p.array_size is not None:
                w(f"    memcpy(resp + {offset_expr}, {p.name}, {sz_expr});")
            else:
                w(f"    memcpy(resp + {offset_expr}, &{p.name}, {sz_expr});")

            if sz_int is not None and isinstance(offset_int, int):
                offset_int += sz_int
                offset_expr = str(offset_int)
            else:
                offset_expr = f"{offset_expr} + {sz_expr}"
                offset_int = None

        _, total_expr = _total_wire_size(out_params, idl)
        w(f"    *resp_len = {total_expr};")
    else:
        w("    *resp_len = 0;")

    w("    return rc;")
    w("}")
