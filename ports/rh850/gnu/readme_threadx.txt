                  Eclipse Foundation's RTOS, ThreadX for Renesas RH850

                                  Using the GNU Tools


1.  Building the ThreadX run-time Library

The port is for the RH850 G4KH core (RH850/U2C), RH850 ABI, with or without
the FPU. It needs the GNU v850-elf toolchain with picolibc, release v1.2.0
(GCC 14.3.0, binutils 2.43.1, picolibc 1.8.12):

  https://github.com/linumiz/rh850-gcc-toolchain/releases/tag/v1.2.0

with its bin directory on the PATH:

  v850-elf-gcc --version

From the ThreadX top-level directory:

  cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/rh850_gnu.cmake .
  cmake --build ./build/

cmake/rh850_gnu.cmake sets the core flags (-mv850e3v5 -mrh850-abi) and
includes cmake/v850-elf.cmake for the tools. The result is build/libthreadx.a.


2.  Build options

RH850_FLOAT=hard (default)   FPU code; FPSR and FPEPC are saved in every
                             thread frame and in every interrupt frame
RH850_FLOAT=soft             -msoft-float; no FPU use, no FPU context
TX_RH850_LOW_POWER=ON        tickless idle through utility/low_power
                             (compile definition TX_LOW_POWER)
TX_RH850_BUILD_QEMU_EXAMPLE=ON
                             the QEMU example in example_build

In tx_user.h (or with -D):

TX_PORT_MAX_IPL              default 0: TX_DISABLE masks every EIINT. With
                             n > 0 the priority numbers 0 to n-1 stay
                             unmasked inside ThreadX; their ISRs must not call
                             any ThreadX service
TX_PORT_FREE_MEMORY_SIZE     default 8192: size of tx_port_free_memory, the
                             block passed to tx_application_define
TX_TIMER_TICKS_PER_SECOND    default 1000


3.  Example build and tests

ports/rh850/gnu/example_build/qemu_rh850_u2c runs samples/demo_threadx.c,
unmodified, with a monitor thread on the rh850-u2c machine of
qemu-system-rh850, release rh850-v1.2.0:

  https://github.com/linumiz/qemu-rh850/releases/tag/rh850-v1.2.0

The monitor checks every sample counter each second for 10 s and ends the run
through the QEMU test device:

  cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/rh850_gnu.cmake \
      -DCMAKE_BUILD_TYPE=Release -DTX_RH850_BUILD_QEMU_EXAMPLE=ON .
  cmake --build ./build/
  ctest --test-dir build --output-on-failure

or by hand:

  qemu-system-rh850 -M rh850-u2c,test-exit=on -icount shift=4,sleep=on \
      -nographic -kernel build/ports/rh850/gnu/example_build/qemu_rh850_u2c/demo_threadx.elf

The board files (start-up, vectors, INTC, OSTM, RLIN3 console, test device,
linker script) are in the same directory; board_sources.cmake lists them for
the example and for the regression tests.

test/tx/cmake/rh850 builds the generic regression suite (test/tx/regression)
on the same board, with the test list and the six build configurations of
test/tx/cmake/riscv:

  test/tx/cmake/rh850/run.sh hard build all
  test/tx/cmake/rh850/run.sh hard test all


4.  System Initialization

The board's crt0.S starts at the reset vector (address 0), sets up the reset
stack, copies .data, clears .bss and calls main, which calls tx_kernel_enter.
_tx_initialize_low_level runs with PSW.ID = 1: the reset stack becomes the
system stack, and tx_application_define gets tx_port_free_memory, a block of
its own apart from the system stack. The board starts the tick in
tx_port_setup_timer_interrupt: OSTM0 at TX_TIMER_TICKS_PER_SECOND, at the
lowest interrupt priority.


5.  Register Usage and Stack Frames

A suspended thread always has one 144-byte frame on its stack, whether it was
switched by a ThreadX call or by an interrupt:

    Offset        Register
    0x00          PLMR
    0x04          FPEPC (hard float)
    0x08          FPSR  (hard float)
    0x0C          EIPSW
    0x10          EIPC
    0x14          r1
    0x18          r2
    0x1C - 0x8C   r4 - r31

_tx_thread_system_return saves the frame and switches through TRAP0 (with a
SYNCE first when the FPU is used). A new thread starts from a frame made by
_tx_thread_stack_build, with PSW.ID = 0, PLMR open and, with the FPU, PSW.CU0
and FPSR.FS set.


6.  Interrupt Handling

Every EIINT goes through rh850_eiint_entry (direct vectors). EIPC, EIPSW and
EIIC are saved before interrupts are enabled again. The outermost level saves
the full frame on the interrupted stack and moves to the system stack; a
nested level saves a 96-byte call-used frame on the system stack. The board
dispatches by EIIC with interrupts enabled; the ISPR ceiling of the INTC holds
off equal and lower priorities until EIRET. Threads are switched only at the
outermost exit, by an EIRET into the scheduler.

TX_DISABLE, TX_RESTORE and tx_interrupt_control use the PLMR core mask
register, not PSW.ID; the interrupt posture is the old PLMR value. A thread is
never switched out while its saved PLMR is masked.


7.  Floating Point

With RH850_FLOAT=hard, FPSR and FPEPC are part of every thread frame and of
the nested interrupt frame, and new threads start with the FPU enabled. A
thread with a pending imprecise FPU exception (FPEC.FPIVD) is not switched out
at the interrupt exit. While it is pending, both restore paths keep FPEPC and
the FPSR cause bits for that exception. RH850_FLOAT_HARD (0 or 1) is set from
RH850_FLOAT by ports/rh850/gnu/CMakeLists.txt; the library and the application
must use the same value.


8.  Idle and Low Power

The scheduler idles in HALT with PSW.ID = 0 and PLMR open. With TX_LOW_POWER
it calls tx_low_power_enter, halts with PSW.ID = 1 (an interrupt request still
ends HALT), calls tx_low_power_exit and then takes the request. The board
provides tx_port_low_power_timer_setup and tx_port_low_power_adjust; the
example board runs the tick on the OSTM0 free-running compare and keeps the
rest of a period in the counter.


9.  Validation

The regression suite in test/tx/cmake/rh850 passes in all six configurations,
with hard and soft float, under qemu-system-rh850 rh850-u2c. The library builds
with the strict warning flags of the regression build.


10. Revision History

For generic code revision information, refer to readme_threadx_generic.txt.

11-05-2026  Parthiban Nallathambi  Initial RH850 G4KH GNU port
