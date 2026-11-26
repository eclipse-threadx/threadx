# ThreadX regression test on the TriCore TC1.6.2 port

This directory builds the ThreadX regression suite in `test/tx/regression` for
the AURIX TC375 Lite Kit. All tests run in one image: `testcontrol.c` is built
without `CTEST`, so its control thread runs every test in `test_control_tests[]`
one after the other. The test output goes to the UART of the Lite Kit.

## Requirements

- AURIX TC375 Lite Kit (TC375TP)
- AURIX GCC with `tricore-elf-gcc` on `PATH` (see [Compiler](#compiler))
- CMake 3.13 or later and Ninja
- A terminal on the virtual COM port of the Lite Kit: 115200 baud

## Compiler

The tests ran with the following AURIX GCC for Linux x86-64:

| Item         | Version                                                    |
|--------------|------------------------------------------------------------|
| Package      | AURIX GCC 03_2026 (`aurixgcc_03-2026_Linux_x86-x64`)       |
| GCC          | 11.3.1 20221230 (AURIX(tm) GCC - Built 2026-03-19 10:12:22) |
| Binutils     | 2.40.50.20230607                                           |
| C library    | newlib 4.2.0                                               |
| Start-up code| `lib/gcc/tricore-elf/11.3.1/crt0-tc38xx.o`                 |

Infineon provides the AURIX GCC for download at
<https://softwaretools.infineon.com/assets/com.ifx.tb.tool.aurixgcc>.
`tricore-elf-gcc --version` prints the GCC version and the build date of the
installed package. The download also offers a Windows package; `run.bat`
builds with it, but the tests ran only with the Linux package.

## Build

```bash
./run.sh build                      # default_build
./run.sh build trace_build          # one or more configurations
./run.sh build all                  # every configuration
```

On Windows, `run.bat` takes the same arguments:

```bat
run.bat build
run.bat build trace_build
run.bat build all
```

The configurations are those of `test/tx/cmake/CMakeLists.txt`:
`default_build`, `disable_notify_callbacks_build`, `stack_checking_build`,
`stack_checking_rand_fill_build`, `trace_build`, `misra_build` and
`misra_trace_build`. Six more configurations each set one option that has
code of its own in the port: `disable_inline_build` (`TX_DISABLE_INLINE`),
`port_use_ccpn_build` (`TX_PORT_USE_CCPN`), `timer_process_in_isr_build`
(`TX_TIMER_PROCESS_IN_ISR`), `not_interruptable_build`
(`TX_NOT_INTERRUPTABLE`), `execution_profile_build`
(`TX_EXECUTION_PROFILE_ENABLE`, with `utility/execution_profile_kit`) and
`low_power_build` (`TX_LOW_POWER`, with `utility/low_power`). Each one
builds into `build/<configuration>/regression/`:
`regression.elf`, `regression.map` and `regression.hex`.

For `TX_EXECUTION_PROFILE_ENABLE` and `TX_LOW_POWER`, the port implements only
the callouts, and the regression test does not test their function. In
`execution_profile_build`, the scheduler calls `_tx_execution_thread_enter()`
and `_tx_execution_thread_exit()`, and the timer interrupt calls
`_tx_execution_isr_enter()` and `_tx_execution_isr_exit()`. No test reads
the measured times. In `low_power_build`, the idle loop of the scheduler calls
`tx_low_power_enter()` and `tx_low_power_exit()`. The build defines none of
the `TX_LOW_POWER_TIMER_SETUP`, `TX_LOW_POWER_TICKLESS` and
`TX_LOW_POWER_USER_*` macros, so the CPU does not enter a low-power mode and
no timer ticks are adjusted. Both configurations show only that the callouts
do not disturb the kernel.

## Board support

The test runs on the example of the port in
`ports/tricore_tc1_6_2/gnu/example_build`, without a driver library. The
start-up code is `crt0-tc38xx.o` of the compiler, and the system runs from
the back-up clock of 100 MHz. `bsp/` adds:

- `tx_initialize_low_level.c`, a copy of the example's file whose timer
  interrupt calls `test_interrupt_dispatch()` before
  `_tx_timer_interrupt()`, as `test/tx/cmake` does for the Linux port. It
  also offsets each timer compare by a pseudo-random 0 to 255 STM ticks.
  Without the offset, the timer can stay in phase with a test loop that
  waits for the interrupt, and the test never ends.
- `main.c` replaces `main()` of the example's `sample_threadx.c`, which the
  test does not build. It clears the test memory, initialises ASCLIN0
  (P14.0, 115200 baud from the fast ASCLIN clock of 100 MHz), sends `printf`
  output to it through `write()`, provides `external_exit()` and starts the
  test control.
- `regression.ld`, given with `-T` after the example's `sample_threadx.ld`,
  places the section `.bss_cpu1` in DSPR1.

The build further changes the regression suite:

- `testcontrol.c` is compiled with `EXTERNAL_EXIT` and with `main` renamed to
  `tx_regression_main`. `regression/CMakeLists.txt` sets
  `TEST_CONTROL_MEMORY_SIZE` to 0x6000 bytes and `TESTS_MEMORY_SIZE` to
  0x1C000 bytes, and `TEST_MEMORY_PLACEMENT` places both arrays in DSPR1
  (`.bss_cpu1` of `bsp/regression.ld`). The default of 0x60000 bytes each
  does not fit the RAM of the TC375. The largest test uses about 94 KiB.
- `threadx_initialize_kernel_setup_test.c` is not built. It has its own `main`
  and is not part of `test_control_tests[]`.

The CSA area of CPU0 (`__CSA_SIZE`, 16 KiB, in `sample_threadx.ld`) holds
the CSA chains of the 25 threads of the multi-level preemption threshold
test. With 8 KiB, that test raises the free context list depletion trap
(FCD); 12 KiB suffice.

## Expected output

The run takes about half a minute. Each test prints one line, and the run
ends with:

```
**** Testing Complete ****
**** Test Summary:  Tests Passed:  97   Tests Failed:  0    System Errors:   0
**** REGRESSION EXIT CODE 0 ****
```

`test_control_return()` prints `Running Queue Max Message Size Test (32)`
after every test. The line belongs to the test control, not to a failure.
