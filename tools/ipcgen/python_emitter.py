"""
Python code emitter: generates a typed Python client module from the AST.

Each service produces one file:
  client/{Name}.py — typed client class with RPC methods + notification dispatch

The generated code depends on the ``aether_ipc`` runtime package (AetherClient)
which handles the actual transport.  This emitter only produces the code-gen layer.
"""

import re
from typing import Optional

from .parser import IdlFile, EnumDef, StructDef, StructField, Method, Notification, Param
from .types import fnv1a_32


# IDL type → Python struct format character (little-endian).
PY_STRUCT_FMT = {
    "uint8":   "B",
    "uint16":  "H",
    "uint32":  "I",
    "uint64":  "Q",
    "int8":    "b",
    "int16":   "h",
    "int32":   "i",
    "int64":   "q",
    "float32": "f",
    "float64": "d",
    "bool":    "B",
}

# IDL type → wire size in bytes.
PY_TYPE_SIZE = {
    "uint8":   1,
    "uint16":  2,
    "uint32":  4,
    "uint64":  8,
    "int8":    1,
    "int16":   2,
    "int32":   4,
    "int64":   8,
    "float32": 4,
    "float64": 8,
    "bool":    1,
}


def _snake_case(name: str) -> str:
    """Convert PascalCase/camelCase to snake_case."""
    s = re.sub(r'([A-Z]+)([A-Z][a-z])', r'\1_\2', name)
    s = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', s)
    return s.lower()


def _py_type_hint(type_name: str, array_size: Optional[int]) -> str:
    """Return a Python type hint for an IDL type."""
    if type_name == "string":
        return "str"
    if type_name == "bool":
        base = "bool"
    elif type_name in PY_STRUCT_FMT:
        if type_name.startswith("float"):
            base = "float"
        else:
            base = "int"
    else:
        base = type_name
    if array_size is not None:
        return f"list[{base}]"
    return base


def _field_wire_size(field: StructField, idl: IdlFile) -> int:
    """Compute the wire size of a struct field in bytes."""
    if field.type_name == "string":
        return field.array_size + 1  # null-terminated, +1 for NUL
    if field.type_name in PY_TYPE_SIZE:
        sz = PY_TYPE_SIZE[field.type_name]
        if field.array_size is not None:
            return sz * field.array_size
        return sz
    # User-defined type: enum → 4 bytes (uint32), struct → recursive
    for e in idl.enums:
        if e.name == field.type_name:
            if field.array_size is not None:
                return 4 * field.array_size
            return 4
    for s in idl.structs:
        if s.name == field.type_name:
            inner = _struct_wire_size(s, idl)
            if field.array_size is not None:
                return inner * field.array_size
            return inner
    # Fallback (shouldn't happen with valid IDL)
    return 4


def _struct_wire_size(struct: StructDef, idl: IdlFile) -> int:
    """Compute the total wire size of a struct."""
    return sum(_field_wire_size(f, idl) for f in struct.fields)


def _param_wire_size(p: Param, idl: IdlFile) -> int:
    """Compute the wire size of a method/notification parameter."""
    if p.type_name == "string":
        return p.array_size + 1
    if p.type_name in PY_TYPE_SIZE:
        sz = PY_TYPE_SIZE[p.type_name]
        if p.array_size is not None:
            return sz * p.array_size
        return sz
    for e in idl.enums:
        if e.name == p.type_name:
            if p.array_size is not None:
                return 4 * p.array_size
            return 4
    for s in idl.structs:
        if s.name == p.type_name:
            inner = _struct_wire_size(s, idl)
            if p.array_size is not None:
                return inner * p.array_size
            return inner
    return 4


def _is_enum(type_name: str, idl: IdlFile) -> bool:
    return any(e.name == type_name for e in idl.enums)


def _is_struct(type_name: str, idl: IdlFile) -> bool:
    return any(s.name == type_name for s in idl.structs)


def _find_struct(type_name: str, idl: IdlFile) -> Optional[StructDef]:
    for s in idl.structs:
        if s.name == type_name:
            return s
    return None


# ── Emit helpers ────────────────────────────────────────────────────────


def _emit_field_pack(w, field_name: str, field: StructField, idl: IdlFile,
                     indent: str = "        "):
    """Emit pack code for a single struct field into variable `buf`."""
    if field.type_name == "string":
        w(f"{indent}{field_name}_bytes = self.{field_name}.encode(\"utf-8\")[:{field.array_size}]")
        total = field.array_size + 1
        w(f"{indent}buf += {field_name}_bytes + b\"\\x00\" * ({total} - len({field_name}_bytes))")
    elif field.type_name in PY_STRUCT_FMT:
        fmt = PY_STRUCT_FMT[field.type_name]
        if field.array_size is not None:
            w(f"{indent}_items = self.{field_name}")
            w(f"{indent}if len(_items) != {field.array_size}:")
            w(f"{indent}    raise ValueError(f\"Field '{field_name}' must have length {field.array_size}, got {{len(_items)}}\")")
            w(f"{indent}for _item in _items:")
            w(f"{indent}    buf += struct.pack(\"<{fmt}\", _item)")
        else:
            w(f"{indent}buf += struct.pack(\"<{fmt}\", self.{field_name})")
    elif _is_enum(field.type_name, idl):
        if field.array_size is not None:
            w(f"{indent}_items = self.{field_name}")
            w(f"{indent}if len(_items) != {field.array_size}:")
            w(f"{indent}    raise ValueError(f\"Field '{field_name}' must have length {field.array_size}, got {{len(_items)}}\")")
            w(f"{indent}for _item in _items:")
            w(f"{indent}    buf += struct.pack(\"<I\", _item)")
        else:
            w(f"{indent}buf += struct.pack(\"<I\", self.{field_name})")
    elif _is_struct(field.type_name, idl):
        if field.array_size is not None:
            w(f"{indent}_items = self.{field_name}")
            w(f"{indent}if len(_items) != {field.array_size}:")
            w(f"{indent}    raise ValueError(f\"Field '{field_name}' must have length {field.array_size}, got {{len(_items)}}\")")
            w(f"{indent}for _item in _items:")
            w(f"{indent}    buf += _item.pack()")
        else:
            w(f"{indent}buf += self.{field_name}.pack()")


def _emit_field_unpack(w, field: StructField, idl: IdlFile,
                       var_prefix: str = "", indent: str = "        "):
    """Emit unpack code for a single struct field. Reads from `data` at `offset`."""
    fname = field.name
    if field.type_name == "string":
        total = field.array_size + 1
        w(f"{indent}{var_prefix}{fname} = data[offset:offset+{total}].split(b\"\\x00\", 1)[0].decode(\"utf-8\")")
        w(f"{indent}offset += {total}")
    elif field.type_name in PY_STRUCT_FMT:
        fmt = PY_STRUCT_FMT[field.type_name]
        sz = PY_TYPE_SIZE[field.type_name]
        is_bool = field.type_name == "bool"
        if field.array_size is not None:
            w(f"{indent}{var_prefix}{fname} = []")
            w(f"{indent}for _ in range({field.array_size}):")
            if is_bool:
                w(f"{indent}    {var_prefix}{fname}.append(bool(struct.unpack_from(\"<{fmt}\", data, offset)[0]))")
            else:
                w(f"{indent}    {var_prefix}{fname}.append(struct.unpack_from(\"<{fmt}\", data, offset)[0])")
            w(f"{indent}    offset += {sz}")
        else:
            if is_bool:
                w(f"{indent}{var_prefix}{fname} = bool(struct.unpack_from(\"<{fmt}\", data, offset)[0])")
            else:
                w(f"{indent}{var_prefix}{fname} = struct.unpack_from(\"<{fmt}\", data, offset)[0]")
            w(f"{indent}offset += {sz}")
    elif _is_enum(field.type_name, idl):
        if field.array_size is not None:
            w(f"{indent}{var_prefix}{fname} = []")
            w(f"{indent}for _ in range({field.array_size}):")
            w(f"{indent}    {var_prefix}{fname}.append(struct.unpack_from(\"<I\", data, offset)[0])")
            w(f"{indent}    offset += 4")
        else:
            w(f"{indent}{var_prefix}{fname} = struct.unpack_from(\"<I\", data, offset)[0]")
            w(f"{indent}offset += 4")
    elif _is_struct(field.type_name, idl):
        st = _find_struct(field.type_name, idl)
        st_size = _struct_wire_size(st, idl)
        if field.array_size is not None:
            w(f"{indent}{var_prefix}{fname} = []")
            w(f"{indent}for _ in range({field.array_size}):")
            w(f"{indent}    {var_prefix}{fname}.append({field.type_name}.unpack(data, offset))")
            w(f"{indent}    offset += {st_size}")
        else:
            w(f"{indent}{var_prefix}{fname} = {field.type_name}.unpack(data, offset)")
            w(f"{indent}offset += {st_size}")


# ── Param pack/unpack (for methods and notifications) ──────────────────


def _emit_param_pack(w, p: Param, idl: IdlFile, indent: str = "    "):
    """Emit code to pack a parameter value into `req` bytes."""
    if p.type_name == "string":
        w(f"{indent}{p.name}_bytes = {p.name}.encode(\"utf-8\")[:{p.array_size}]")
        total = p.array_size + 1
        w(f"{indent}req += {p.name}_bytes + b\"\\x00\" * ({total} - len({p.name}_bytes))")
    elif p.type_name in PY_STRUCT_FMT:
        fmt = PY_STRUCT_FMT[p.type_name]
        if p.array_size is not None:
            w(f"{indent}if len({p.name}) != {p.array_size}:")
            w(f"{indent}    raise ValueError(\"{p.name} must have length {p.array_size}\")")
            w(f"{indent}for _item in {p.name}:")
            w(f"{indent}    req += struct.pack(\"<{fmt}\", _item)")
        else:
            w(f"{indent}req += struct.pack(\"<{fmt}\", {p.name})")
    elif _is_enum(p.type_name, idl):
        if p.array_size is not None:
            w(f"{indent}if len({p.name}) != {p.array_size}:")
            w(f"{indent}    raise ValueError(\"{p.name} must have length {p.array_size}\")")
            w(f"{indent}for _item in {p.name}:")
            w(f"{indent}    req += struct.pack(\"<I\", _item)")
        else:
            w(f"{indent}req += struct.pack(\"<I\", {p.name})")
    elif _is_struct(p.type_name, idl):
        if p.array_size is not None:
            w(f"{indent}if len({p.name}) != {p.array_size}:")
            w(f"{indent}    raise ValueError(\"{p.name} must have length {p.array_size}\")")
            w(f"{indent}for _item in {p.name}:")
            w(f"{indent}    req += _item.pack()")
        else:
            w(f"{indent}req += {p.name}.pack()")


def _emit_param_unpack(w, p: Param, idl: IdlFile, data_var: str = "resp",
                       indent: str = "        "):
    """Emit code to unpack a parameter from `data_var` at `offset`."""
    if p.type_name == "string":
        total = p.array_size + 1
        w(f"{indent}{p.name} = {data_var}[offset:offset+{total}].split(b\"\\x00\", 1)[0].decode(\"utf-8\")")
        w(f"{indent}offset += {total}")
    elif p.type_name in PY_STRUCT_FMT:
        fmt = PY_STRUCT_FMT[p.type_name]
        sz = PY_TYPE_SIZE[p.type_name]
        is_bool = p.type_name == "bool"
        if p.array_size is not None:
            w(f"{indent}{p.name} = []")
            w(f"{indent}for _ in range({p.array_size}):")
            if is_bool:
                w(f"{indent}    {p.name}.append(bool(struct.unpack_from(\"<{fmt}\", {data_var}, offset)[0]))")
            else:
                w(f"{indent}    {p.name}.append(struct.unpack_from(\"<{fmt}\", {data_var}, offset)[0])")
            w(f"{indent}    offset += {sz}")
        else:
            if is_bool:
                w(f"{indent}{p.name} = bool(struct.unpack_from(\"<{fmt}\", {data_var}, offset)[0])")
            else:
                w(f"{indent}{p.name} = struct.unpack_from(\"<{fmt}\", {data_var}, offset)[0]")
            w(f"{indent}offset += {sz}")
    elif _is_enum(p.type_name, idl):
        if p.array_size is not None:
            w(f"{indent}{p.name} = []")
            w(f"{indent}for _ in range({p.array_size}):")
            w(f"{indent}    {p.name}.append(struct.unpack_from(\"<I\", {data_var}, offset)[0])")
            w(f"{indent}    offset += 4")
        else:
            w(f"{indent}{p.name} = struct.unpack_from(\"<I\", {data_var}, offset)[0]")
            w(f"{indent}offset += 4")
    elif _is_struct(p.type_name, idl):
        st = _find_struct(p.type_name, idl)
        st_size = _struct_wire_size(st, idl)
        if p.array_size is not None:
            w(f"{indent}{p.name} = []")
            w(f"{indent}for _ in range({p.array_size}):")
            w(f"{indent}    {p.name}.append({p.type_name}.unpack({data_var}, offset))")
            w(f"{indent}    offset += {st_size}")
        else:
            w(f"{indent}{p.name} = {p.type_name}.unpack({data_var}, offset)")
            w(f"{indent}offset += {st_size}")


def _param_default(p: Param, idl: IdlFile) -> str:
    """Return a default value expression for a parameter."""
    if p.type_name == "string":
        return '""'
    if p.type_name in PY_STRUCT_FMT:
        if p.array_size is not None:
            if p.type_name.startswith("float"):
                return f"[0.0] * {p.array_size}"
            if p.type_name == "bool":
                return f"[False] * {p.array_size}"
            return f"[0] * {p.array_size}"
        if p.type_name.startswith("float"):
            return "0.0"
        if p.type_name == "bool":
            return "False"
        return "0"
    if _is_enum(p.type_name, idl):
        if p.array_size is not None:
            return f"[0] * {p.array_size}"
        return "0"
    if _is_struct(p.type_name, idl):
        if p.array_size is not None:
            return f"[{p.type_name}() for _ in range({p.array_size})]"
        return f"{p.type_name}()"
    return "0"


# ── Main entry point ────────────────────────────────────────────────────


def emit_python_client(idl: IdlFile) -> str:
    """Generate a typed Python client module from the IDL AST.

    Returns the generated Python source as a string.
    """
    name = idl.service_name
    service_id = fnv1a_32(name)
    lines: list[str] = []
    w = lines.append

    w('"""Generated by ipcgen -- do not edit."""')
    w("import struct")
    w("from typing import Optional, Callable")
    w("from aether_ipc import AetherClient, IPC_SUCCESS")
    w("")
    w(f'SERVICE_NAME = "{name}"')
    w(f"SERVICE_ID = 0x{service_id:08x}")

    # ── Enums ──
    if idl.enums:
        w("")
        w("")
        w("# -- Enums --")
        for enum_def in idl.enums:
            w("")
            w(f"class {enum_def.name}:")
            for val in enum_def.values:
                w(f"    {val.name} = {val.value}")

    # ── Structs ──
    if idl.structs:
        w("")
        w("")
        w("# -- Structs --")
        for struct_def in idl.structs:
            wire_size = _struct_wire_size(struct_def, idl)
            field_names = [f.name for f in struct_def.fields]

            w("")
            w(f"class {struct_def.name}:")
            w(f"    __slots__ = ({', '.join(repr(n) for n in field_names)},)")
            w(f"    WIRE_SIZE = {wire_size}")
            w("")

            # __init__
            init_params = []
            for f in struct_def.fields:
                if f.type_name == "string":
                    init_params.append(f'{f.name}: str = ""')
                elif f.type_name in PY_STRUCT_FMT:
                    if f.array_size is not None:
                        init_params.append(f"{f.name}: list = None")
                    elif f.type_name.startswith("float"):
                        init_params.append(f"{f.name}: float = 0.0")
                    elif f.type_name == "bool":
                        init_params.append(f"{f.name}: bool = False")
                    else:
                        init_params.append(f"{f.name}: int = 0")
                elif _is_enum(f.type_name, idl):
                    if f.array_size is not None:
                        init_params.append(f"{f.name}: list = None")
                    else:
                        init_params.append(f"{f.name}: int = 0")
                elif _is_struct(f.type_name, idl):
                    if f.array_size is not None:
                        init_params.append(f"{f.name}: list = None")
                    else:
                        init_params.append(f"{f.name} = None")
                else:
                    init_params.append(f"{f.name}: int = 0")

            w(f"    def __init__(self, {', '.join(init_params)}):")
            for f in struct_def.fields:
                if f.array_size is not None and f.type_name != "string":
                    if _is_struct(f.type_name, idl):
                        w(f"        self.{f.name} = {f.name} if {f.name} is not None else [{f.type_name}() for _ in range({f.array_size})]")
                    else:
                        default = "0.0" if f.type_name.startswith("float") else "False" if f.type_name == "bool" else "0"
                        w(f"        self.{f.name} = {f.name} if {f.name} is not None else [{default}] * {f.array_size}")
                elif _is_struct(f.type_name, idl) and f.array_size is None:
                    w(f"        self.{f.name} = {f.name} if {f.name} is not None else {f.type_name}()")
                else:
                    w(f"        self.{f.name} = {f.name}")
            w("")

            # pack()
            w("    def pack(self) -> bytes:")
            w("        buf = bytearray()")
            for f in struct_def.fields:
                _emit_field_pack(w, f.name, f, idl)
            w("        return bytes(buf)")
            w("")

            # unpack()
            w("    @classmethod")
            w(f"    def unpack(cls, data: bytes, offset: int = 0) -> \"{struct_def.name}\":")
            for f in struct_def.fields:
                _emit_field_unpack(w, f, idl)
            field_kwargs = ", ".join(f"{f.name}={f.name}" for f in struct_def.fields)
            w(f"        return cls({field_kwargs})")

    # ── Client class ──
    w("")
    w("")
    w(f"class {name}Client:")

    # Method ID constants
    for m in idl.methods:
        w(f"    METHOD_{_snake_case(m.name).upper()} = {m.method_id}")
    for n in idl.notifications:
        w(f"    NOTIFY_{_snake_case(n.name).upper()} = {n.notify_id}")

    w("")
    w("    def __init__(self, client: AetherClient):")
    w("        self._client = client")
    for n in idl.notifications:
        snake = _snake_case(n.name)
        w(f"        self._on_{snake}: Optional[Callable] = None")
    if idl.notifications:
        w("        self._client.set_notification_handler(self._dispatch_notification)")

    # ── Methods ──
    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]
        snake = _snake_case(m.name)

        # Build signature
        sig_parts = ["self"]
        for p in in_params:
            hint = _py_type_hint(p.type_name, p.array_size)
            sig_parts.append(f"{p.name}: {hint}")
        sig_parts.append("timeout_ms: int = 2000")

        # Build return type hint
        ret_parts = ["int"]
        for p in out_params:
            hint = _py_type_hint(p.type_name, p.array_size)
            ret_parts.append(hint)

        w("")
        w(f"    def {snake}({', '.join(sig_parts)}) -> tuple[{', '.join(ret_parts)}]:")

        # Build docstring
        out_names = [p.name for p in out_params]
        w(f'        """Returns (status, {", ".join(out_names)})."""' if out_names
          else f'        """Returns (status,)."""')

        # Marshal [in] params
        if in_params:
            w("        req = bytearray()")
            for p in in_params:
                _emit_param_pack(w, p, idl, indent="        ")
            w("        req = bytes(req)")
        else:
            w("        req = b\"\"")

        # Compute expected response size for out params
        if out_params:
            expected_resp = sum(_param_wire_size(p, idl) for p in out_params)
        else:
            expected_resp = 0

        const_name = f"self.METHOD_{_snake_case(m.name).upper()}"
        w(f"        rc, resp = self._client.call(SERVICE_ID, {const_name}, req, timeout_ms)")

        if out_params:
            defaults = ", ".join(_param_default(p, idl) for p in out_params)
            w(f"        if rc != IPC_SUCCESS or len(resp) < {expected_resp}:")
            w(f"            return (rc, {defaults})")

            # Unmarshal [out] params
            w("        offset = 0")
            for p in out_params:
                _emit_param_unpack(w, p, idl, data_var="resp", indent="        ")

            out_vars = ", ".join(p.name for p in out_params)
            w(f"        return (rc, {out_vars})")
        else:
            w("        return (rc,)")

    # ── Notification registration ──
    for n in idl.notifications:
        snake = _snake_case(n.name)
        param_hints = []
        for p in n.params:
            hint = _py_type_hint(p.type_name, p.array_size)
            param_hints.append(hint)
        if param_hints:
            callable_hint = f"Callable[[{', '.join(param_hints)}], None]"
        else:
            callable_hint = "Callable[[], None]"

        w("")
        w(f"    def on_{snake}(self, handler: {callable_hint}):")
        w(f'        """Register handler for {n.name} notifications."""')
        w(f"        self._on_{snake} = handler")

    # ── Notification dispatch ──
    w("")
    w("    def _dispatch_notification(self, service_id: int, message_id: int, payload: bytes):")
    w("        if service_id != SERVICE_ID:")
    w("            return")

    if idl.notifications:
        for i, n in enumerate(idl.notifications):
            snake = _snake_case(n.name)
            const = f"self.NOTIFY_{snake.upper()}"
            keyword = "if" if i == 0 else "elif"
            w(f"        {keyword} message_id == {const} and self._on_{snake}:")
            expected = sum(_param_wire_size(p, idl) for p in n.params)
            if n.params:
                w(f"            if len(payload) >= {expected}:")
                w("                offset = 0")
                for p in n.params:
                    _emit_param_unpack(w, p, idl, data_var="payload", indent="                ")
                args = ", ".join(p.name for p in n.params)
                w(f"                self._on_{snake}({args})")
            else:
                w(f"            self._on_{snake}()")
    else:
        w("        pass")

    w("")

    return "\n".join(lines)
