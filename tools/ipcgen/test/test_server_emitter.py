"""Tests for the server-side C++ code emitter."""

import pytest

from ipcgen.types import fnv1a_32
from ipcgen.emitter import emit_server_h, emit_server_cpp

from .conftest import parse, DEVICE_MONITOR_IDL


class TestServerEmitter:
    """Generated server-side header and implementation."""

    @pytest.fixture
    def idl(self):
        """Parse the DeviceMonitor IDL for use in emitter tests."""
        return parse(DEVICE_MONITOR_IDL)

    def test_header_structure(self, idl):
        """Server header has pragma, includes, namespace, and class."""
        h = emit_server_h(idl)
        assert "#pragma once" in h
        assert '#include "ServiceBase.h"' in h
        assert "namespace aether::ipc" in h
        assert "class DeviceMonitor : public ServiceBase" in h
        assert "using ServiceBase::ServiceBase;" in h

    def test_service_id(self, idl):
        """kServiceId matches the FNV-1a hash of the service name."""
        h = emit_server_h(idl)
        expected = f"0x{fnv1a_32('DeviceMonitor'):08x}u"
        assert expected in h

    def test_method_enum(self, idl):
        """Header contains MethodId enum with correct entries."""
        h = emit_server_h(idl)
        assert "enum MethodId : uint32_t" in h
        assert "kGetDeviceCount = 1," in h
        assert "kGetDeviceStatus = 2," in h

    def test_notify_enum(self, idl):
        """Header contains NotifyId enum with correct entries."""
        h = emit_server_h(idl)
        assert "enum NotifyId : uint32_t" in h
        assert "kDeviceConnected = 1," in h
        assert "kDeviceDisconnected = 2," in h

    def test_handler_signatures(self, idl):
        """Pure virtual handler methods have correct signatures."""
        h = emit_server_h(idl)
        assert "virtual int handleGetDeviceCount(uint32_t *count) = 0;" in h
        assert "virtual int handleGetDeviceStatus(uint32_t deviceId, uint32_t *status) = 0;" in h

    def test_notify_signatures(self, idl):
        """Notification sender methods have correct signatures."""
        h = emit_server_h(idl)
        assert "int notifyDeviceConnected(uint32_t deviceId);" in h
        assert "int notifyDeviceDisconnected(uint32_t deviceId);" in h

    def test_onRequest_override(self, idl):
        """onRequest override is declared in the header."""
        h = emit_server_h(idl)
        assert "int onRequest(" in h
        assert "override;" in h

    def test_cpp_dispatch_switch(self, idl):
        """Implementation has correct switch cases and marshal/unmarshal."""
        cpp = emit_server_cpp(idl)

        # Switch on messageId with enum case labels.
        assert "switch (messageId)" in cpp
        assert "case DeviceMonitor::kGetDeviceCount:" in cpp
        assert "case DeviceMonitor::kGetDeviceStatus:" in cpp
        assert "return IPC_ERR_INVALID_METHOD;" in cpp

        # GetDeviceCount: no [in] unmarshal, calls handler, marshals [out].
        assert "handleGetDeviceCount(&count)" in cpp
        assert "response->resize(sizeof(count))" in cpp

        # GetDeviceStatus: unmarshals deviceId, calls handler.
        assert "std::memcpy(&deviceId, request.data()" in cpp
        assert "handleGetDeviceStatus(deviceId, &status)" in cpp

        # Uses _rc to avoid name collision with user params.
        assert "int _rc = " in cpp

        # Notification senders use enum names in sendNotify.
        assert "sendNotify(kServiceId, DeviceMonitor::kDeviceConnected," in cpp
        assert "sendNotify(kServiceId, DeviceMonitor::kDeviceDisconnected," in cpp
