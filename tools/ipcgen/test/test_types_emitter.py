"""Tests for the types header emitter and struct/enum integration."""

import pytest

from ipcgen.emitter import emit_types_h, emit_server_h, emit_server_cpp, emit_client_h, emit_client_cpp

from .conftest import parse


class TestTypesEmitter:
    """Generated types header and struct/enum integration with server/client."""

    def test_enum_generation(self):
        """Enum is emitted with uint32_t underlying type."""
        idl = parse("""
            enum DeviceType { Unknown = 0, USB = 1, Bluetooth = 2, };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "enum DeviceType : uint32_t" in h
        assert "Unknown = 0," in h
        assert "USB = 1," in h
        assert "Bluetooth = 2," in h

    def test_struct_generation(self):
        """Struct is emitted with correct field types."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "struct Point" in h
        assert "uint32_t x;" in h
        assert "uint32_t y;" in h

    def test_struct_with_enum_field(self):
        """Struct field that is an enum type uses the enum name directly."""
        idl = parse("""
            enum Color { Red = 0, Green = 1, };
            struct Pixel { uint32 x; Color color; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "Color color;" in h

    def test_types_header_namespace(self):
        """Types header wraps definitions in ms::ipc namespace."""
        idl = parse("""
            enum E { A = 0, };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "namespace ms::ipc" in h
        assert "} // namespace ms::ipc" in h

    def test_types_header_includes(self):
        """Types header includes <cstdint> and pragma once."""
        idl = parse("""
            enum E { A = 0, };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "#pragma once" in h
        assert "#include <cstdint>" in h

    def test_server_h_includes_types(self):
        """Server header includes Types.h when types are defined."""
        idl = parse("""
            enum E { A = 0, };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_server_h(idl)
        assert '#include "FooTypes.h"' in h

    def test_client_h_includes_types(self):
        """Client header includes Types.h when types are defined."""
        idl = parse("""
            enum E { A = 0, };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_client_h(idl)
        assert '#include "FooTypes.h"' in h

    def test_no_types_no_include(self):
        """Headers do NOT include Types.h when no types are defined."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_server_h(idl)
        assert "Types.h" not in h
        h = emit_client_h(idl)
        assert "Types.h" not in h

    def test_struct_param_in_server_h(self):
        """Server handler uses struct type in signature."""
        idl = parse("""
            struct Info { uint32 id; uint32 status; };
            service Foo {
                [method=1]
                int GetInfo([in] uint32 id, [out] Info* info);
            };
        """)
        h = emit_server_h(idl)
        assert "virtual int handleGetInfo(uint32_t id, Info *info) = 0;" in h

    def test_struct_param_in_server_cpp(self):
        """Server cpp correctly marshals struct with sizeof."""
        idl = parse("""
            struct Info { uint32 id; uint32 status; };
            service Foo {
                [method=1]
                int GetInfo([in] uint32 id, [out] Info* info);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "Info info;" in cpp
        assert "sizeof(info)" in cpp

    def test_struct_param_in_client_h(self):
        """Client method uses struct type in signature."""
        idl = parse("""
            struct Info { uint32 id; uint32 status; };
            service Foo {
                [method=1]
                int GetInfo([in] uint32 id, [out] Info* info);
            };
        """)
        h = emit_client_h(idl)
        assert "Info *info" in h

    def test_struct_in_notification(self):
        """Notification callback uses struct type."""
        idl = parse("""
            struct Info { uint32 id; uint32 status; };
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void InfoChanged([in] Info info);
            };
        """)
        h = emit_client_h(idl)
        assert "virtual void onInfoChanged(Info info) {}" in h
        cpp = emit_client_cpp(idl)
        assert "Info info;" in cpp
