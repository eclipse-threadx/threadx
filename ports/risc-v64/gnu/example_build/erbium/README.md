# ThreadX example for Erbium

This directory runs the standard ThreadX demo on one hart of Erbium, the
16-hart RISC-V platform of the OpenHW Foundation CORE-ET project, using the
RISC-V64 GNU port in machine mode. The public `et-platform` functional
simulator, `erbium_emu`, is the reference target.

## Hardware used

| Item | Address | Notes |
|------|---------|-------|
| Hart 0 | - | RV64IMC, machine mode; other harts are parked |
| MRAM | `0x4000_0200` | 16 MiB window; the first 512 bytes belong to the boot flow |
| System registers | `0x0200_0000` | `SystemConfig` bit 6 enables the UART |
| UART0 | `0x0200_4000` | Original Shakti integration, polling, 115200 8N1 |
| Machine timer | `0x80F4_0200` | `mtime`, with `mtimecmp` at `+0x08` |
| PLIC | `0xA000_0000` | Machine-mode context of hart 0; sources 1 to 6 |

## Port notes

* **Soft float.** Erbium implements only part of the F extension: divide,
  square root and conversions between single precision and 64-bit integers
  raise an emulation exception. The example and the ThreadX library are
  built for `rv64imc_zicsr_zifencei` with the `lp64` ABI, so the port saves
  no FP state. `csr.h` rejects a build with F enabled.
* **No C library.** The `riscv64-unknown-elf` toolchain installed by
  `scripts/install_riscv.sh` ships `lp64d` libraries only, so the example
  links with `-nostdlib` and `board.c` provides `memset`. A missing runtime
  helper shows up as a link error, not as a mix of ABIs.
* **No WFI.** The example never executes `wfi`, like the Zephyr Erbium
  port. `TX_USE_WFI_IDLE` stays undefined, so the scheduler spins while
  idle, and parked harts and fatal-trap halts spin as well.
* **Trap vector.** Erbium ignores `mtvec` base bits 11:1, so the trap
  vector must be 4 KiB aligned. `trap_entry` is placed in its own 4 KiB
  aligned section and used in direct mode.
* **PLIC.** The silicon hardwires source priorities to 1 and thresholds to
  0. The simulator models them as writable registers that reset to 0, so
  `plic_init()` writes the hardwired values. Drivers register a callback and
  enable their source with `plic_irq_enable()`.
* **Clocks.** The UART divisor assumes a 400 MHz input clock and the tick
  assumes a 2 MHz `mtime` rate, the values used by the Zephyr and NuttX
  Erbium ports. If the boot firmware programs other rates, define
  `ERBIUM_UART_CLOCK_HZ` and `ERBIUM_MTIME_FREQ_HZ` when you build.
* **UART.** The console only polls. `uart_init()` sets the UART enable bit
  in `SystemConfig` with a read-modify-write, which leaves the watchdog
  setting as the boot flow left it.

## Building

Install the toolchain with `scripts/install_riscv.sh`, or put another
`riscv64-unknown-elf` GCC on `PATH`, then run:

```bash
./build.sh
```

This configures `CMakeLists.txt` with `erbium_gnu.cmake`, builds
`libthreadx.a` for the soft-float ABI and links `build/demo_threadx.elf`.
Set `BUILD_DIR` to build elsewhere.

## Running on the simulator

Build `erbium_emu` from
[et-platform](https://github.com/aifoundry-org/et-platform). The example was
tested at revision `836a4ab600e93c3059bb58c898edbc37744cd8d0`:

```bash
git clone https://github.com/aifoundry-org/et-platform.git
cd et-platform
git checkout 836a4ab600e93c3059bb58c898edbc37744cd8d0
cmake -S erbium-hal -B build-hal -DCMAKE_INSTALL_PREFIX="$PWD/install"
cmake --build build-hal --target install
cmake -S sw-sysemu -B build-emu -GNinja -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS=-Wno-error=unused-result \
      -DCMAKE_PREFIX_PATH="$PWD/install"
cmake --build build-emu --target erbium_emu
```

At that revision a release build with a distribution GCC that enables
`_FORTIFY_SOURCE` stops on an unused `write()` result in the UART model;
the `CMAKE_CXX_FLAGS` setting above keeps that warning from being an error.

Load the ELF and start hart 0 at the MRAM load address, bypassing the boot
ROM:

```bash
erbium_emu -elf_load build/demo_threadx.elf -reset_pc 0x40000200 \
    -minions 1 -single_thread -max_cycles 100000000 \
    -uart_rx_file /dev/null -uart_tx_file uart.log
cat uart.log
```

The log starts with `[UART0] : Erbium UART initialized`, followed by lines
from all eight demo threads. The simulator ends with a `max cycles reached`
error once the cycle budget is spent; judge the run by the UART log, not by
the exit status. With the default clocks one ThreadX tick is 2,000,000
simulated cycles, so the 100,000,000-cycle run shows `thread_0` five times.

The simulator verifies register accesses, the trap flow and the console.
It does not verify baud timing or the timer rate of real silicon.

## Simulator regression tests

With `erbium_emu` on `PATH`, run:

```bash
ERBIUM_EMU=erbium_emu ./test/run_simulator_tests.sh
```

The UART test checks that a long polling write does not lose ThreadX ticks.
The PLIC test changes one source in timer context while a thread changes
another, and checks that neither update is lost. The runner builds both
images with CMake and Ninja and reports a failure if either check fails.
These tests use the simulator's UART drain rate, not physical baud timing.

## Files

| File | Purpose |
|------|---------|
| `entry.S` | Reset entry: parks other harts, sets `gp`, stack and `.bss` |
| `tx_initialize_low_level.S` | Trap vector and `_tx_initialize_low_level` |
| `trap.c` | Dispatches the timer, the PLIC and fatal exceptions |
| `board.c` | `board_init()` and `memset` |
| `plic.c`, `plic.h` | PLIC driver for the hart's machine-mode context |
| `hwtimer.c`, `hwtimer.h` | Periodic tick from `mtime` and `mtimecmp` |
| `uart.c`, `uart.h` | Polling console |
| `demo_threadx.c` | The standard ThreadX demo |
| `link.lds` | MRAM layout |
| `erbium_gnu.cmake` | Toolchain file: `rv64imc_zicsr_zifencei`, `lp64` |
| `CMakeLists.txt`, `build.sh` | Build of the library and the demo |
| `test/` | Simulator tests for UART timekeeping and PLIC updates |

## References

* [Erbium processor](https://github.com/openhwfoundation/core-et-erbium)
* [UART registers](https://github.com/openhwfoundation/core-et-erbium/blob/325b32b7efaa2c2ab9c91001c89d3f49a4740826/doc/uart.md)
* [CPU memory map](https://github.com/openhwfoundation/core-et-erbium/blob/325b32b7efaa2c2ab9c91001c89d3f49a4740826/doc/cpu_mm.md)
* [Platform interrupts](https://github.com/openhwfoundation/core-et-erbium/blob/325b32b7efaa2c2ab9c91001c89d3f49a4740826/doc/interrupts.md)
* [CPU subsystem](https://github.com/openhwfoundation/core-et-erbium/blob/325b32b7efaa2c2ab9c91001c89d3f49a4740826/doc/cpu_subsystem.md)
* [et-platform simulator](https://github.com/aifoundry-org/et-platform)
