<!--
  Copyright (c) 2026 Eclipse ThreadX contributors

  This program and the accompanying materials are made available under the
  terms of the MIT License which is available at
  https://opensource.org/licenses/MIT.

  SPDX-License-Identifier: MIT
-->

# ESP32-C6 (RISC-V32) Bare-Metal ThreadX Example Build

This directory contains the **Eclipse ThreadX** bare-metal bring-up example for the **Espressif ESP32-C6** microcontroller board (`ESP32-C6-DevKitC-1`).

---

> [!WARNING]
> ### Current Project Status & Known Issues
> 
> **This example build is currently INCOMPLETE and has active hardware bring-up issues:**
> 
> 1. **Serial Output & Reset Loop**: When flashed to physical hardware (`write_flash 0x10000 build/esp32c6_demo.bin`), the serial monitor shows **no console output** and the board continuously resets every few seconds (watchdog / trap reset loop).
> 2. **Timer Interrupt Freeze**: When stepping through JTAG with OpenOCD/GDB, execution successfully reaches `thread_0_entry`. However, after calling `tx_thread_sleep()`, the ThreadX scheduler gets **stuck inside `_tx_thread_schedule_loop()`** at `wfi` because the 100 Hz SYSTIMER hardware tick interrupt is not advancing `_tx_timer_system_clock`.
> 
> **Where to start for the next developer**:
> * Refer to [`threadx_ESP32C6_native_WIFI_plan.md`](threadx_ESP32C6_native_WIFI_plan.md) for the architectural blueprint.

---

## Architecture Overview

* **Target CPU**: ESP32-C6 High-Performance RISC-V32 Core (`rv32imac_zicsr_zifencei`, 160 MHz)
* **RTOS Kernel**: Eclipse ThreadX RISC-V32 GNU Port
* **Boot Chain**: Espressif Mask ROM $\rightarrow$ 2nd-Stage Bootloader $\rightarrow$ ThreadX Application Entry (`_start`)
* **Drivers Included**:
  * Console UART Output (`esp_rom_output_tx_one_char`)
  * RISC-V PLIC & ESP32-C6 Interrupt Matrix Router (`interrupt.c`)
  * Hardware SYSTIMER 100 Hz Kernel Tick Driver (`systimer.c` using pinned `systimer_hal.c`)

---

## Build Prerequisites

Ensure the following tools are installed and present in your system `PATH`:

1. **Toolchain**: `riscv32-esp-elf-gcc` (Espressif RISC-V GCC 14+)
2. **Build System**: CMake 3.16+ and Ninja (or Make)
3. **Flash Tool**: `esptool.py` (Installed via `pip install esptool`)

---

## Building the Example Firmware

Run the following commands from this directory:

```bash
# Configure CMake build system
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/esp32c6-toolchain.cmake

# Build executable ELF and bootable binary image
cmake --build build
```

Upon successful compilation, the output artifacts will be placed under `build/`:
* `build/esp32c6_demo.elf`: Symbol-rich executable for OpenOCD/GDB debugging.
* `build/esp32c6_demo.bin`: Bootable binary image formatted for Espressif 2nd-stage bootloader.

---

## Flashing to ESP32-C6 Hardware

Connect your ESP32-C6 board via USB and run `esptool.py`:

```bash
esptool.py --chip esp32c6 -p COM3 -b 460800 write_flash 0x10000 build/esp32c6_demo.bin
```

*(Replace `COM3` with your operating system's serial port path, e.g. `/dev/ttyUSB0` on Linux).*

---

## Monitoring Serial Diagnostics

Open any serial terminal emulator (such as PuTTY, TeraTerm, or `idf_monitor`) configured for:
* **Baudrate**: 115200
* **Data bits**: 8
* **Stop bits**: 1
* **Parity**: None

---

## Hardware JTAG Debugging with OpenOCD & GDB

1. **Launch OpenOCD**:
   ```bash
   openocd -f board/esp32c6-builtin.cfg
   ```
2. **Launch GDB**:
   ```bash
   riscv32-esp-elf-gdb build/esp32c6_demo.elf
   ```
3. **Connect and Debug in GDB**:
   ```gdb
   target remote :3333
   mon reset halt
   load
   hbreak thread_0_entry
   continue
   ```
