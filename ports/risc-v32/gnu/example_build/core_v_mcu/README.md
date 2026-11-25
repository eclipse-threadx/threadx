# ThreadX Port for CORE-V MCU

This directory contains the Board Support Package (BSP) and demo application for
running Eclipse ThreadX on the
[CORE-V MCU](https://github.com/openhwgroup/core-v-mcu) — an open-source 32-bit
RISC-V SoC based on the CV32E40P core (RV32IMC).  The reference hardware is a
[Digilent Nexys A7](https://digilent.com/reference/programmable-logic/nexys-a7/start)
FPGA board running the synthesised CORE-V MCU bitstream.

---

## Table of Contents

1. [Hardware overview](#1-hardware-overview)
2. [Software prerequisites](#2-software-prerequisites)
3. [Directory structure](#3-directory-structure)
4. [Architecture notes](#4-architecture-notes)
5. [Memory map](#5-memory-map)
6. [Building](#6-building)
7. [Flashing and debugging](#7-flashing-and-debugging)
8. [BSP API reference](#8-bsp-api-reference)
9. [Unit tests](#9-unit-tests)
10. [Adapting to a different board](#10-adapting-to-a-different-board)
11. [Setup and deployment scripts](#11-setup-and-deployment-scripts)

---

## 1. Hardware overview

| Item | Detail |
|------|--------|
| CPU | CV32E40P — RV32IMC, in-order, 4-stage pipeline |
| ISA | `rv32imc_zicsr` (integer, compressed, multiply/divide, CSR instructions) |
| L2 SRAM | 512 KB at `0x1c000000` (code + data + stack) |
| Interrupt controller | PULP APB interrupt controller (32 lines, **not** a standard CLINT) |
| Timer | PULP FC Timer at `0x1A10B000`, 32 kHz reference clock |
| UART | UDMA UART channel 0 at `0x1A102080` |
| GPIO | PULP GPIO controller at `0x1A101000` |
| FLL | Frequency Locked Loop — 5 MHz input (FPGA), 50 MHz output |
| Debug probe | [Ashling Opella LD](https://www.ashling.com/ashling-opella-ld/) via JTAG |

The CV32E40P does **not** implement the standard RISC-V CLINT (`mip`/`mie` per-source
masking).  Individual interrupt lines are enabled and cleared through PULP APB
registers, and `mstatus.MIE` (bit 3) acts as the single global enable.

---

## 2. Software prerequisites

Run `install_deps.sh` to install all dependencies in one step:

```bash
bash install_deps.sh
```

Or install individually:

| Tool | Minimum version | Notes |
|------|----------------|-------|
| `riscv-none-elf-gcc` | 15.x | xPack riscv-none-elf-gcc — run `bash install_deps.sh` |
| CMake | 3.15 | `apt install cmake` |
| Ninja | 1.10 | `apt install ninja-build` |
| OpenOCD | 0.12 | `apt install openocd` |
| GDB | any multiarch | `apt install gdb-multiarch` |

> **Note:** Use the [xPack riscv-none-elf-gcc](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack)
> toolchain — a cross-platform (Linux/macOS/Windows) multilib bare-metal
> toolchain analogous to `arm-none-eabi-gcc`.  Its multilib includes
> rv32imc/ilp32 (soft-float) libgcc required for the CV32E40P.  Do **not**
> use `riscv32-unknown-elf-gcc` (riscv-collab) — that release is built
> `--disable-multilib --with-abi=ilp32d` and cannot link soft-float objects.

---

## 3. Directory structure

```
core_v_mcu/
├── bsp/                        Board Support Package
│   ├── fll.c                   Frequency Locked Loop driver
│   ├── gpio.c                  GPIO driver
│   ├── irq.c                   PULP APB interrupt controller driver
│   ├── string.c                Freestanding memset/memcpy shim
│   ├── system_core_v_mcu.c     Top-level BSP init and ISR dispatcher
│   ├── timer_irq.c             PULP FC Timer driver
│   └── uart_driver.c           UDMA UART driver
├── include/                    BSP headers
│   ├── bits.h                  Bit-manipulation macros
│   ├── csr.h                   CSR access macros
│   ├── fll.h
│   ├── gpio.h
│   ├── io.h                    MMIO read/write inlines
│   ├── irq.h
│   ├── memory_map.h            Peripheral base addresses
│   ├── properties.h            Clock/frequency constants
│   ├── pulp_mem_map.h          PULP peripheral register offsets
│   ├── string.h                Freestanding string shim header
│   ├── system_core_v_mcu.h
│   ├── timer.h
│   ├── timer_irq.h
│   ├── tx_user.h               ThreadX compile-time configuration
│   └── uart_driver.h
├── tests/                      Host-compiled unit tests (no cross-compiler needed)
│   ├── mock/
│   │   ├── mmio_mock.c         Replaces io.h MMIO inlines with a software map
│   │   └── mmio_mock.h
│   ├── test_irq.c              Tests for irq.c
│   ├── test_timer.c            Tests for timer_irq.c
│   └── CMakeLists.txt
├── crt0.S                      C runtime startup (clear BSS, call main)
├── vectors.S                   32-entry vectored interrupt table
├── tx_initialize_low_level.S   ThreadX platform init (mtvec, stack pointer)
├── link.ld                     Linker script
├── demo_threadx.c              Demo application (blinky + UART heartbeat)
├── CMakeLists.txt              Build definition
├── build.sh                    One-shot build script
├── gdb_init                    GDB startup script
└── openocd-nexys-Ashling-Opella-LD.cfg   OpenOCD configuration
```

The ThreadX kernel sources are **not** duplicated here; they are referenced from the
repository root via `THREADX_ROOT`.

---

## 4. Architecture notes

### 4.1 Interrupt model

The CV32E40P uses a PULP/PULPissimo-style interrupt controller rather than the
standard RISC-V CLINT.  Key differences:

- **32 direct interrupt lines** connect to the core.  `mcause` for an external
  interrupt is `0x80000000 | N`, where `N` (bits 4–0) is the line number.
- **Per-line masking** is done through APB registers in the PULP IRQ controller
  (`IRQ_REG_MASK_SET_OFFSET` / `IRQ_REG_MASK_CLEAR_OFFSET`) — not through the `mie`
  CSR.
- **Global enable** is `mstatus.MIE` (bit 3).  ThreadX manages this itself: the
  scheduler sets MIE just before entering the WFI idle loop in
  `_tx_thread_schedule`.

ISR dispatch flow:

```
CPU takes interrupt
  → PC jumps to mtvec base (vectored mode, bit 0 = 1)
  → vectors.S entry for line N → tx_trap_entry
  → tx_trap_entry saves registers, calls _tx_thread_context_save
  → tx_trap_handler(mcause, mepc, mtval) dispatches via isr_table[mcause & 0x1f]
  → ThreadX timer ISR (line 10) calls _tx_timer_interrupt
  → _tx_thread_context_restore (never returns to tx_trap_entry)
```

### 4.2 `mtvec` alignment

The CV32E40P requires the vector table base to be **256-byte aligned**.  The linker
script places `.vectors` at `0x1c000800`, which satisfies this.  `mtvec` is written
as `0x1c000801` (base + vectored-mode bit).

### 4.3 ThreadX timer tick

- Reference clock: 32 768 Hz (ARCHI_REF_CLOCK)
- Tick rate: 100 Hz → compare value 327 (~0.1 % drift)
- The timer auto-reloads on compare match (cycle mode)
- `_tx_timer_interrupt` is registered in `isr_table[10]` during `system_init()`

### 4.4 FLL and clock

On the Nexys A7 FPGA, the FLL input is 5 MHz (`ARCHI_FPGA_FREQUENCY`).  After
`pi_fll_init()` the system clock is 50 MHz (`DEFAULT_SYSTEM_CLOCK`).  The UART baud
rate divisor is computed from this 50 MHz clock.

### 4.5 Freestanding build

The port builds with `-ffreestanding -nodefaultlibs` — no newlib or libc.
`bsp/string.c` provides minimal `memset` and `memcpy` for ThreadX internal use.

---

## 5. Memory map

| Region | Start | Size | Contents |
|--------|-------|------|----------|
| Vector table | `0x1c000800` | 128 B | 32 × 4-byte jump instructions to `tx_trap_entry` |
| `.text` (boot address) | `0x1c000880` | — | Code + read-only data |
| `.data` | after `.text` | — | Initialised data |
| `.bss` | after `.data` | — | Zero-initialised data |
| ThreadX free memory | after `.bss` | — | Heap pools, thread stacks |
| Stack | top of L2 | 8 KB | Initial/idle stack grows downward from `0x1c080000` |

L2 total: 512 KB (`0x1c000000`–`0x1c07ffff`).

Peripheral base addresses (see `include/memory_map.h`):

| Peripheral | Address |
|------------|---------|
| PULP IRQ controller | `0x1A109800` |
| PULP FC Timer | `0x1A10B000` |
| PULP GPIO | `0x1A101000` |
| UDMA UART ch0 | `0x1A102080` |
| FLL | `0x1A100000` |

---

## 6. Building

### Quick start

```bash
cd ports/risc-v32/gnu/example_build/core_v_mcu
bash build.sh
```

The script configures and builds with Ninja.  The output is:

```
build/demo_threadx.elf   — ELF with debug symbols
build/demo_threadx.bin   — raw binary for flashing
build/demo_threadx.map   — linker map
```

### Manual CMake invocation

```bash
THREADX_ROOT=$(realpath ../../../../..)
cmake -B build -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="${THREADX_ROOT}/cmake/riscv-none-elf-rv32imc.cmake" \
      .
cmake --build build
```

### Toolchain file

`${THREADX_ROOT}/cmake/riscv-none-elf-rv32imc.cmake` sets:

```
CMAKE_C_COMPILER    riscv-none-elf-gcc
CMAKE_ASM_COMPILER  riscv-none-elf-gcc
CMAKE_OBJCOPY       riscv-none-elf-objcopy
CMAKE_C_FLAGS       -march=rv32imc_zicsr -mabi=ilp32 -mcmodel=medlow
```

---

## 7. Flashing and debugging

### 7.1 Quick deploy (recommended)

Use `deploy.sh` to automate the entire OpenOCD + GDB workflow in one command:

```bash
# Flash and run (leave target running after GDB exits):
bash deploy.sh

# Build first, then flash and run:
bash deploy.sh --build

# Flash and stop at main for interactive debugging:
bash deploy.sh --debug

# Override the ELF or the OpenOCD config:
bash deploy.sh --elf path/to/my.elf --openocd-cfg path/to/other.cfg
```

> **First time only:** run `bash ../../../../../samplex-fd/OpenHW/scripts/setup_opella.sh`
> (or the equivalent path) once to install the udev rule for the Opella LD.
> See [Section 11](#11-setup-and-deployment-scripts) for details.

### 7.2 Manual procedure

#### Connect hardware

1. Connect the Ashling Opella LD to the Nexys A7 JTAG header.
2. Connect the USB-UART cable (CORE-V MCU UART0 mapped to USB micro-B on Nexys A7).
3. Power on the board.

#### Start OpenOCD

```bash
openocd -f openocd-nexys-Ashling-Opella-LD.cfg
```

Expected output ends with `Ready for Remote Connections`.  OpenOCD listens on
port 3333 (GDB) and 4444 (telnet).

#### Flash and run with GDB

In a second terminal:

```bash
riscv-none-elf-gdb --command=gdb_init
```

`gdb_init` connects to OpenOCD, loads the ELF, sets a breakpoint at `main`, and
continues.  You should see the LED on pin 5 blinking at 1 Hz and the UART printing
`ThreadX on CORE-V MCU` followed by periodic `.` heartbeats at 115 200 baud.

#### UART monitor

```bash
minicom -b 115200 -D /dev/ttyUSBx   # adjust device as needed
```

#### Useful GDB commands

```
(gdb) info threads          — list ThreadX threads
(gdb) thread 2              — switch to a thread
(gdb) bt                    — backtrace
(gdb) monitor halt          — halt via OpenOCD
(gdb) monitor resume        — resume via OpenOCD
```

---

## 8. BSP API reference

### `system_core_v_mcu.h`

```c
void system_init(void);
```
Initialises FLL, UART, IRQ controller, and FC Timer.  Must be called once from
`main()` before `tx_kernel_enter()`.

### `uart_driver.h`

```c
int  uart_init(uint8_t uart_id, uint32_t baudrate, uint32_t periph_freq);
void uart_write_byte(uint8_t uart_id, uint8_t c);
int  uart_read_byte(uint8_t uart_id);   /* returns -1 if no data */
void uart_write_str(uint8_t uart_id, const char *str);
void uart_write_buf(uint8_t uart_id, const uint8_t *buf, uint32_t len);
```

`uart_id` 0 is the console UART.  `uart_read_byte` is non-blocking.

### `gpio.h`

```c
void gpio_init(void);
void gpio_set_output(uint32_t pin_mask);  /* configure pins as outputs */
void gpio_set(uint32_t pin_mask);         /* drive high */
void gpio_clear(uint32_t pin_mask);       /* drive low */
void gpio_toggle(uint32_t pin_mask);
```

`pin_mask` is a bitmask where bit N corresponds to GPIO pin N.

### `irq.h`

```c
void pulp_irq_init(void);
void irq_enable(uint32_t irq_id);
void irq_disable(uint32_t irq_id);
void irq_clint_enable(void);    /* set mstatus.MIE */
void irq_clint_disable(void);   /* clear mstatus.MIE */
```

`irq_id` values are defined as `IRQ_FC_EVT_*` in `irq.h`.  The timer uses
`IRQ_FC_EVT_TIMER0_LO` (= 10).

### `timer_irq.h`

```c
void     timer_irq_init(uint32_t ticks);
void     timer_irq_set_timeout(uint32_t ticks, bool enable);
uint32_t timer_irq_cycle_get_32(void);
```

### Registering a custom ISR

Add a function pointer to `isr_table` before calling `tx_kernel_enter()`:

```c
extern void (*isr_table[32])(void);

static void my_irq_handler(void) { /* ... */ }

/* In main() or tx_application_define(), before enabling the line: */
isr_table[MY_IRQ_LINE] = my_irq_handler;
irq_enable(MY_IRQ_LINE);
```

---

## 9. Unit tests

The tests compile for the **host** (no cross-compiler required) using a software
MMIO mock that backs the peripheral register space with a `uint32_t` array.

```bash
cd tests
cmake -B build -G Ninja
cmake --build build
./build/tx_core_v_mcu_tests
```

Expected output:

```
irq_tests: PASS
timer_tests: PASS
```

The mock is enabled by defining `CORE_V_MCU_MMIO_MOCK` before including `io.h`.
`mmio_mock.c` provides `mock_reg_read` / `mock_reg_write` helpers for assertions.

---

## 10. Adapting to a different board

To port to a different CORE-V MCU carrier board:

1. **Clock frequencies** — update `ARCHI_REF_CLOCK`, `ARCHI_FPGA_FREQUENCY`, and
   `DEFAULT_SYSTEM_CLOCK` in `include/properties.h`.

2. **Memory layout** — adjust `ORIGIN` and `LENGTH` for `L2` in `link.ld` and
   update `__boot_address` if the boot ROM routes differently.

3. **mtvec base** — if `.vectors` moves, update the address in `link.ld`.  The
   256-byte alignment requirement is a hard CV32E40P constraint.

4. **Debug probe** — replace `openocd-nexys-Ashling-Opella-LD.cfg` with a config
   matching your probe (e.g., `interface jlink` for a J-Link).

5. **GPIO pin** — `DEMO_LED_PIN` in `demo_threadx.c` selects the LED.  Change to
   match your board's LED assignment.

---

## 11. Setup and deployment scripts

### 11.1 `setup_opella.sh` — one-time host setup

Located in `samplex-fd/OpenHW/scripts/setup_opella.sh`.  Run **once per machine**
to grant user-space access to the Ashling Opella LD USB device.

```bash
# Native Linux:
sudo bash samplex-fd/OpenHW/scripts/setup_opella.sh

# WSL (also attempts usbipd-win USB forwarding):
bash samplex-fd/OpenHW/scripts/setup_opella.sh
```

What it does:

| Step | Native Linux | WSL |
|------|-------------|-----|
| Write `/etc/udev/rules.d/99-ashling-opella.rules` | ✓ | ✓ |
| Add user to `plugdev` group | ✓ | ✓ |
| Reload udev | ✓ | ✓ |
| `usbipd.exe attach --wsl` (v3: `wsl attach`) | — | automatic |

> **WSL — one-time bind (Windows admin required):**
> Before the first attach, run this once in an **elevated (Run as Administrator) PowerShell** on the Windows host:
> ```powershell
> usbipd bind --hardware-id 0b6b:0040
> ```
> After binding, `setup_opella.sh` handles subsequent attaches automatically.

> **WSL — if attach fails with "not USBIP capable" (usbipd-win v3 + Ubuntu 22.04+):**
> Ubuntu 22.04 and 24.04 enable systemd by default.  usbipd-win **v3** has a
> known bug where it incorrectly reports the kernel as not USBIP capable when
> systemd is running.  **Upgrade usbipd-win to v4 or later:**
> <https://github.com/dorssel/usbipd-win/releases>
>
> If you are on an older WSL2 kernel without USBIP support, first run:
> ```powershell
> wsl --update
> wsl --shutdown
> ```

After running, **re-plug the probe**.  On WSL, re-run the script after each WSL restart.

### 11.2 `deploy.sh` — flash and run

```bash
# Flash and run (leave target running):
bash deploy.sh

# Build first, then flash:
bash deploy.sh --build

# Flash and stop at main for interactive debug session:
bash deploy.sh --debug
```

The script starts OpenOCD in the background, waits for it to become ready,
then drives `riscv-none-elf-gdb` to load the ELF.  OpenOCD is always
stopped on exit (including Ctrl-C).

---

## Licence

New files in this directory are © 2026 Eclipse ThreadX contributors, MIT.

Files derived from
[core-v-freertos](https://github.com/openhwgroup/core-v-freertos) retain their
original Apache-2.0 copyright notices (ETH Zurich, GreenWaves Technologies, Wind
River Systems, SiFive Inc.) and are dual-licensed `Apache-2.0 AND MIT` (or
`(Apache-2.0 OR BSD-2-Clause) AND MIT` for `crt0.S`).  See individual file headers.
