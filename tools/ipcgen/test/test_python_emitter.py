"""Tests for the Python backend emitter."""

import os
import subprocess
import sys

from ipcgen.python_emitter import emit_python_client
from ipcgen.types import fnv1a_32

from .conftest import parse, DEVICE_MONITOR_IDL


TYPED_IDL = """\
enum DeviceStatus {
    ONLINE = 0,
    OFFLINE = 1,
};

struct DeviceInfo {
    uint32 id;
    string[64] name;
};

service DeviceMonitor {
    [method=1]
    int GetDeviceCount([out] uint32 count);

    [method=2]
    int GetDeviceInfo([in] uint32 index, [out] DeviceInfo info);
};

notifications DeviceMonitor {
    [notify=1]
    void DeviceConnected([in] uint32 deviceId, [in] DeviceStatus status);
};
"""

ARRAY_IDL = """\
service KeyStore {
    [method=1]
    int GetKey([in] uint32 slot, [out] uint8[32] key);

    [method=2]
    int SetData([in] uint8[16] data);
};

notifications KeyStore {
    [notify=1]
    void KeyChanged([in] uint32 slot, [in] uint8[32] newKey);
};
"""

STRING_IDL = """\
struct UserInfo {
    uint32 id;
    string[64] name;
    string[128] email;
};

service UserService {
    [method=1]
    int SetName([in] uint32 userId, [in] string[64] name);

    [method=2]
    int GetName([in] uint32 userId, [out] string[64] name);

    [method=3]
    int GetUser([in] uint32 userId, [out] UserInfo info);
};

notifications UserService {
    [notify=1]
    void NameChanged([in] uint32 userId, [in] string[64] newName);
};
"""

ALL_TYPES_IDL = """\
service AllTypes {
    [method=1]
    int TestInts([in] uint8 a, [in] uint16 b, [in] uint32 c, [in] uint64 d,
                 [in] int8 e, [in] int16 f, [in] int32 g, [in] int64 h);

    [method=2]
    int TestFloats([in] float32 x, [in] float64 y, [out] float32 rx, [out] float64 ry);

    [method=3]
    int TestBool([in] bool flag, [out] bool result);
};
"""


class TestPythonEnumGeneration:
    def test_enum_class_generated(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert "class DeviceStatus:" in code
        assert "    ONLINE = 0" in code
        assert "    OFFLINE = 1" in code

    def test_no_enums_no_section(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "# -- Enums --" not in code


class TestPythonStructGeneration:
    def test_struct_class_generated(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert "class DeviceInfo:" in code
        assert "__slots__ = ('id', 'name',)" in code

    def test_struct_wire_size(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        # uint32 (4) + string[64] with NUL (65) = 69
        assert "WIRE_SIZE = 69" in code

    def test_struct_pack_method(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert "def pack(self) -> bytes:" in code
        # Wire format is always LE — these format strings are protocol-correct.
        assert 'struct.pack("<I", self.id)' in code

    def test_struct_unpack_method(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert "def unpack(cls, data: bytes, offset: int = 0)" in code
        # Wire format is always LE — these format strings are protocol-correct.
        assert 'struct.unpack_from("<I", data, offset)' in code

    def test_string_field_pack(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert 'name_bytes = self.name.encode("utf-8")[:64]' in code
        assert "65 - len(name_bytes)" in code

    def test_string_field_unpack(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert '.split(b"\\x00", 1)[0].decode("utf-8")' in code

    def test_no_structs_no_section(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "# -- Structs --" not in code


class TestPythonMethodGeneration:
    def test_method_generated(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "def get_device_count(self" in code
        assert "def get_device_status(self" in code

    def test_method_constants(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "METHOD_GET_DEVICE_COUNT = 1" in code
        assert "METHOD_GET_DEVICE_STATUS = 2" in code

    def test_method_with_in_params(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert "def get_device_info(self, index: int" in code
        assert 'struct.pack("<I", index)' in code

    def test_method_with_out_params(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "Returns (status, count)" in code
        assert 'struct.unpack_from("<I", resp, offset)' in code

    def test_method_with_struct_out(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        assert "DeviceInfo.unpack(resp, offset)" in code
        assert "return (rc, info)" in code

    def test_method_returns_default_on_error(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "return (rc, 0)" in code

    def test_service_id(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        expected = f"SERVICE_ID = 0x{fnv1a_32('DeviceMonitor'):08x}"
        assert expected in code

    def test_client_class_name(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "class DeviceMonitorClient:" in code


class TestPythonNotificationGeneration:
    def test_notification_registration(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "def on_device_connected(self, handler:" in code
        assert "def on_device_disconnected(self, handler:" in code

    def test_notification_constants(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "NOTIFY_DEVICE_CONNECTED = 1" in code
        assert "NOTIFY_DEVICE_DISCONNECTED = 2" in code

    def test_notification_dispatch(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "_dispatch_notification" in code
        assert "self._on_device_connected(" in code

    def test_notification_handler_wired_in_init(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "self._client.set_notification_handler(self._dispatch_notification)" in code

    def test_dispatch_checks_service_id(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "if service_id != SERVICE_ID:" in code

    def test_notification_with_enum_param(self):
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        # DeviceConnected has [in] uint32 deviceId, [in] DeviceStatus status
        assert "self._on_device_connected(deviceId, status)" in code


class TestPythonStringHandling:
    def test_string_in_param(self):
        idl = parse(STRING_IDL)
        code = emit_python_client(idl)
        assert "name: str" in code
        assert 'name_bytes = name.encode("utf-8")[:64]' in code

    def test_string_out_param(self):
        idl = parse(STRING_IDL)
        code = emit_python_client(idl)
        assert '.split(b"\\x00", 1)[0].decode("utf-8")' in code

    def test_string_struct_field(self):
        idl = parse(STRING_IDL)
        code = emit_python_client(idl)
        # UserInfo has string[64] name and string[128] email
        # wire size: 4 + 65 + 129 = 198
        assert "WIRE_SIZE = 198" in code


class TestPythonArrayHandling:
    def test_array_out_param(self):
        idl = parse(ARRAY_IDL)
        code = emit_python_client(idl)
        assert "def get_key(self, slot: int" in code

    def test_array_in_param(self):
        idl = parse(ARRAY_IDL)
        code = emit_python_client(idl)
        assert "def set_data(self, data: list[int]" in code

    def test_array_notification_param(self):
        idl = parse(ARRAY_IDL)
        code = emit_python_client(idl)
        assert "NOTIFY_KEY_CHANGED = 1" in code


class TestPythonAllTypes:
    def test_integer_types(self):
        idl = parse(ALL_TYPES_IDL)
        code = emit_python_client(idl)
        # Check various struct format codes are used
        assert '"<B"' in code   # uint8
        assert '"<H"' in code   # uint16
        assert '"<I"' in code   # uint32
        assert '"<Q"' in code   # uint64
        assert '"<b"' in code   # int8
        assert '"<h"' in code   # int16
        assert '"<i"' in code   # int32
        assert '"<q"' in code   # int64

    def test_float_types(self):
        idl = parse(ALL_TYPES_IDL)
        code = emit_python_client(idl)
        assert '"<f"' in code   # float32
        assert '"<d"' in code   # float64

    def test_bool_type(self):
        idl = parse(ALL_TYPES_IDL)
        code = emit_python_client(idl)
        # bool uses "B" (uint8) for wire compat with C++, wrapped with bool() on unpack
        assert 'struct.pack("<B", flag)' in code
        assert 'bool(struct.unpack_from("<B"' in code


class TestPythonEndToEnd:
    def test_full_typed_idl_generates_valid_python(self):
        """The generated code is syntactically valid Python."""
        idl = parse(TYPED_IDL)
        code = emit_python_client(idl)
        # Should compile without errors (syntax check)
        compile(code, "<generated>", "exec")

    def test_full_basic_idl_generates_valid_python(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        compile(code, "<generated>", "exec")

    def test_full_string_idl_generates_valid_python(self):
        idl = parse(STRING_IDL)
        code = emit_python_client(idl)
        compile(code, "<generated>", "exec")

    def test_full_array_idl_generates_valid_python(self):
        idl = parse(ARRAY_IDL)
        code = emit_python_client(idl)
        compile(code, "<generated>", "exec")

    def test_full_all_types_generates_valid_python(self):
        idl = parse(ALL_TYPES_IDL)
        code = emit_python_client(idl)
        compile(code, "<generated>", "exec")

    def test_header_comment(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert code.startswith('"""Generated by ipcgen')

    def test_imports(self):
        idl = parse(DEVICE_MONITOR_IDL)
        code = emit_python_client(idl)
        assert "import struct" in code
        assert "from typing import Optional, Callable" in code
        assert "from aether_ipc import AetherClient, IPC_SUCCESS" in code

    def test_cli_invocation(self, tmp_path):
        """Running ipcgen --backend python produces the expected .py file."""
        idl_file = tmp_path / "DeviceMonitor.idl"
        idl_file.write_text(TYPED_IDL)
        outdir = tmp_path / "out"

        tools_dir = os.path.join(os.path.dirname(__file__), "..", "..")
        result = subprocess.run(
            [sys.executable, "-m", "ipcgen", str(idl_file),
             "--outdir", str(outdir), "--backend", "python"],
            capture_output=True,
            text=True,
            cwd=tools_dir,
        )

        assert result.returncode == 0, f"ipcgen failed:\n{result.stderr}"
        py_file = outdir / "client" / "DeviceMonitor.py"
        assert py_file.exists()

        content = py_file.read_text()
        assert "class DeviceMonitorClient:" in content
        assert "class DeviceStatus:" in content
        assert "class DeviceInfo:" in content

    def test_cli_no_types_header_for_python(self, tmp_path):
        """Python backend should not generate a C++ Types.h file."""
        idl_file = tmp_path / "DeviceMonitor.idl"
        idl_file.write_text(TYPED_IDL)
        outdir = tmp_path / "out"

        tools_dir = os.path.join(os.path.dirname(__file__), "..", "..")
        result = subprocess.run(
            [sys.executable, "-m", "ipcgen", str(idl_file),
             "--outdir", str(outdir), "--backend", "python"],
            capture_output=True,
            text=True,
            cwd=tools_dir,
        )

        assert result.returncode == 0
        assert not (outdir / "DeviceMonitorTypes.h").exists()
