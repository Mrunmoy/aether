"""Tests for the client-side C++ code emitter."""

import pytest

from ipcgen.emitter import emit_client_h, emit_client_cpp

from .conftest import parse, DEVICE_MONITOR_IDL


class TestClientEmitter:
    """Generated client-side header and implementation."""

    @pytest.fixture
    def idl(self):
        """Parse the DeviceMonitor IDL for use in emitter tests."""
        return parse(DEVICE_MONITOR_IDL)

    def test_header_structure(self, idl):
        """Client header has pragma, includes, namespace, and class."""
        h = emit_client_h(idl)
        assert "#pragma once" in h
        assert '#include "ClientBase.h"' in h
        assert "namespace aether::ipc" in h
        assert "class DeviceMonitor : public ClientBase" in h
        assert "using ClientBase::ClientBase;" in h

    def test_rpc_method_signatures(self, idl):
        """Typed RPC methods have correct signatures with timeout default."""
        h = emit_client_h(idl)
        assert "int GetDeviceCount(uint32_t *count, uint32_t timeoutMs = 2000);" in h
        assert "int GetDeviceStatus(uint32_t deviceId, uint32_t *status, uint32_t timeoutMs = 2000);" in h

    def test_method_enum(self, idl):
        """Header contains MethodId enum with correct entries."""
        h = emit_client_h(idl)
        assert "enum MethodId : uint32_t" in h
        assert "kGetDeviceCount = 1," in h
        assert "kGetDeviceStatus = 2," in h

    def test_notify_enum(self, idl):
        """Header contains NotifyId enum with correct entries."""
        h = emit_client_h(idl)
        assert "enum NotifyId : uint32_t" in h
        assert "kDeviceConnected = 1," in h
        assert "kDeviceDisconnected = 2," in h

    def test_notification_callbacks(self, idl):
        """Virtual notification callbacks with empty default body."""
        h = emit_client_h(idl)
        assert "virtual void onDeviceConnected(uint32_t deviceId) {}" in h
        assert "virtual void onDeviceDisconnected(uint32_t deviceId) {}" in h

    def test_onNotification_override(self, idl):
        """onNotification override is declared in the header."""
        h = emit_client_h(idl)
        assert "void onNotification(" in h
        assert "override;" in h

    def test_cpp_rpc_methods(self, idl):
        """Implementation has correct call() invocations and marshaling."""
        cpp = emit_client_cpp(idl)

        # GetDeviceCount: empty request (no [in] params), unmarshals count.
        assert "std::vector<uint8_t> request;" in cpp
        assert "call(kServiceId, DeviceMonitor::kGetDeviceCount, request," in cpp
        assert "std::memcpy(count, response.data()," in cpp

        # GetDeviceStatus: marshals deviceId, unmarshals status.
        assert "std::memcpy(request.data(), &deviceId," in cpp
        assert "call(kServiceId, DeviceMonitor::kGetDeviceStatus, request," in cpp

        # Uses _rc to avoid name collision.
        assert "int _rc = call(" in cpp

    def test_cpp_notification_switch(self, idl):
        """Notification dispatch has serviceId guard and correct cases."""
        cpp = emit_client_cpp(idl)

        assert "if (serviceId != kServiceId) return;" in cpp
        assert "case DeviceMonitor::kDeviceConnected:" in cpp
        assert "case DeviceMonitor::kDeviceDisconnected:" in cpp
        assert "onDeviceConnected(deviceId);" in cpp
        assert "onDeviceDisconnected(deviceId);" in cpp
