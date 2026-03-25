# MCU Firmware Template (aether-lite)

Bare-metal firmware template demonstrating an **aether-lite** service on an
ARM Cortex-M MCU communicating over UART.

> **This code is a template.** It references MCU HAL functions that do not
> exist on a desktop host and is **not** added to the project CMake build.
> Copy these files into your firmware project and adapt the HAL stubs.

## Overview

The template implements a `DeviceService` with three RPC methods and one
periodic notification:

| Type         | ID | Name             | Description                        |
|--------------|----|------------------|------------------------------------|
| Method       |  1 | `GetDeviceInfo`  | Returns device name + FW version   |
| Method       |  2 | `ReadSensor`     | Reads an ADC sensor as float32     |
| Method       |  3 | `SetLedState`    | Turns an LED on or off             |
| Notification |  1 | `SensorUpdate`   | Periodic sensor reading broadcast  |

Communication uses UART at 115 200 baud with the standard aether-lite
binary framing (sync + length + header + payload + CRC-32).

## File Layout

```
examples/mcu-firmware/
  main.c              — Startup, HAL wiring, ring buffer, main loop
  device_handlers.h   — Service/method IDs, packed wire structs
  device_handlers.c   — Handler implementations
```

## Prerequisites

* ARM GCC toolchain (`arm-none-eabi-gcc`)
* An STM32 (or similar Cortex-M) BSP / HAL package
* The `aether-lite/` header directory on your include path

## Porting Guide

1. **Copy** `main.c`, `device_handlers.h`, and `device_handlers.c` into your
   firmware source tree.
2. **Add** the `aether-lite/` headers to your compiler include path
   (`-I path/to/aether-lite`).
3. **Replace every `/* >>> REPLACE …` stub** in `main.c` with calls to your
   MCU's HAL (e.g., `HAL_UART_Transmit` on STM32).
4. **Hook the UART RX interrupt** to call `ring_push()` so incoming bytes
   land in the ring buffer.
5. **Adjust `aether_lite_config.h`** if your MCU is very constrained:
   set `AL_CRC_USE_TABLE 0` to save 1 KB flash at the cost of slower CRC,
   or reduce `AL_MAX_PAYLOAD` if 256 bytes is too large.
6. Build and flash.

## HAL Integration

The three callbacks in `al_hal_t` map directly to MCU peripherals:

| Callback       | Typical MCU implementation                         |
|----------------|----------------------------------------------------|
| `rx_read`      | Drain bytes from a UART RX ring buffer              |
| `tx_write`     | Blocking UART transmit (`HAL_UART_Transmit`)        |
| `get_tick_ms`  | Return the SysTick millisecond counter (`HAL_GetTick`) |

The ring buffer included in `main.c` is ISR-safe for a single-producer
(UART RX IRQ) / single-consumer (main loop calling `al_poll`) pattern.
