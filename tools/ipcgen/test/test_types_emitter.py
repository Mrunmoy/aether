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
                int GetInfo([in] uint32 id, [out] Info info);
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
                int GetInfo([in] uint32 id, [out] Info info);
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
                int GetInfo([in] uint32 id, [out] Info info);
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

    # ── Fixed-length array emission ─────────────────────────────────

    def test_struct_array_field_emitted(self):
        """Struct with array field emits std::array<T, N>."""
        idl = parse("""
            struct Mac { uint8[6] bytes; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "std::array<uint8_t, 6> bytes;" in h

    def test_array_types_header_includes(self):
        """Types header includes <array> when struct has array fields."""
        idl = parse("""
            struct Mac { uint8[6] bytes; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "#include <array>" in h

    def test_no_array_no_array_include(self):
        """Types header omits <array> when no array fields exist."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "#include <array>" not in h

    def test_array_param_in_server_h(self):
        """Server handler signature uses std::array for array params."""
        idl = parse("""
            service Foo {
                [method=1]
                int SetKey([in] uint8[16] key);
            };
        """)
        h = emit_server_h(idl)
        assert "std::array<uint8_t, 16> key" in h
        assert "#include <array>" in h

    def test_array_param_in_client_h(self):
        """Client method signature uses raw T* for [out] array params."""
        idl = parse("""
            service Foo {
                [method=1]
                int GetKey([in] uint32 slot, [out] uint8[32] key);
            };
        """)
        h = emit_client_h(idl)
        assert "uint8_t *key" in h

    def test_array_param_marshal_sizeof(self):
        """Generated code uses sizeof() on std::array param — works correctly."""
        idl = parse("""
            service Foo {
                [method=1]
                int SetKey([in] uint8[16] key);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "std::array<uint8_t, 16> key;" in cpp
        assert "sizeof(key)" in cpp

    def test_array_in_notification_callback(self):
        """Notification callback uses std::array for array params."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void DataReady([in] uint8[64] data);
            };
        """)
        h = emit_client_h(idl)
        assert "virtual void onDataReady(std::array<uint8_t, 64> data) {}" in h

    # ── Struct with nested array fields ─────────────────────────────

    def test_struct_multiple_array_fields_emitted(self):
        """Struct with mix of array and scalar fields emits correctly."""
        idl = parse("""
            struct Packet {
                uint32 id;
                uint8[6] mac;
                uint16 flags;
                uint8[16] payload;
            };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "uint32_t id;" in h
        assert "std::array<uint8_t, 6> mac;" in h
        assert "uint16_t flags;" in h
        assert "std::array<uint8_t, 16> payload;" in h

    def test_struct_array_of_struct(self):
        """Struct with array of another struct emits std::array<StructName, N>."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            struct Triangle { Point[3] vertices; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "std::array<Point, 3> vertices;" in h

    def test_struct_array_of_enum(self):
        """Struct with array of enum type emits std::array<EnumName, N>."""
        idl = parse("""
            enum Color { Red = 0, Green = 1, Blue = 2, };
            struct Palette { Color[4] colors; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "std::array<Color, 4> colors;" in h

    def test_nested_struct_with_array_field(self):
        """Struct containing a struct that has array fields — both emit correctly."""
        idl = parse("""
            struct Mac { uint8[6] bytes; };
            struct Device {
                uint32 id;
                Mac mac;
            };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        assert "std::array<uint8_t, 6> bytes;" in h
        assert "Mac mac;" in h  # nested struct, not an array

    def test_struct_array_of_struct_with_array(self):
        """Array of structs where the inner struct also has arrays."""
        idl = parse("""
            struct Entry { uint32 id; uint8[6] mac; };
            struct Table { Entry[8] entries; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        h = emit_types_h(idl)
        # Inner struct
        assert "std::array<uint8_t, 6> mac;" in h
        # Outer struct with array of inner
        assert "std::array<Entry, 8> entries;" in h

    # ── Array params: server-side marshaling ────────────────────────

    def test_server_h_array_handler_signature(self):
        """Server handler uses std::array for [in] and [out] array params."""
        idl = parse("""
            service Foo {
                [method=1]
                int Exchange([in] uint8[16] inBuf, [out] uint8[32] outBuf);
            };
        """)
        h = emit_server_h(idl)
        assert "virtual int handleExchange(std::array<uint8_t, 16> inBuf, uint8_t *outBuf) = 0;" in h

    def test_server_cpp_array_unmarshal(self):
        """Server cpp correctly declares and unmarshals array [in] param."""
        idl = parse("""
            service Foo {
                [method=1]
                int SetKey([in] uint8[16] key);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "std::array<uint8_t, 16> key;" in cpp
        assert "std::memcpy(&key, request.data() + 0, sizeof(key));" in cpp

    def test_server_cpp_mixed_params_offset(self):
        """Server cpp tracks offsets correctly when mixing scalar and array [in] params."""
        idl = parse("""
            service Foo {
                [method=1]
                int Write([in] uint32 offset, [in] uint8[256] data);
            };
        """)
        cpp = emit_server_cpp(idl)
        # First [in] param at offset 0
        assert "std::memcpy(&offset, request.data() + 0, sizeof(offset));" in cpp
        # Second [in] param at sizeof(uint32_t)
        assert "std::memcpy(&data, request.data() + sizeof(uint32_t), sizeof(data));" in cpp

    def test_server_cpp_array_out_param_marshal(self):
        """Server cpp correctly marshals array [out] param in response."""
        idl = parse("""
            service Foo {
                [method=1]
                int GetKey([out] uint8[32] key);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "uint8_t key[32];" in cpp
        assert "response->resize(sizeof(key));" in cpp
        assert "std::memcpy(response->data(), key, sizeof(key));" in cpp

    def test_server_cpp_array_and_scalar_out_params(self):
        """Server cpp marshals multiple [out] params with arrays and scalars."""
        idl = parse("""
            service Foo {
                [method=1]
                int Read([in] uint32 addr, [out] uint8[64] data, [out] uint32 status);
            };
        """)
        cpp = emit_server_cpp(idl)
        # Both [out] params declared — array as C array, scalar as plain var
        assert "uint8_t data[64];" in cpp
        assert "uint32_t status;" in cpp
        # Response resize sums both
        assert "response->resize(sizeof(data) + sizeof(status));" in cpp

    def test_server_notify_array_param(self):
        """Server notification sender correctly marshals array param."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void DataReady([in] uint8[64] data);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "int Foo::notifyDataReady(std::array<uint8_t, 64> data)" in cpp
        assert "std::vector<uint8_t> payload(sizeof(data));" in cpp
        assert "std::memcpy(payload.data(), &data, sizeof(data));" in cpp

    def test_server_notify_mixed_params(self):
        """Server notification sender with scalar + array params."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void Log([in] uint32 level, [in] uint8[128] message);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "int Foo::notifyLog(uint32_t level, std::array<uint8_t, 128> message)" in cpp
        assert "sizeof(level) + sizeof(message)" in cpp

    # ── Array params: client-side marshaling ────────────────────────

    def test_client_h_array_method_signature(self):
        """Client header has correct signature for method with array params."""
        idl = parse("""
            service Foo {
                [method=1]
                int Write([in] uint32 offset, [in] uint8[256] data, [out] uint32 written);
            };
        """)
        h = emit_client_h(idl)
        assert "int Write(uint32_t offset, std::array<uint8_t, 256> data, uint32_t *written, uint32_t timeoutMs = 2000);" in h

    def test_client_cpp_array_in_marshal(self):
        """Client cpp marshals array [in] param correctly."""
        idl = parse("""
            service Foo {
                [method=1]
                int SetKey([in] uint8[16] key);
            };
        """)
        cpp = emit_client_cpp(idl)
        assert "std::vector<uint8_t> request(sizeof(key));" in cpp
        assert "std::memcpy(request.data(), &key, sizeof(key));" in cpp

    def test_client_cpp_mixed_in_marshal_offsets(self):
        """Client cpp tracks offsets for mixed scalar + array [in] params."""
        idl = parse("""
            service Foo {
                [method=1]
                int Write([in] uint32 offset, [in] uint8[256] data);
            };
        """)
        cpp = emit_client_cpp(idl)
        # Request size sums both
        assert "std::vector<uint8_t> request(sizeof(offset) + sizeof(data));" in cpp
        # First param at base, second at offset
        assert "std::memcpy(request.data(), &offset, sizeof(offset));" in cpp
        assert "std::memcpy(request.data() + sizeof(offset), &data, sizeof(data));" in cpp

    def test_client_cpp_array_out_unmarshal(self):
        """Client cpp unmarshals array [out] param correctly."""
        idl = parse("""
            service Foo {
                [method=1]
                int GetKey([out] uint8[32] key);
            };
        """)
        cpp = emit_client_cpp(idl)
        assert "response.size() >= 32 * sizeof(uint8_t)" in cpp
        assert "std::memcpy(key, response.data(), 32 * sizeof(uint8_t));" in cpp

    def test_client_notification_array_unmarshal(self):
        """Client notification dispatch correctly unmarshals array param."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void DataReady([in] uint8[64] data);
            };
        """)
        cpp = emit_client_cpp(idl)
        assert "std::array<uint8_t, 64> data;" in cpp
        assert "std::memcpy(&data, payload.data(), sizeof(data));" in cpp
        assert "onDataReady(data);" in cpp

    def test_client_notification_mixed_unmarshal(self):
        """Client notification dispatch with scalar + array params."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void Log([in] uint32 level, [in] uint8[128] message);
            };
        """)
        cpp = emit_client_cpp(idl)
        assert "uint32_t level;" in cpp
        assert "std::array<uint8_t, 128> message;" in cpp
        assert "std::memcpy(&level, payload.data(), sizeof(level));" in cpp
        assert "std::memcpy(&message, payload.data() + sizeof(level), sizeof(message));" in cpp
        assert "onLog(level, message);" in cpp

    # ── Include guard behavior ──────────────────────────────────────

    def test_server_h_no_array_no_array_include(self):
        """Server header omits <array> when no params use arrays."""
        idl = parse("""
            service Foo {
                [method=1]
                int Get([in] uint32 x, [out] uint32 y);
            };
        """)
        h = emit_server_h(idl)
        assert "#include <array>" not in h

    def test_client_h_no_array_no_array_include(self):
        """Client header omits <array> when no params use arrays."""
        idl = parse("""
            service Foo {
                [method=1]
                int Get([in] uint32 x, [out] uint32 y);
            };
        """)
        h = emit_client_h(idl)
        assert "#include <array>" not in h

    def test_server_h_array_include_from_notification(self):
        """Server header includes <array> when notification has array param."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void Data([in] uint8[32] buf);
            };
        """)
        h = emit_server_h(idl)
        assert "#include <array>" in h

    def test_client_h_array_include_from_notification(self):
        """Client header includes <array> when notification has array param."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void Data([in] uint8[32] buf);
            };
        """)
        h = emit_client_h(idl)
        assert "#include <array>" in h

    # ── Array of struct as param ────────────────────────────────────

    def test_server_h_array_of_struct_param(self):
        """Server handler with array-of-struct param."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo {
                [method=1]
                int SetPoints([in] Point[4] pts);
            };
        """)
        h = emit_server_h(idl)
        assert "std::array<Point, 4> pts" in h
        assert "#include <array>" in h

    def test_client_h_array_of_struct_param(self):
        """Client method with array-of-struct param."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo {
                [method=1]
                int SetPoints([in] Point[4] pts);
            };
        """)
        h = emit_client_h(idl)
        assert "std::array<Point, 4> pts" in h

    def test_server_cpp_array_of_struct_marshal(self):
        """Server cpp unmarshals array-of-struct [in] param."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo {
                [method=1]
                int SetPoints([in] Point[4] pts);
            };
        """)
        cpp = emit_server_cpp(idl)
        assert "std::array<Point, 4> pts;" in cpp
        assert "sizeof(pts)" in cpp

    def test_client_cpp_array_of_struct_marshal(self):
        """Client cpp marshals array-of-struct [in] param."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo {
                [method=1]
                int SetPoints([in] Point[4] pts);
            };
        """)
        cpp = emit_client_cpp(idl)
        assert "std::vector<uint8_t> request(sizeof(pts));" in cpp
        assert "std::memcpy(request.data(), &pts, sizeof(pts));" in cpp
