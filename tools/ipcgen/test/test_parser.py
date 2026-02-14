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
