"""Tests for the aether-lite C99 code emitter."""

import io
import pytest

from ipcgen.types import fnv1a_32
from ipcgen.aether_lite_emitter import emit_aether_lite_h, emit_aether_lite_c

from .conftest import parse


TEMP_SENSOR_IDL = """\
service TemperatureSensor
{
    [method=1]
    int GetTemperature([out] float32 celsius);

    [method=2]
    int SetThreshold([in] float32 high, [in] float32 low);
};

notifications TemperatureSensor
{
    [notify=1]
    void OverTemperature([in] float32 celsius);
};
"""

DEVICE_MONITOR_IDL = """\
service DeviceMonitor
{
    [method=1]
    int GetDeviceCount([out] uint32 count);

    [method=2]
    int GetDeviceStatus([in] uint32 deviceId, [out] uint32 status);
};

notifications DeviceMonitor
{
    [notify=1]
    void DeviceConnected([in] uint32 deviceId);

    [notify=2]
    void DeviceDisconnected([in] uint32 deviceId);
};
"""

STRING_IDL = """\
service StringService
{
    [method=1]
    int GetName([out] string[32] name);

    [method=2]
    int SetName([in] string[32] name);
};
"""

ARRAY_IDL = """\
service ArrayService
{
    [method=1]
    int ReadBlock([out] uint8[16] data);

    [method=2]
    int WriteBlock([in] uint8[16] data);
};
"""

MULTI_PARAM_IDL = """\
service MultiParam
{
    [method=1]
    int Transfer([in] uint32 srcAddr, [in] uint32 dstAddr, [in] uint16 length,
                 [out] uint8 status);
};
"""

NO_NOTIFICATIONS_IDL = """\
service SimpleService
{
    [method=1]
    int Ping([out] uint32 pong);
};
"""

NOTIFY_ONLY_IDL = """\
service EventSource
{
    [method=1]
    int Enable([in] uint8 flags);
};

notifications EventSource
{
    [notify=1]
    void Alert([in] uint8 level, [in] uint32 code);
};
"""


def _gen_h(idl_text):
    idl = parse(idl_text)
    buf = io.StringIO()
    emit_aether_lite_h(idl, buf)
    return buf.getvalue()


def _gen_c(idl_text):
    idl = parse(idl_text)
    buf = io.StringIO()
    emit_aether_lite_c(idl, buf)
    return buf.getvalue()


class TestHeaderStructure:
    """Basic header structure: guard, includes, macros."""

    def test_include_guard(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "#ifndef TEMPERATURE_SENSOR_H" in h
        assert "#define TEMPERATURE_SENSOR_H" in h
        assert "#endif" in h

    def test_includes(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert '#include "aether_lite_dispatch.h"' in h
        assert '#include "aether_lite_types.h"' in h
        assert "#include <string.h>" in h

    def test_no_cpp_comments(self):
        """Generated code must use C89/C99 block comments, not //."""
        h = _gen_h(TEMP_SENSOR_IDL)
        c = _gen_c(TEMP_SENSOR_IDL)
        for line in h.splitlines():
            stripped = line.lstrip()
            assert not stripped.startswith("//"), f"C++ comment in header: {line}"
        for line in c.splitlines():
            stripped = line.lstrip()
            assert not stripped.startswith("//"), f"C++ comment in source: {line}"

    def test_do_not_edit_banner(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "do not edit" in h


class TestServiceId:
    """Service ID macro matches FNV-1a hash."""

    def test_service_id_value(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        expected = f"0x{fnv1a_32('TemperatureSensor'):08x}u"
        assert expected in h

    def test_service_id_macro_name(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "TEMPERATURE_SENSOR_SERVICE_ID" in h


class TestMethodIds:
    """Method ID macros."""

    def test_method_id_macros(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "TEMPERATURE_SENSOR_GET_TEMPERATURE  1" in h
        assert "TEMPERATURE_SENSOR_SET_THRESHOLD  2" in h

    def test_method_count(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "TEMPERATURE_SENSOR_METHOD_COUNT 2" in h


class TestNotificationIds:
    """Notification ID macros."""

    def test_notification_id_macro(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "TEMPERATURE_SENSOR_OVER_TEMPERATURE  1" in h

    def test_no_notification_section_when_none(self):
        h = _gen_h(NO_NOTIFICATIONS_IDL)
        assert "Notification IDs" not in h


class TestHandlerDeclarations:
    """Extern handler declarations in the header."""

    def test_out_param_handler(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "extern int TemperatureSensor_handleGetTemperature(float *celsius);" in h

    def test_in_param_handler(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "extern int TemperatureSensor_handleSetThreshold(float high, float low);" in h

    def test_mixed_in_out_handler(self):
        h = _gen_h(DEVICE_MONITOR_IDL)
        assert "extern int DeviceMonitor_handleGetDeviceStatus(uint32_t deviceId, uint32_t *status);" in h

    def test_no_params_produces_void(self):
        """A method with no params should have (void) in the handler."""
        idl_text = """\
service NoArgs
{
    [method=1]
    int Reset();
};
"""
        # This IDL has no params at all
        # The parser requires at least an empty param list: ()
        h = _gen_h(idl_text)
        assert "extern int NoArgs_handleReset(void);" in h


class TestRegisterFunction:
    """Static inline register function."""

    def test_register_function_exists(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "static inline int TemperatureSensor_register(void)" in h

    def test_register_calls_al_register_service(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "al_register_service(TEMPERATURE_SENSOR_SERVICE_ID, methods, TEMPERATURE_SENSOR_METHOD_COUNT)" in h

    def test_register_has_dispatch_externs(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "extern int TemperatureSensor_dispatchGetTemperature(" in h
        assert "extern int TemperatureSensor_dispatchSetThreshold(" in h

    def test_method_table_entries(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "TEMPERATURE_SENSOR_GET_TEMPERATURE, TemperatureSensor_dispatchGetTemperature" in h
        assert "TEMPERATURE_SENSOR_SET_THRESHOLD, TemperatureSensor_dispatchSetThreshold" in h


class TestNotificationSender:
    """Notification sender static inline functions."""

    def test_sender_signature(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "static inline int TemperatureSensor_notifyOverTemperature(float celsius)" in h

    def test_sender_calls_al_send_notify(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "al_send_notify(TEMPERATURE_SENSOR_SERVICE_ID, TEMPERATURE_SENSOR_OVER_TEMPERATURE," in h

    def test_sender_uses_memcpy(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "memcpy(buf" in h

    def test_multi_param_notification(self):
        h = _gen_h(NOTIFY_ONLY_IDL)
        assert "static inline int EventSource_notifyAlert(uint8_t level, uint32_t code)" in h
        assert "al_send_notify(" in h


class TestDispatchWrappers:
    """Dispatch wrapper functions in the .c file."""

    def test_dispatch_function_signature(self):
        c = _gen_c(TEMP_SENSOR_IDL)
        assert "int TemperatureSensor_dispatchGetTemperature(" in c
        assert "int TemperatureSensor_dispatchSetThreshold(" in c

    def test_out_only_dispatch(self):
        """GetTemperature: no [in], one [out] float."""
        c = _gen_c(TEMP_SENSOR_IDL)
        assert "(void)req; (void)req_len;" in c
        assert "float celsius = 0;" in c
        assert "TemperatureSensor_handleGetTemperature(&celsius)" in c
        assert "memcpy(resp + 0, &celsius, 4);" in c
        assert "*resp_len = 4;" in c

    def test_in_only_dispatch(self):
        """SetThreshold: two [in] floats, no [out]."""
        c = _gen_c(TEMP_SENSOR_IDL)
        assert "if (req_len < 8) return AL_ERR_INVALID_ARGUMENT;" in c
        assert "float high = 0;" in c
        assert "memcpy(&high, req + 0, 4);" in c
        assert "float low = 0;" in c
        assert "memcpy(&low, req + 4, 4);" in c
        assert "TemperatureSensor_handleSetThreshold(high, low)" in c
        assert "(void)resp; (void)resp_cap;" in c

    def test_mixed_in_out_dispatch(self):
        """GetDeviceStatus: one [in] uint32, one [out] uint32."""
        c = _gen_c(DEVICE_MONITOR_IDL)
        assert "if (req_len < 4) return AL_ERR_INVALID_ARGUMENT;" in c
        assert "if (resp_cap < 4) return AL_ERR_OVERFLOW;" in c
        assert "memcpy(&deviceId, req + 0, 4);" in c
        assert "DeviceMonitor_handleGetDeviceStatus(deviceId, &status)" in c
        assert "memcpy(resp + 0, &status, 4);" in c

    def test_resp_cap_check_for_out(self):
        c = _gen_c(TEMP_SENSOR_IDL)
        assert "if (resp_cap < 4) return AL_ERR_OVERFLOW;" in c

    def test_uses_memcpy_not_cast(self):
        """All marshaling must use memcpy, not pointer casts."""
        c = _gen_c(TEMP_SENSOR_IDL)
        # Should not contain any C-style casts to float* etc.
        assert "(float *)" not in c
        assert "(uint32_t *)" not in c

    def test_includes_header(self):
        c = _gen_c(TEMP_SENSOR_IDL)
        assert '#include "TemperatureSensor.h"' in c
        assert "#include <string.h>" in c


class TestStringHandling:
    """String parameter marshaling."""

    def test_out_string_handler_signature(self):
        h = _gen_h(STRING_IDL)
        assert "extern int StringService_handleGetName(char *name);" in h

    def test_in_string_handler_signature(self):
        h = _gen_h(STRING_IDL)
        assert "extern int StringService_handleSetName(const char *name);" in h

    def test_in_string_unmarshal(self):
        c = _gen_c(STRING_IDL)
        assert "char name[33];" in c
        assert 'name[32] = \'\\0\';' in c

    def test_out_string_marshal(self):
        c = _gen_c(STRING_IDL)
        # The out string should be memset to 0 and memcpy'd to resp
        assert "memset(name, 0, 33);" in c
        assert "memcpy(resp + 0, name, 33);" in c


class TestArrayHandling:
    """Array parameter marshaling."""

    def test_out_array_handler_signature(self):
        h = _gen_h(ARRAY_IDL)
        assert "extern int ArrayService_handleReadBlock(uint8_t *data);" in h

    def test_in_array_handler_signature(self):
        h = _gen_h(ARRAY_IDL)
        assert "extern int ArrayService_handleWriteBlock(const uint8_t *data);" in h

    def test_in_array_unmarshal(self):
        c = _gen_c(ARRAY_IDL)
        assert "uint8_t data[16];" in c
        assert "memcpy(data, req + 0, 16);" in c

    def test_out_array_marshal(self):
        c = _gen_c(ARRAY_IDL)
        assert "memcpy(resp + 0, data, 16);" in c


class TestMultipleInParams:
    """Methods with multiple [in] params."""

    def test_offset_calculation(self):
        c = _gen_c(MULTI_PARAM_IDL)
        # srcAddr at offset 0, dstAddr at offset 4, length at offset 8
        assert "memcpy(&srcAddr, req + 0, 4);" in c
        assert "memcpy(&dstAddr, req + 4, 4);" in c
        assert "memcpy(&length, req + 8, 2);" in c

    def test_total_request_size_check(self):
        c = _gen_c(MULTI_PARAM_IDL)
        # 4 + 4 + 2 = 10
        assert "if (req_len < 10) return AL_ERR_INVALID_ARGUMENT;" in c

    def test_handler_call(self):
        c = _gen_c(MULTI_PARAM_IDL)
        assert "MultiParam_handleTransfer(srcAddr, dstAddr, length, &status)" in c


class TestCamelToSnake:
    """CamelCase to UPPER_SNAKE_CASE conversion."""

    def test_device_monitor(self):
        h = _gen_h(DEVICE_MONITOR_IDL)
        assert "DEVICE_MONITOR_SERVICE_ID" in h
        assert "DEVICE_MONITOR_METHOD_COUNT" in h
        assert "DEVICE_MONITOR_GET_DEVICE_COUNT" in h
