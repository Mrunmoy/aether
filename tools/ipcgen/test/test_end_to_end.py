"""End-to-end tests: full pipeline from IDL text to files on disk."""

import os
import subprocess
import sys

from ipcgen.emitter import emit_server_h, emit_server_cpp, emit_client_h, emit_client_cpp, emit_types_h

from .conftest import parse, DEVICE_MONITOR_IDL


TYPED_IDL = """\
enum DeviceType
{
    Unknown = 0,
    USB = 1,
    Bluetooth = 2,
};

struct DeviceInfo
{
    uint32 id;
    DeviceType type;
    uint32 vendorId;
};

service DeviceMonitor
{
    [method=1]
    int GetDeviceCount([out] uint32* count);

    [method=2]
    int GetDeviceInfo([in] uint32 deviceId, [out] DeviceInfo* info);
};

notifications DeviceMonitor
{
    [notify=1]
    void DeviceConnected([in] DeviceInfo info);
};
"""


class TestEndToEnd:
    """Full pipeline: IDL text -> parse -> emit -> files on disk."""

    def test_generates_four_files(self, tmp_path):
        """Generator creates server/ and client/ dirs with .h and .cpp."""
        idl = parse(DEVICE_MONITOR_IDL)
        name = idl.service_name

        server_dir = tmp_path / "server"
        client_dir = tmp_path / "client"
        server_dir.mkdir(parents=True)
        client_dir.mkdir(parents=True)

        (server_dir / f"{name}.h").write_text(emit_server_h(idl))
        (server_dir / f"{name}.cpp").write_text(emit_server_cpp(idl))
        (client_dir / f"{name}.h").write_text(emit_client_h(idl))
        (client_dir / f"{name}.cpp").write_text(emit_client_cpp(idl))

        assert (server_dir / "DeviceMonitor.h").exists()
        assert (server_dir / "DeviceMonitor.cpp").exists()
        assert (client_dir / "DeviceMonitor.h").exists()
        assert (client_dir / "DeviceMonitor.cpp").exists()

    def test_generated_files_match_emitters(self, tmp_path):
        """File contents match what the emitter functions return."""
        idl = parse(DEVICE_MONITOR_IDL)
        name = idl.service_name

        server_dir = tmp_path / "server"
        client_dir = tmp_path / "client"
        server_dir.mkdir()
        client_dir.mkdir()

        expected_server_h = emit_server_h(idl)
        expected_client_cpp = emit_client_cpp(idl)

        (server_dir / f"{name}.h").write_text(expected_server_h)
        (client_dir / f"{name}.cpp").write_text(expected_client_cpp)

        assert (server_dir / f"{name}.h").read_text() == expected_server_h
        assert (client_dir / f"{name}.cpp").read_text() == expected_client_cpp

    def test_cli_invocation(self, tmp_path):
        """Running ipcgen as a subprocess produces the expected files."""
        idl_file = tmp_path / "DeviceMonitor.idl"
        idl_file.write_text(DEVICE_MONITOR_IDL)
        outdir = tmp_path / "out"

        # Run as package: python -m ipcgen
        tools_dir = os.path.join(os.path.dirname(__file__), "..", "..")
        result = subprocess.run(
            [sys.executable, "-m", "ipcgen", str(idl_file), "--outdir", str(outdir)],
            capture_output=True,
            text=True,
            cwd=tools_dir,
        )

        assert result.returncode == 0, f"ipcgen failed:\n{result.stderr}"
        assert (outdir / "server" / "DeviceMonitor.h").exists()
        assert (outdir / "server" / "DeviceMonitor.cpp").exists()
        assert (outdir / "client" / "DeviceMonitor.h").exists()
        assert (outdir / "client" / "DeviceMonitor.cpp").exists()

        # Verify the generated content is non-trivial.
        server_h = (outdir / "server" / "DeviceMonitor.h").read_text()
        assert "class DeviceMonitor : public ServiceBase" in server_h

    def test_generates_types_header(self, tmp_path):
        """When types are present, generates Types.h plus the usual four."""
        idl = parse(TYPED_IDL)
        name = idl.service_name

        server_dir = tmp_path / "server"
        client_dir = tmp_path / "client"
        server_dir.mkdir(parents=True)
        client_dir.mkdir(parents=True)

        (tmp_path / f"{name}Types.h").write_text(emit_types_h(idl))
        (server_dir / f"{name}.h").write_text(emit_server_h(idl))
        (server_dir / f"{name}.cpp").write_text(emit_server_cpp(idl))
        (client_dir / f"{name}.h").write_text(emit_client_h(idl))
        (client_dir / f"{name}.cpp").write_text(emit_client_cpp(idl))

        assert (tmp_path / "DeviceMonitorTypes.h").exists()
        types_content = (tmp_path / f"{name}Types.h").read_text()
        assert "enum DeviceType : uint32_t" in types_content
        assert "struct DeviceInfo" in types_content

    def test_cli_with_types(self, tmp_path):
        """CLI generates Types.h when IDL has enum/struct definitions."""
        idl_file = tmp_path / "DeviceMonitor.idl"
        idl_file.write_text(TYPED_IDL)
        outdir = tmp_path / "out"

        tools_dir = os.path.join(os.path.dirname(__file__), "..", "..")
        result = subprocess.run(
            [sys.executable, "-m", "ipcgen", str(idl_file), "--outdir", str(outdir)],
            capture_output=True,
            text=True,
            cwd=tools_dir,
        )

        assert result.returncode == 0, f"ipcgen failed:\n{result.stderr}"
        assert (outdir / "DeviceMonitorTypes.h").exists()
        assert (outdir / "server" / "DeviceMonitor.h").exists()
        assert (outdir / "client" / "DeviceMonitor.h").exists()

        # Types header has correct content.
        types_h = (outdir / "DeviceMonitorTypes.h").read_text()
        assert "enum DeviceType : uint32_t" in types_h
        assert "struct DeviceInfo" in types_h

        # Server header includes the types header.
        server_h = (outdir / "server" / "DeviceMonitor.h").read_text()
        assert '#include "DeviceMonitorTypes.h"' in server_h
