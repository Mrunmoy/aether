"""Tests for the IDL parser — AST construction and validation errors."""

import pytest

from .conftest import parse, DEVICE_MONITOR_IDL


class TestParser:
    """IDL parsing: AST construction and validation errors."""

    def test_minimal_service(self):
        """Service with a single method, no notifications."""
        idl = parse("""
            service Foo {
                [method=1]
                int DoSomething([in] uint32 x);
            };
        """)
        assert idl.service_name == "Foo"
        assert len(idl.methods) == 1
        assert idl.methods[0].name == "DoSomething"
        assert idl.methods[0].method_id == 1
        assert len(idl.methods[0].params) == 1
        assert idl.notifications == []

    def test_minimal_notifications(self):
        """Notifications block alone still sets the service name."""
        idl = parse("""
            notifications Foo {
                [notify=1]
                void SomethingHappened([in] uint32 id);
            };
        """)
        assert idl.service_name == "Foo"
        assert idl.methods == []
        assert len(idl.notifications) == 1
        assert idl.notifications[0].name == "SomethingHappened"
        assert idl.notifications[0].notify_id == 1

    def test_full_service(self):
        """DeviceMonitor IDL parses completely with all fields correct."""
        idl = parse(DEVICE_MONITOR_IDL)

        assert idl.service_name == "DeviceMonitor"

        # Methods
        assert len(idl.methods) == 2
        m1, m2 = idl.methods

        assert m1.name == "GetDeviceCount"
        assert m1.method_id == 1
        assert len(m1.params) == 1
        assert m1.params[0].direction == "out"
        assert m1.params[0].type_name == "uint32"
        assert m1.params[0].name == "count"
        assert m1.params[0].is_pointer is True

        assert m2.name == "GetDeviceStatus"
        assert m2.method_id == 2
        assert len(m2.params) == 2
        assert m2.params[0].direction == "in"
        assert m2.params[1].direction == "out"

        # Notifications
        assert len(idl.notifications) == 2
        n1, n2 = idl.notifications
        assert n1.name == "DeviceConnected"
        assert n1.notify_id == 1
        assert n2.name == "DeviceDisconnected"
        assert n2.notify_id == 2

    def test_method_no_in_params(self):
        """Method with only [out] params — no [in] to unmarshal."""
        idl = parse("""
            service Foo {
                [method=1]
                int GetCount([out] uint32* count);
            };
        """)
        in_params = [p for p in idl.methods[0].params if p.direction == "in"]
        out_params = [p for p in idl.methods[0].params if p.direction == "out"]
        assert len(in_params) == 0
        assert len(out_params) == 1

    def test_method_no_out_params(self):
        """Method with only [in] params — fire-and-forget style."""
        idl = parse("""
            service Foo {
                [method=1]
                int Fire([in] uint32 id);
            };
        """)
        in_params = [p for p in idl.methods[0].params if p.direction == "in"]
        out_params = [p for p in idl.methods[0].params if p.direction == "out"]
        assert len(in_params) == 1
        assert len(out_params) == 0

    def test_method_multiple_params(self):
        """Method with multiple [in] and [out] params."""
        idl = parse("""
            service Foo {
                [method=1]
                int Transfer([in] uint32 src, [in] uint32 dst,
                             [out] uint32* srcBal, [out] uint32* dstBal);
            };
        """)
        m = idl.methods[0]
        assert len(m.params) == 4
        assert [p.direction for p in m.params] == ["in", "in", "out", "out"]
        assert [p.name for p in m.params] == ["src", "dst", "srcBal", "dstBal"]

    def test_all_scalar_types(self):
        """All 11 supported IDL types parse without error."""
        types = [
            "uint8", "uint16", "uint32", "uint64",
            "int8", "int16", "int32", "int64",
            "float32", "float64", "bool",
        ]
        # Build a service with one [in] param per type.
        methods = []
        for i, t in enumerate(types, start=1):
            methods.append(f"    [method={i}]\n    int M{i}([in] {t} x);")
        body = "\n".join(methods)
        idl = parse(f"service Foo {{\n{body}\n}};")

        assert len(idl.methods) == len(types)
        for m, t in zip(idl.methods, types):
            assert m.params[0].type_name == t

    def test_comments_ignored(self):
        """Comments in IDL are skipped by the parser."""
        idl = parse("""
            // This is a service
            service Foo {
                /* method 1 */
                [method=1]
                int Get([out] uint32* val); // inline comment
            };
        """)
        assert idl.service_name == "Foo"
        assert len(idl.methods) == 1

    def test_empty_input(self):
        """Empty input raises SyntaxError."""
        with pytest.raises(SyntaxError, match="No service block found"):
            parse("")

    def test_name_mismatch(self):
        """Mismatched service and notifications names raise SyntaxError."""
        with pytest.raises(SyntaxError, match="mismatch"):
            parse("""
                service Foo {
                    [method=1]
                    int Get([out] uint32* val);
                };
                notifications Bar {
                    [notify=1]
                    void Event([in] uint32 id);
                };
            """)

    def test_unknown_type(self):
        """Unknown IDL type raises SyntaxError."""
        with pytest.raises(SyntaxError, match="unknown type"):
            parse("""
                service Foo {
                    [method=1]
                    int Get([in] string name);
                };
            """)

    def test_out_param_not_pointer(self):
        """[out] param without pointer raises SyntaxError."""
        with pytest.raises(SyntaxError, match="must be a pointer"):
            parse("""
                service Foo {
                    [method=1]
                    int Get([out] uint32 val);
                };
            """)

    def test_in_param_is_pointer(self):
        """[in] param with pointer raises SyntaxError."""
        with pytest.raises(SyntaxError, match="must not be a pointer"):
            parse("""
                service Foo {
                    [method=1]
                    int Get([in] uint32* val);
                };
            """)

    def test_notification_with_out_param(self):
        """Notification with [out] param raises SyntaxError."""
        with pytest.raises(SyntaxError, match="notification params must be"):
            parse("""
                notifications Foo {
                    [notify=1]
                    void Event([out] uint32* val);
                };
            """)

    def test_missing_semicolon(self):
        """Missing semicolon after method raises SyntaxError."""
        with pytest.raises(SyntaxError):
            parse("""
                service Foo {
                    [method=1]
                    int Get([in] uint32 val)
                };
            """)

    # ── Enum / struct parsing ────────────────────────────────────────

    def test_enum_basic(self):
        """Enum with explicit values parses correctly."""
        idl = parse("""
            enum DeviceType {
                Unknown = 0,
                USB = 1,
                Bluetooth = 2,
            };
            service Foo {
                [method=1]
                int Get([in] uint32 x);
            };
        """)
        assert len(idl.enums) == 1
        e = idl.enums[0]
        assert e.name == "DeviceType"
        assert len(e.values) == 3
        assert e.values[0].name == "Unknown"
        assert e.values[0].value == 0
        assert e.values[2].name == "Bluetooth"
        assert e.values[2].value == 2

    def test_struct_basic(self):
        """Struct with scalar fields parses correctly."""
        idl = parse("""
            struct Point {
                uint32 x;
                uint32 y;
            };
            service Foo {
                [method=1]
                int Get([in] uint32 x);
            };
        """)
        assert len(idl.structs) == 1
        s = idl.structs[0]
        assert s.name == "Point"
        assert len(s.fields) == 2
        assert s.fields[0].type_name == "uint32"
        assert s.fields[0].name == "x"

    def test_struct_with_enum_field(self):
        """Struct can reference a previously defined enum."""
        idl = parse("""
            enum Color { Red = 0, Green = 1, Blue = 2, };
            struct Pixel {
                uint32 x;
                uint32 y;
                Color color;
            };
            service Foo {
                [method=1]
                int Get([in] uint32 x);
            };
        """)
        assert idl.structs[0].fields[2].type_name == "Color"

    def test_struct_with_struct_field(self):
        """Struct can contain another struct (nested POD)."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            struct Rect { Point topLeft; Point bottomRight; };
            service Foo {
                [method=1]
                int Get([in] uint32 x);
            };
        """)
        assert idl.structs[1].fields[0].type_name == "Point"

    def test_enum_used_as_param(self):
        """Enum type can be used as a method parameter."""
        idl = parse("""
            enum Color { Red = 0, Blue = 1, };
            service Foo {
                [method=1]
                int SetColor([in] Color c);
            };
        """)
        assert idl.methods[0].params[0].type_name == "Color"

    def test_struct_used_as_out_param(self):
        """Struct type can be used as an [out] pointer param."""
        idl = parse("""
            struct Info { uint32 id; uint32 status; };
            service Foo {
                [method=1]
                int GetInfo([in] uint32 id, [out] Info* info);
            };
        """)
        p = idl.methods[0].params[1]
        assert p.type_name == "Info"
        assert p.is_pointer is True
        assert p.direction == "out"

    def test_struct_in_notification(self):
        """Struct type can be used as a notification [in] param."""
        idl = parse("""
            struct Info { uint32 id; uint32 status; };
            service Foo {
                [method=1]
                int Get([in] uint32 x);
            };
            notifications Foo {
                [notify=1]
                void InfoChanged([in] Info info);
            };
        """)
        assert idl.notifications[0].params[0].type_name == "Info"

    def test_unknown_struct_field_type(self):
        """Struct field with undefined type raises SyntaxError."""
        with pytest.raises(SyntaxError, match="unknown type"):
            parse("""
                struct Bad { FakeType x; };
                service Foo { [method=1] int Get([in] uint32 x); };
            """)

    def test_duplicate_type_name(self):
        """Defining two enums with the same name raises SyntaxError."""
        with pytest.raises(SyntaxError, match="already defined"):
            parse("""
                enum Foo { A = 0, };
                enum Foo { B = 1, };
                service Bar { [method=1] int Get([in] uint32 x); };
            """)

    def test_type_name_shadows_builtin(self):
        """User type name matching a built-in type raises SyntaxError."""
        with pytest.raises(SyntaxError, match="already defined"):
            parse("""
                enum uint32 { A = 0, };
                service Foo { [method=1] int Get([in] uint32 x); };
            """)

    def test_empty_struct_rejected(self):
        """Struct with no fields raises SyntaxError."""
        with pytest.raises(SyntaxError, match="no fields"):
            parse("""
                struct Empty {};
                service Foo { [method=1] int Get([in] uint32 x); };
            """)

    # ── Fixed-length array parsing ──────────────────────────────────

    def test_struct_array_field(self):
        """Struct field with fixed-length array parses array_size."""
        idl = parse("""
            struct Mac { uint8[6] bytes; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        f = idl.structs[0].fields[0]
        assert f.type_name == "uint8"
        assert f.name == "bytes"
        assert f.array_size == 6

    def test_struct_array_of_enum(self):
        """Struct field can be a fixed-length array of an enum type."""
        idl = parse("""
            enum Color { Red = 0, Green = 1, Blue = 2, };
            struct Palette { Color[4] colors; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        f = idl.structs[0].fields[0]
        assert f.type_name == "Color"
        assert f.array_size == 4

    def test_param_array_in(self):
        """[in] param with fixed-length array."""
        idl = parse("""
            service Foo {
                [method=1]
                int SetKey([in] uint8[16] key);
            };
        """)
        p = idl.methods[0].params[0]
        assert p.type_name == "uint8"
        assert p.array_size == 16
        assert p.direction == "in"

    def test_param_array_out(self):
        """[out] param with fixed-length array pointer."""
        idl = parse("""
            service Foo {
                [method=1]
                int GetKey([in] uint32 slot, [out] uint8[32]* key);
            };
        """)
        p = idl.methods[0].params[1]
        assert p.type_name == "uint8"
        assert p.array_size == 32
        assert p.is_pointer is True
        assert p.direction == "out"

    def test_notification_array_param(self):
        """Notification [in] param with fixed-length array."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void DataReady([in] uint8[64] data);
            };
        """)
        p = idl.notifications[0].params[0]
        assert p.type_name == "uint8"
        assert p.array_size == 64

    def test_array_zero_size_rejected(self):
        """Array size of 0 raises SyntaxError."""
        with pytest.raises(SyntaxError, match="array size must be >= 1"):
            parse("""
                struct Bad { uint8[0] data; };
                service Foo { [method=1] int Get([in] uint32 x); };
            """)

    def test_struct_multiple_array_fields(self):
        """Struct with a mix of array and non-array fields."""
        idl = parse("""
            struct Packet {
                uint32 id;
                uint8[6] mac;
                uint16 flags;
                uint8[16] payload;
            };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        s = idl.structs[0]
        assert len(s.fields) == 4
        assert s.fields[0].array_size is None     # uint32 id
        assert s.fields[1].array_size == 6         # uint8[6] mac
        assert s.fields[2].array_size is None     # uint16 flags
        assert s.fields[3].array_size == 16        # uint8[16] payload

    def test_struct_with_array_of_struct(self):
        """Struct containing a fixed-length array of another struct."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            struct Triangle { Point[3] vertices; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        f = idl.structs[1].fields[0]
        assert f.type_name == "Point"
        assert f.array_size == 3
        assert f.name == "vertices"

    def test_nested_struct_with_arrays(self):
        """Struct containing another struct that itself has array fields."""
        idl = parse("""
            struct Mac { uint8[6] bytes; };
            struct Device {
                uint32 id;
                Mac mac;
            };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        # Mac has array field, Device has Mac field (no array_size on Device.mac)
        assert idl.structs[0].fields[0].array_size == 6  # Mac.bytes
        assert idl.structs[1].fields[1].type_name == "Mac"
        assert idl.structs[1].fields[1].array_size is None

    def test_param_array_zero_size_rejected(self):
        """Array size of 0 on method param raises SyntaxError."""
        with pytest.raises(SyntaxError, match="array size must be >= 1"):
            parse("""
                service Foo {
                    [method=1]
                    int Get([in] uint8[0] data);
                };
            """)

    def test_array_all_pod_types(self):
        """Arrays of all built-in POD types parse correctly."""
        types = [
            "uint8", "uint16", "uint32", "uint64",
            "int8", "int16", "int32", "int64",
            "float32", "float64", "bool",
        ]
        fields = [f"    {t}[4] f{i};" for i, t in enumerate(types)]
        body = "\n".join(fields)
        idl = parse(f"""
            struct AllTypes {{
                {body}
            }};
            service Foo {{ [method=1] int Get([in] uint32 x); }};
        """)
        assert len(idl.structs[0].fields) == len(types)
        for f in idl.structs[0].fields:
            assert f.array_size == 4

    def test_non_array_field_has_none_array_size(self):
        """Scalar fields have array_size == None (not accidentally set)."""
        idl = parse("""
            struct Data { uint32 x; uint32 y; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        for f in idl.structs[0].fields:
            assert f.array_size is None

    def test_non_array_param_has_none_array_size(self):
        """Scalar params have array_size == None."""
        idl = parse("""
            service Foo {
                [method=1]
                int Transfer([in] uint32 src, [out] uint32* dst);
            };
        """)
        for p in idl.methods[0].params:
            assert p.array_size is None

    def test_method_mixed_array_and_scalar_params(self):
        """Method with both array and scalar params parses correctly."""
        idl = parse("""
            service Foo {
                [method=1]
                int WriteBlock([in] uint32 offset, [in] uint8[256] data,
                               [out] uint32* written);
            };
        """)
        m = idl.methods[0]
        assert len(m.params) == 3
        assert m.params[0].type_name == "uint32"
        assert m.params[0].array_size is None
        assert m.params[1].type_name == "uint8"
        assert m.params[1].array_size == 256
        assert m.params[2].type_name == "uint32"
        assert m.params[2].array_size is None

    def test_method_multiple_array_params(self):
        """Method with multiple array params of different sizes."""
        idl = parse("""
            service Foo {
                [method=1]
                int Exchange([in] uint8[16] inBuf, [out] uint8[32]* outBuf);
            };
        """)
        m = idl.methods[0]
        assert m.params[0].array_size == 16
        assert m.params[0].direction == "in"
        assert m.params[1].array_size == 32
        assert m.params[1].direction == "out"
        assert m.params[1].is_pointer is True

    def test_notification_multiple_array_params(self):
        """Notification with multiple params including arrays."""
        idl = parse("""
            service Foo { [method=1] int Get([in] uint32 x); };
            notifications Foo {
                [notify=1]
                void DataReceived([in] uint32 channel, [in] uint8[64] data);
            };
        """)
        n = idl.notifications[0]
        assert len(n.params) == 2
        assert n.params[0].array_size is None
        assert n.params[1].array_size == 64

    def test_array_of_struct_as_param(self):
        """Array of user-defined struct as method param."""
        idl = parse("""
            struct Point { uint32 x; uint32 y; };
            service Foo {
                [method=1]
                int SetPoints([in] Point[4] pts);
            };
        """)
        p = idl.methods[0].params[0]
        assert p.type_name == "Point"
        assert p.array_size == 4

    def test_array_of_enum_as_param(self):
        """Array of enum as method param."""
        idl = parse("""
            enum Color { Red = 0, Green = 1, Blue = 2, };
            service Foo {
                [method=1]
                int SetColors([in] Color[3] colors);
            };
        """)
        p = idl.methods[0].params[0]
        assert p.type_name == "Color"
        assert p.array_size == 3

    def test_array_size_1(self):
        """Array size of 1 is valid (edge case)."""
        idl = parse("""
            struct Wrapper { uint8[1] byte; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        assert idl.structs[0].fields[0].array_size == 1

    def test_large_array_size(self):
        """Large array size parses correctly."""
        idl = parse("""
            struct Buffer { uint8[4096] data; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        assert idl.structs[0].fields[0].array_size == 4096

    def test_struct_array_of_struct_with_array(self):
        """Array of structs where the struct itself has array fields."""
        idl = parse("""
            struct Entry { uint32 id; uint8[6] mac; };
            struct Table { Entry[8] entries; };
            service Foo { [method=1] int Get([in] uint32 x); };
        """)
        # Entry has an array field
        assert idl.structs[0].fields[1].array_size == 6
        # Table has an array of Entry
        f = idl.structs[1].fields[0]
        assert f.type_name == "Entry"
        assert f.array_size == 8
