#!/usr/bin/env python3
"""
ipcgen — IDL-to-C++ code generator for ms-ipc.

Reads a minimal IDL file and emits:
  {Name}Skeleton.h / .cpp   (server-side, inherits ServiceBase)
  {Name}Client.h   / .cpp   (client-side, inherits ClientBase)

Usage:
    python3 ipcgen.py <input.idl> --outdir <dir>
"""

import argparse
import os
import re
import sys
from dataclasses import dataclass, field
from typing import List, Optional


# ── IDL type → C++ type mapping ──────────────────────────────────────

TYPE_MAP = {
    "uint8":   "uint8_t",
    "uint16":  "uint16_t",
    "uint32":  "uint32_t",
    "uint64":  "uint64_t",
    "int8":    "int8_t",
    "int16":   "int16_t",
    "int32":   "int32_t",
    "int64":   "int64_t",
    "float32": "float",
    "float64": "double",
    "bool":    "bool",
}


# ── AST ──────────────────────────────────────────────────────────────

@dataclass
class Param:
    direction: str   # "in" or "out"
    type_name: str   # IDL type name (e.g. "uint32")
    name: str        # parameter name
    is_pointer: bool # True for [out] params


@dataclass
class Method:
    name: str
    method_id: int
    params: List[Param]


@dataclass
class Notification:
    name: str
    notify_id: int
    params: List[Param]


@dataclass
class IdlFile:
    service_name: str
    methods: List[Method] = field(default_factory=list)
    notifications: List[Notification] = field(default_factory=list)


# ── FNV-1a hash (32-bit) ────────────────────────────────────────────

def fnv1a_32(s: str) -> int:
    h = 0x811c9dc5
    for b in s.encode("utf-8"):
        h ^= b
        h = (h * 0x01000193) & 0xFFFFFFFF
    return h


# ── Lexer ────────────────────────────────────────────────────────────

# Token types
TOK_KEYWORD   = "KEYWORD"
TOK_IDENT     = "IDENT"
TOK_NUMBER    = "NUMBER"
TOK_SYMBOL    = "SYMBOL"
TOK_ATTR      = "ATTR"       # e.g. [method=1], [in], [out], [notify=2]
TOK_EOF       = "EOF"

KEYWORDS = {"service", "notifications", "int", "void"}

@dataclass
class Token:
    kind: str
    value: str
    line: int


def tokenize(text: str) -> List[Token]:
    tokens = []
    i = 0
    line = 1
    n = len(text)

    while i < n:
        # Newlines
        if text[i] == "\n":
            line += 1
            i += 1
            continue

        # Whitespace
        if text[i] in " \t\r":
            i += 1
            continue

        # Single-line comment
        if text[i:i+2] == "//":
            while i < n and text[i] != "\n":
                i += 1
            continue

        # Block comment
        if text[i:i+2] == "/*":
            end = text.find("*/", i + 2)
            if end == -1:
                raise SyntaxError(f"Line {line}: unterminated block comment")
            line += text[i:end+2].count("\n")
            i = end + 2
            continue

        # Attribute: [something] or [something=value]
        if text[i] == "[":
            j = text.find("]", i)
            if j == -1:
                raise SyntaxError(f"Line {line}: unterminated attribute")
            tokens.append(Token(TOK_ATTR, text[i+1:j].strip(), line))
            i = j + 1
            continue

        # Symbols
        if text[i] in "{}();,*":
            tokens.append(Token(TOK_SYMBOL, text[i], line))
            i += 1
            continue

        # Number
        if text[i].isdigit():
            j = i
            while j < n and text[j].isdigit():
                j += 1
            tokens.append(Token(TOK_NUMBER, text[i:j], line))
            i = j
            continue

        # Identifier / keyword
        if text[i].isalpha() or text[i] == "_":
            j = i
            while j < n and (text[j].isalnum() or text[j] == "_"):
                j += 1
            word = text[i:j]
            kind = TOK_KEYWORD if word in KEYWORDS else TOK_IDENT
            tokens.append(Token(kind, word, line))
            i = j
            continue

        raise SyntaxError(f"Line {line}: unexpected character '{text[i]}'")

    tokens.append(Token(TOK_EOF, "", line))
    return tokens


# ── Parser ───────────────────────────────────────────────────────────

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0

    def peek(self) -> Token:
        return self.tokens[self.pos]

    def advance(self) -> Token:
        tok = self.tokens[self.pos]
        self.pos += 1
        return tok

    def expect(self, kind: str, value: Optional[str] = None) -> Token:
        tok = self.advance()
        if tok.kind != kind:
            raise SyntaxError(
                f"Line {tok.line}: expected {kind}"
                f"{f' {value!r}' if value else ''}, got {tok.kind} {tok.value!r}")
        if value is not None and tok.value != value:
            raise SyntaxError(
                f"Line {tok.line}: expected {value!r}, got {tok.value!r}")
        return tok

    def parse(self) -> IdlFile:
        idl = IdlFile(service_name="")

        while self.peek().kind != TOK_EOF:
            tok = self.peek()

            if tok.kind == TOK_KEYWORD and tok.value == "service":
                self._parse_service(idl)
            elif tok.kind == TOK_KEYWORD and tok.value == "notifications":
                self._parse_notifications(idl)
            else:
                raise SyntaxError(
                    f"Line {tok.line}: expected 'service' or 'notifications', "
                    f"got {tok.value!r}")

        if not idl.service_name:
            raise SyntaxError("No service block found")

        return idl

    def _parse_service(self, idl: IdlFile):
        self.expect(TOK_KEYWORD, "service")
        name_tok = self.expect(TOK_IDENT)
        if idl.service_name and idl.service_name != name_tok.value:
            raise SyntaxError(
                f"Line {name_tok.line}: service name mismatch: "
                f"{name_tok.value!r} vs {idl.service_name!r}")
        idl.service_name = name_tok.value

        self.expect(TOK_SYMBOL, "{")
        while self.peek().value != "}":
            idl.methods.append(self._parse_method())
        self.expect(TOK_SYMBOL, "}")
        self.expect(TOK_SYMBOL, ";")

    def _parse_notifications(self, idl: IdlFile):
        self.expect(TOK_KEYWORD, "notifications")
        name_tok = self.expect(TOK_IDENT)
        if idl.service_name and idl.service_name != name_tok.value:
            raise SyntaxError(
                f"Line {name_tok.line}: notifications name mismatch: "
                f"{name_tok.value!r} vs {idl.service_name!r}")
        idl.service_name = name_tok.value

        self.expect(TOK_SYMBOL, "{")
        while self.peek().value != "}":
            idl.notifications.append(self._parse_notification())
        self.expect(TOK_SYMBOL, "}")
        self.expect(TOK_SYMBOL, ";")

    def _parse_method(self) -> Method:
        # [method=N]
        attr_tok = self.expect(TOK_ATTR)
        m = re.match(r"method\s*=\s*(\d+)", attr_tok.value)
        if not m:
            raise SyntaxError(
                f"Line {attr_tok.line}: expected [method=N], got [{attr_tok.value}]")
        method_id = int(m.group(1))

        # return type (must be "int")
        self.expect(TOK_KEYWORD, "int")

        # method name
        name_tok = self.expect(TOK_IDENT)

        # params
        params = self._parse_params()
        self.expect(TOK_SYMBOL, ";")

        return Method(name=name_tok.value, method_id=method_id, params=params)

    def _parse_notification(self) -> Notification:
        # [notify=N]
        attr_tok = self.expect(TOK_ATTR)
        m = re.match(r"notify\s*=\s*(\d+)", attr_tok.value)
        if not m:
            raise SyntaxError(
                f"Line {attr_tok.line}: expected [notify=N], got [{attr_tok.value}]")
        notify_id = int(m.group(1))

        # return type (must be "void")
        self.expect(TOK_KEYWORD, "void")

        # notification name
        name_tok = self.expect(TOK_IDENT)

        # params
        params = self._parse_params()
        self.expect(TOK_SYMBOL, ";")

        # All notification params must be [in]
        for p in params:
            if p.direction != "in":
                raise SyntaxError(
                    f"Line {attr_tok.line}: notification params must be [in]")

        return Notification(name=name_tok.value, notify_id=notify_id, params=params)

    def _parse_params(self) -> List[Param]:
        self.expect(TOK_SYMBOL, "(")
        params = []
        if self.peek().value != ")":
            params.append(self._parse_param())
            while self.peek().value == ",":
                self.advance()  # consume ","
                params.append(self._parse_param())
        self.expect(TOK_SYMBOL, ")")
        return params

    def _parse_param(self) -> Param:
        # [in] or [out]
        attr_tok = self.expect(TOK_ATTR)
        direction = attr_tok.value.strip()
        if direction not in ("in", "out"):
            raise SyntaxError(
                f"Line {attr_tok.line}: expected [in] or [out], got [{direction}]")

        # type name
        type_tok = self.advance()
        if type_tok.value not in TYPE_MAP:
            raise SyntaxError(
                f"Line {type_tok.line}: unknown type {type_tok.value!r}")
        type_name = type_tok.value

        # optional pointer '*'
        is_pointer = False
        if self.peek().value == "*":
            self.advance()
            is_pointer = True

        # param name
        name_tok = self.expect(TOK_IDENT)

        # Validate: [out] must be pointer, [in] must not
        if direction == "out" and not is_pointer:
            raise SyntaxError(
                f"Line {name_tok.line}: [out] param '{name_tok.value}' must be a pointer")
        if direction == "in" and is_pointer:
            raise SyntaxError(
                f"Line {name_tok.line}: [in] param '{name_tok.value}' must not be a pointer")

        return Param(direction=direction, type_name=type_name,
                     name=name_tok.value, is_pointer=is_pointer)


# ── C++ Emitter ──────────────────────────────────────────────────────

def cpp_type(idl_type: str) -> str:
    return TYPE_MAP[idl_type]


def sizeof_expr(idl_type: str) -> str:
    return f"sizeof({cpp_type(idl_type)})"


def emit_server_h(idl: IdlFile) -> str:
    name = idl.service_name
    service_id = fnv1a_32(name)
    lines = []
    w = lines.append

    w(f"// Auto-generated by ipcgen — do not edit.")
    w(f"#pragma once")
    w(f"#include \"ServiceBase.h\"")
    w(f"#include <cstdint>")
    w(f"#include <cstring>")
    w(f"#include <vector>")
    w(f"")
    w(f"namespace ms::ipc")
    w(f"{{")
    w(f"")
    w(f"class {name} : public ServiceBase")
    w(f"{{")
    w(f"public:")
    w(f"    using ServiceBase::ServiceBase;")
    w(f"")
    w(f"    static constexpr uint32_t kServiceId = 0x{service_id:08x}u;")
    w(f"")
    w(f"protected:")

    # Pure virtual handlers
    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]
        parts = []
        for p in in_params:
            parts.append(f"{cpp_type(p.type_name)} {p.name}")
        for p in out_params:
            parts.append(f"{cpp_type(p.type_name)} *{p.name}")
        sig = ", ".join(parts)
        w(f"    virtual int handle{m.name}({sig}) = 0;")

    w(f"")

    # Notification senders
    for n in idl.notifications:
        parts = []
        for p in n.params:
            parts.append(f"{cpp_type(p.type_name)} {p.name}")
        sig = ", ".join(parts)
        w(f"    int notify{n.name}({sig});")

    w(f"")
    w(f"    int onRequest(uint32_t messageId, const std::vector<uint8_t> &request,")
    w(f"                  std::vector<uint8_t> *response) override;")
    w(f"}};")
    w(f"")
    w(f"}} // namespace ms::ipc")
    w(f"")

    return "\n".join(lines)


def emit_server_cpp(idl: IdlFile) -> str:
    name = idl.service_name
    lines = []
    w = lines.append

    w(f"// Auto-generated by ipcgen — do not edit.")
    w(f"#include \"{name}.h\"")
    w(f"")
    w(f"namespace ms::ipc")
    w(f"{{")
    w(f"")

    # onRequest
    w(f"int {name}::onRequest(uint32_t messageId,")
    pad = " " * len(f"int {name}::onRequest(")
    w(f"{pad}const std::vector<uint8_t> &request,")
    w(f"{pad}std::vector<uint8_t> *response)")
    w(f"{{")
    w(f"    switch (messageId)")
    w(f"    {{")

    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]

        w(f"    case {m.method_id}: // {m.name}")
        w(f"    {{")

        # Unmarshal [in] params
        if in_params:
            offset = 0
            for p in in_params:
                ct = cpp_type(p.type_name)
                w(f"        {ct} {p.name};")
                w(f"        std::memcpy(&{p.name}, request.data() + {offset}, sizeof({p.name}));")
                offset_expr = f"sizeof({ct})"
                if offset == 0:
                    offset = offset_expr
                else:
                    offset = f"{offset} + {offset_expr}"

        # Declare [out] params
        for p in out_params:
            w(f"        {cpp_type(p.type_name)} {p.name};")

        # Call handler
        call_args = []
        for p in in_params:
            call_args.append(p.name)
        for p in out_params:
            call_args.append(f"&{p.name}")
        w(f"        int _rc = handle{m.name}({', '.join(call_args)});")

        # Marshal [out] params
        if out_params:
            # Calculate total size
            size_parts = [f"sizeof({p.name})" for p in out_params]
            total_size = " + ".join(size_parts)
            w(f"        response->resize({total_size});")
            offset = "0"
            for i, p in enumerate(out_params):
                if i == 0:
                    w(f"        std::memcpy(response->data(), &{p.name}, sizeof({p.name}));")
                    offset = f"sizeof({p.name})"
                else:
                    w(f"        std::memcpy(response->data() + {offset}, &{p.name}, sizeof({p.name}));")
                    offset = f"{offset} + sizeof({p.name})"

        w(f"        return _rc;")
        w(f"    }}")

    w(f"    default:")
    w(f"        return IPC_ERR_INVALID_METHOD;")
    w(f"    }}")
    w(f"}}")

    # Notification senders
    for n in idl.notifications:
        w(f"")
        parts = []
        for p in n.params:
            parts.append(f"{cpp_type(p.type_name)} {p.name}")
        sig = ", ".join(parts)

        w(f"int {name}::notify{n.name}({sig})")
        w(f"{{")

        if n.params:
            size_parts = [f"sizeof({p.name})" for p in n.params]
            total_size = " + ".join(size_parts)
            w(f"    std::vector<uint8_t> payload({total_size});")
            offset = "0"
            for i, p in enumerate(n.params):
                if i == 0:
                    w(f"    std::memcpy(payload.data(), &{p.name}, sizeof({p.name}));")
                    offset = f"sizeof({p.name})"
                else:
                    w(f"    std::memcpy(payload.data() + {offset}, &{p.name}, sizeof({p.name}));")
                    offset = f"{offset} + sizeof({p.name})"
            w(f"    return sendNotify(kServiceId, {n.notify_id}, payload.data(),")
            w(f"                      static_cast<uint32_t>(payload.size()));")
        else:
            w(f"    return sendNotify(kServiceId, {n.notify_id}, nullptr, 0);")

        w(f"}}")

    w(f"")
    w(f"}} // namespace ms::ipc")
    w(f"")

    return "\n".join(lines)


def emit_client_h(idl: IdlFile) -> str:
    name = idl.service_name
    service_id = fnv1a_32(name)
    lines = []
    w = lines.append

    w(f"// Auto-generated by ipcgen — do not edit.")
    w(f"#pragma once")
    w(f"#include \"ClientBase.h\"")
    w(f"#include <cstdint>")
    w(f"#include <cstring>")
    w(f"#include <vector>")
    w(f"")
    w(f"namespace ms::ipc")
    w(f"{{")
    w(f"")
    w(f"class {name} : public ClientBase")
    w(f"{{")
    w(f"public:")
    w(f"    using ClientBase::ClientBase;")
    w(f"")
    w(f"    static constexpr uint32_t kServiceId = 0x{service_id:08x}u;")
    w(f"")

    # Typed RPC methods
    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]
        parts = []
        for p in in_params:
            parts.append(f"{cpp_type(p.type_name)} {p.name}")
        for p in out_params:
            parts.append(f"{cpp_type(p.type_name)} *{p.name}")
        parts.append("uint32_t timeoutMs = 2000")
        sig = ", ".join(parts)
        w(f"    int {m.name}({sig});")

    w(f"")
    w(f"protected:")

    # Notification callbacks
    for n in idl.notifications:
        parts = []
        for p in n.params:
            parts.append(f"{cpp_type(p.type_name)} {p.name}")
        sig = ", ".join(parts)
        w(f"    virtual void on{n.name}({sig}) {{}}")

    w(f"")
    w(f"    void onNotification(uint32_t serviceId, uint32_t messageId,")
    w(f"                        const std::vector<uint8_t> &payload) override;")
    w(f"}};")
    w(f"")
    w(f"}} // namespace ms::ipc")
    w(f"")

    return "\n".join(lines)


def emit_client_cpp(idl: IdlFile) -> str:
    name = idl.service_name
    lines = []
    w = lines.append

    w(f"// Auto-generated by ipcgen — do not edit.")
    w(f"#include \"{name}.h\"")
    w(f"")
    w(f"namespace ms::ipc")
    w(f"{{")

    # Typed RPC methods
    for m in idl.methods:
        in_params = [p for p in m.params if p.direction == "in"]
        out_params = [p for p in m.params if p.direction == "out"]

        parts = []
        for p in in_params:
            parts.append(f"{cpp_type(p.type_name)} {p.name}")
        for p in out_params:
            parts.append(f"{cpp_type(p.type_name)} *{p.name}")
        parts.append("uint32_t timeoutMs")
        sig = ", ".join(parts)

        w(f"")
        w(f"int {name}::{m.name}({sig})")
        w(f"{{")

        # Marshal [in] params
        if in_params:
            size_parts = [f"sizeof({p.name})" for p in in_params]
            total_size = " + ".join(size_parts)
            w(f"    std::vector<uint8_t> request({total_size});")
            offset = "0"
            for i, p in enumerate(in_params):
                if i == 0:
                    w(f"    std::memcpy(request.data(), &{p.name}, sizeof({p.name}));")
                    offset = f"sizeof({p.name})"
                else:
                    w(f"    std::memcpy(request.data() + {offset}, &{p.name}, sizeof({p.name}));")
                    offset = f"{offset} + sizeof({p.name})"
        else:
            w(f"    std::vector<uint8_t> request;")

        w(f"")
        w(f"    std::vector<uint8_t> response;")
        w(f"    int _rc = call(kServiceId, {m.method_id}, request, &response, timeoutMs);")

        # Unmarshal [out] params
        if out_params:
            w(f"")
            # Calculate minimum response size
            size_parts = [f"sizeof(*{p.name})" for p in out_params]
            min_size = " + ".join(size_parts)
            w(f"    if (_rc == IPC_SUCCESS && response.size() >= {min_size})")
            w(f"    {{")
            offset = "0"
            for i, p in enumerate(out_params):
                if i == 0:
                    w(f"        if ({p.name})")
                    w(f"            std::memcpy({p.name}, response.data(), sizeof(*{p.name}));")
                    offset = f"sizeof(*{p.name})"
                else:
                    w(f"        if ({p.name})")
                    w(f"            std::memcpy({p.name}, response.data() + {offset}, sizeof(*{p.name}));")
                    offset = f"{offset} + sizeof(*{p.name})"
            w(f"    }}")

        w(f"    return _rc;")
        w(f"}}")

    # onNotification
    w(f"")
    w(f"void {name}::onNotification(uint32_t serviceId, uint32_t messageId,")
    pad = " " * len(f"void {name}::onNotification(")
    w(f"{pad}const std::vector<uint8_t> &payload)")
    w(f"{{")
    w(f"    if (serviceId != kServiceId) return;")
    w(f"")
    w(f"    switch (messageId)")
    w(f"    {{")

    for n in idl.notifications:
        w(f"    case {n.notify_id}: // {n.name}")
        w(f"    {{")
        offset = "0"
        for i, p in enumerate(n.params):
            ct = cpp_type(p.type_name)
            w(f"        {ct} {p.name};")
            if i == 0:
                w(f"        std::memcpy(&{p.name}, payload.data(), sizeof({p.name}));")
                offset = f"sizeof({p.name})"
            else:
                w(f"        std::memcpy(&{p.name}, payload.data() + {offset}, sizeof({p.name}));")
                offset = f"{offset} + sizeof({p.name})"
        call_args = ", ".join(p.name for p in n.params)
        w(f"        on{n.name}({call_args});")
        w(f"        break;")
        w(f"    }}")

    w(f"    default:")
    w(f"        break;")
    w(f"    }}")
    w(f"}}")
    w(f"")
    w(f"}} // namespace ms::ipc")
    w(f"")

    return "\n".join(lines)


# ── Main ─────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="ms-ipc IDL code generator")
    parser.add_argument("idl", help="Input .idl file")
    parser.add_argument("--outdir", required=True, help="Output directory")
    args = parser.parse_args()

    with open(args.idl) as f:
        text = f.read()

    tokens = tokenize(text)
    p = Parser(tokens)
    idl = p.parse()

    name = idl.service_name
    server_dir = os.path.join(args.outdir, "server")
    client_dir = os.path.join(args.outdir, "client")
    os.makedirs(server_dir, exist_ok=True)
    os.makedirs(client_dir, exist_ok=True)

    files = [
        (server_dir, f"{name}.h",   emit_server_h(idl)),
        (server_dir, f"{name}.cpp", emit_server_cpp(idl)),
        (client_dir, f"{name}.h",   emit_client_h(idl)),
        (client_dir, f"{name}.cpp", emit_client_cpp(idl)),
    ]

    for directory, filename, content in files:
        path = os.path.join(directory, filename)
        with open(path, "w") as f:
            f.write(content)
        print(f"  wrote {path}")

    print(f"\nGenerated {len(files)} files for service '{name}' "
          f"(serviceId=0x{fnv1a_32(name):08x})")


if __name__ == "__main__":
    main()
