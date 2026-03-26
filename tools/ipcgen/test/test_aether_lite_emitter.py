"""Tests for the aether-lite C99 code emitter."""

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
    return emit_aether_lite_h(parse(idl_text))


def _gen_c(idl_text):
    return emit_aether_lite_c(parse(idl_text))


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
        assert "al_register_service(TEMPERATURE_SENSOR_SERVICE_ID, methods, (uint8_t)TEMPERATURE_SENSOR_METHOD_COUNT)" in h

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
        import re
        c = _gen_c(TEMP_SENSOR_IDL)
        # Should not contain any C-style casts to float* etc. (with or without spaces)
        assert "(float *)" not in c
        assert "(uint32_t *)" not in c
        assert not re.search(r"\([a-z_]+\d*_t\s*\*\)", c), \
            "Generated code contains pointer casts"

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


STRUCT_IDL = """\
struct DeviceInfo
{
    uint32 id;
    string[64] name;
};

service DeviceQuery
{
    [method=1]
    int GetInfo([in] uint32 slot, [out] DeviceInfo info);
};

notifications DeviceQuery
{
    [notify=1]
    void InfoChanged([in] DeviceInfo info);
};
"""

STRUCT_MULTI_FIELD_IDL = """\
struct SensorReading
{
    uint8 sensorId;
    uint32 timestamp;
    float32 value;
};

service SensorHub
{
    [method=1]
    int GetReading([in] uint8 channel, [out] SensorReading reading);

    [method=2]
    int SetReading([in] SensorReading reading);
};
"""


class TestStructMarshaling:
    """Struct params use field-by-field memcpy, not sizeof."""

    def test_struct_out_marshal_field_by_field(self):
        """[out] DeviceInfo must marshal id then name individually."""
        c = _gen_c(STRUCT_IDL)
        # Must have field-by-field memcpy, not a single sizeof(DeviceInfo)
        assert "sizeof(DeviceInfo)" not in c
        assert "memcpy(resp + 0, &info.id, 4);" in c
        assert "memcpy(resp + 4, info.name, 65);" in c

    def test_struct_in_unmarshal_field_by_field(self):
        """[in] SensorReading must unmarshal fields individually."""
        c = _gen_c(STRUCT_MULTI_FIELD_IDL)
        assert "sizeof(SensorReading)" not in c
        # SetReading: [in] SensorReading reading at offset 0
        assert "memcpy(&reading.sensorId, req + 0, 1);" in c
        assert "memcpy(&reading.timestamp, req + 1, 4);" in c
        assert "memcpy(&reading.value, req + 5, 4);" in c

    def test_struct_wire_size_is_sum_of_fields(self):
        """Wire size for DeviceInfo = 4 (uint32) + 65 (string[64]) = 69."""
        c = _gen_c(STRUCT_IDL)
        # resp_cap check should use 69, not sizeof(DeviceInfo)
        assert "if (resp_cap < 69) return AL_ERR_OVERFLOW;" in c

    def test_struct_wire_size_multi_field(self):
        """Wire size for SensorReading = 1 + 4 + 4 = 9."""
        c = _gen_c(STRUCT_MULTI_FIELD_IDL)
        # GetReading: [out] SensorReading -> resp_cap check
        assert "if (resp_cap < 9) return AL_ERR_OVERFLOW;" in c
        # SetReading: [in] SensorReading -> req_len check
        assert "if (req_len < 9) return AL_ERR_INVALID_ARGUMENT;" in c

    def test_struct_out_resp_len(self):
        """*resp_len should be set to the packed wire size."""
        c = _gen_c(STRUCT_IDL)
        assert "*resp_len = 69;" in c

    def test_struct_zero_init(self):
        """[out] struct params should be zero-initialized with memset."""
        c = _gen_c(STRUCT_IDL)
        assert "DeviceInfo info;" in c
        assert "memset(&info, 0, sizeof(info));" in c

    def test_struct_in_zero_init(self):
        """[in] struct params should be zero-initialized with memset."""
        c = _gen_c(STRUCT_MULTI_FIELD_IDL)
        assert "SensorReading reading;" in c
        assert "memset(&reading, 0, sizeof(reading));" in c

    def test_struct_notification_field_by_field(self):
        """Notification sender for struct param uses field-by-field marshal."""
        h = _gen_h(STRUCT_IDL)
        assert "sizeof(DeviceInfo)" not in h
        assert "memcpy(buf + 0, &info.id, 4);" in h
        assert "memcpy(buf + 4, info.name, 65);" in h

    def test_struct_notification_buf_size(self):
        """Notification buffer size should be the packed wire size."""
        h = _gen_h(STRUCT_IDL)
        assert "uint8_t buf[69];" in h

    def test_struct_no_cpp_comments(self):
        """Struct-containing generated code must not use // comments."""
        h = _gen_h(STRUCT_IDL)
        c = _gen_c(STRUCT_IDL)
        for line in h.splitlines():
            assert not line.lstrip().startswith("//"), f"C++ comment: {line}"
        for line in c.splitlines():
            assert not line.lstrip().startswith("//"), f"C++ comment: {line}"

    def test_struct_out_marshal_with_preceding_scalar(self):
        """GetReading has [in] uint8 channel before [out] SensorReading."""
        c = _gen_c(STRUCT_MULTI_FIELD_IDL)
        # The [out] SensorReading marshal starts at offset 0 in resp
        assert "memcpy(resp + 0, &reading.sensorId, 1);" in c
        assert "memcpy(resp + 1, &reading.timestamp, 4);" in c
        assert "memcpy(resp + 5, &reading.value, 4);" in c


class TestZeroMethodService:
    """Service with no methods should not emit a zero-length array."""

    ZERO_METHOD_IDL = """\
service EmptyService
{
};

notifications EmptyService
{
    [notify=1]
    void Heartbeat();
};
"""

    def test_zero_method_returns_error(self):
        h = _gen_h(self.ZERO_METHOD_IDL)
        assert "AL_ERR_INVALID_ARGUMENT" in h
        assert "methods[" not in h

    def test_zero_method_count_macro(self):
        h = _gen_h(self.ZERO_METHOD_IDL)
        assert "EMPTY_SERVICE_METHOD_COUNT 0" in h

    def test_zero_method_register_compiles(self):
        """Register function should not contain al_register_service call."""
        h = _gen_h(self.ZERO_METHOD_IDL)
        assert "al_register_service(" not in h


class TestNotificationGuard:
    """Notification senders wrapped in AL_ENABLE_NOTIFICATIONS guard."""

    def test_notification_guard_present(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "#if AL_ENABLE_NOTIFICATIONS" in h
        assert "#endif /* AL_ENABLE_NOTIFICATIONS */" in h

    def test_notification_sender_inside_guard(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        guard_start = h.index("#if AL_ENABLE_NOTIFICATIONS")
        guard_end = h.index("#endif /* AL_ENABLE_NOTIFICATIONS */")
        sender = h.index("TemperatureSensor_notifyOverTemperature")
        assert guard_start < sender < guard_end

    def test_no_guard_when_no_notifications(self):
        h = _gen_h(NO_NOTIFICATIONS_IDL)
        assert "AL_ENABLE_NOTIFICATIONS" not in h


class TestMethodCountGuard:
    """AL_MAX_METHODS compile-time guard and uint8_t cast."""

    def test_max_methods_guard(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "#if TEMPERATURE_SENSOR_METHOD_COUNT > AL_MAX_METHODS" in h
        assert '#error "TEMPERATURE_SENSOR_METHOD_COUNT exceeds AL_MAX_METHODS"' in h

    def test_uint8_cast(self):
        h = _gen_h(TEMP_SENSOR_IDL)
        assert "(uint8_t)TEMPERATURE_SENSOR_METHOD_COUNT" in h


class TestInlineTypeDefinitions:
    """C99 type definitions emitted inline in the header."""

    ENUM_STRUCT_IDL = """\
enum DeviceType
{
    DEVICE_SENSOR = 0,
    DEVICE_ACTUATOR = 1,
    DEVICE_CONTROLLER = 2,
};

struct DeviceInfo
{
    uint32 id;
    string[64] name;
};

service DeviceQuery
{
    [method=1]
    int GetInfo([in] uint32 slot, [out] DeviceInfo info);
};
"""

    def test_enum_defines(self):
        h = _gen_h(self.ENUM_STRUCT_IDL)
        assert "#define DEVICE_SENSOR  0" in h
        assert "#define DEVICE_ACTUATOR  1" in h
        assert "#define DEVICE_CONTROLLER  2" in h

    def test_enum_typedef(self):
        h = _gen_h(self.ENUM_STRUCT_IDL)
        assert "typedef uint32_t DeviceType;" in h

    def test_struct_typedef(self):
        h = _gen_h(self.ENUM_STRUCT_IDL)
        assert "typedef struct {" in h
        assert "uint32_t id;" in h
        assert "char name[65];" in h
        assert "} DeviceInfo;" in h

    def test_struct_only_idl(self):
        """Struct-only IDL (no enum) should still emit typedef."""
        h = _gen_h(STRUCT_IDL)
        assert "typedef struct {" in h
        assert "} DeviceInfo;" in h

    def test_no_types_section_when_no_types(self):
        """No type definitions section when IDL has no enums/structs."""
        h = _gen_h(NO_NOTIFICATIONS_IDL)
        assert "IDL type definitions" not in h


class TestCamelToSnake:
    """CamelCase to UPPER_SNAKE_CASE conversion."""

    def test_device_monitor(self):
        h = _gen_h(DEVICE_MONITOR_IDL)
        assert "DEVICE_MONITOR_SERVICE_ID" in h
        assert "DEVICE_MONITOR_METHOD_COUNT" in h
        assert "DEVICE_MONITOR_GET_DEVICE_COUNT" in h
