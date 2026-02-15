"""
ipcgen — IDL-to-C++ code generator for ms-ipc.

Reads a minimal IDL file and emits server and client C++ classes
that inherit from ServiceBase and ClientBase respectively.
"""

from .types import TYPE_MAP, cpp_type, fnv1a_32
from .lexer import (
    tokenize, Token,
    TOK_KEYWORD, TOK_IDENT, TOK_NUMBER, TOK_SYMBOL, TOK_ATTR, TOK_EOF,
)
from .parser import (
    Parser, IdlFile, Method, Notification, Param,
    EnumValue, EnumDef, StructField, StructDef,
)
from .emitter import emit_server_h, emit_server_cpp, emit_client_h, emit_client_cpp, emit_types_h

__all__ = [
    "TYPE_MAP", "cpp_type", "fnv1a_32",
    "tokenize", "Token",
    "TOK_KEYWORD", "TOK_IDENT", "TOK_NUMBER", "TOK_SYMBOL", "TOK_ATTR", "TOK_EOF",
    "Parser", "IdlFile", "Method", "Notification", "Param",
    "EnumValue", "EnumDef", "StructField", "StructDef",
    "emit_server_h", "emit_server_cpp", "emit_client_h", "emit_client_cpp", "emit_types_h",
]
