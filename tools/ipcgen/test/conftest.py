"""Shared fixtures and helpers for ipcgen tests."""

from ipcgen.lexer import tokenize
from ipcgen.parser import Parser, IdlFile


def parse(text: str) -> IdlFile:
    """Convenience: tokenize + parse in one call."""
    tokens = tokenize(text)
    return Parser(tokens).parse()


def make_idl(methods=None, notifications=None, name="Foo") -> IdlFile:
    """Build an IdlFile AST directly for emitter tests."""
    return IdlFile(
        service_name=name,
        methods=methods or [],
        notifications=notifications or [],
    )


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
