           Eclipse ThreadX for TriCore TC1.6.2 (Infineon AURIX TC3)

                              Using the AURIX GCC Tools


1.  Prerequisites

The port runs on CPU0 of an AURIX TC3 device and was developed on the AURIX
TC375 Lite Kit (KIT_A2G_TC375_LITE). It requires the AURIX GCC
(tricore-elf-gcc) on PATH and uses no driver library.


2.  Building the ThreadX Run-time Library

With CMake from the repository root, run:

   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/tricore_tc1_6_2.cmake
   cmake --build build

The build produces the library tx.a, which the application links. tx.a does
not contain _tx_initialize_low_level and the timer interrupt, because both
depend on the board; the application provides them (section 4).

inc/tx_port.h holds the definitions for the ThreadX common sources and the
application, including the settings that tx_user.h or the command line may
override. src/tx_port_tricore.h holds the internal definitions of the port.


3.  Demonstration System

In example_build, run:

   ./build_threadx_sample.sh

This builds tx.a and sample_threadx.elf for the TC375 Lite Kit from:

  sample_threadx.c           samples/demo_threadx.c with the event trace
                             start (section 8)
  tx_initialize_low_level.c  low-level initialisation, trap vector table
                             with the ThreadX system call and timer
                             interrupt
  sample_threadx.ld          memory description of CPU0 of the TC375

The start-up code is crt0-tc38xx.o of the AURIX GCC. The AURIX GCC has no
TC37x device; -mcpu=tc38xx selects the same TC1.6.2 core and start-up code
that applies to the TC375 unchanged. sample_threadx.ld, given with -T,
replaces the TC38x memory description of the compiler. The start-up code is
located at 0x80000000, the cached address of 0xA0000000, at which the boot
mode header of the board starts CPU0. The build produces no boot mode header.

The clock system keeps its reset state: the system runs from the back-up
clock of 100 MHz, and the caches stay disabled. CPU1 and CPU2 stay halted.


4.  System Initialization

main calls tx_kernel_enter. Interrupts stay disabled from reset until
_tx_thread_schedule starts the first thread. The port clears PSW.IS at the
start of _tx_initialize_kernel_enter (TX_PORT_SPECIFIC_PRE_INITIALIZATION),
so traps and interrupts switch to the interrupt stack.

_tx_initialize_low_level:

  - sets the first unused memory to __TX_FREE_MEM, which sample_threadx.ld
    defines after the CSAs; the remaining data RAM of CPU0 is free for
    tx_application_define,
  - loads BTV with _tx_trap_table and BIV with __INTTAB_CPU0, the vector
    table of sample_threadx.ld; it clears the CPU0 ENDINIT bit for both,
  - sets up the ThreadX timer (section 7),
  - sets up the dispatch interrupt: TX_PORT_TRICORE_DISPATCH_SRC for CPU0
    with TX_PORT_TRICORE_DISPATCH_PRIORITY, enabled (section 5).


5.  Thread Scheduling

ThreadX enters the scheduler through system call
TX_PORT_TRICORE_SYSCALL_ID_SCHEDULE. The class 6 trap entry shall save the
lower context with SVLCX and call _tx_thread_syscall_handler with the trap
identification number. Any other number goes to tx_port_tricore_syscall_hook,
which runs like an interrupt handler; its weak default stops the CPU.

An interrupt or trap handler that makes another thread ready requests the
dispatch interrupt _tx_thread_dispatch_isr, a software request on the SRC at
TX_PORT_TRICORE_DISPATCH_SRC_ADDRESS (SRC_GPSR00 of the TC3xx, 0xF0038990, by
default) at TX_PORT_TRICORE_DISPATCH_PRIORITY (1 by default).


6.  Thread Context

A thread's context is its CSA chain; the port keeps the link to it in
tx_thread_csa (TX_THREAD_EXTENSION_0). The thread stack holds only local
data. A new thread starts at its entry function in supervisor mode with
interrupts enabled. The FPU state is part of the CSAs, so threads use the
FPU without setup.

CSA budget: a suspended thread holds one CSA per call level plus two; each
active interrupt or trap holds two plus one per call level. ThreadX stack
checking does not cover the CSAs. The CSA area of CPU0 (__CSA_SIZE, 16 KiB,
in sample_threadx.ld) shall hold the chains of all threads plus the deepest
interrupt nesting, up to LCX as set by the start-up code. If the CSAs for a
new thread would reach LCX, tx_thread_create or tx_thread_reset raises FCD.


7.  Timer and Interrupt Handling

The ThreadX timer uses STM0 compare 0 with TX_TIMER_TICKS_PER_SECOND (1000
by default) ticks per second. The timer interrupt has the priority
TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY (2 by default). If STM has passed
the next compare value, for example after a debugger halt, the ticks in
between are lost.

TX_DISABLE clears ICR.IE by default. With TX_PORT_USE_CCPN defined, it
raises ICR.CCPN to TX_PORT_CCPN (31 by default) instead and never lowers it;
interrupts above TX_PORT_CCPN stay enabled in ThreadX critical sections and
shall not call ThreadX services. TX_RESTORE restores the previous ICR.

An ISR that calls ThreadX services is a plain interrupt handler on CPU0 with
a priority above TX_PORT_TRICORE_DISPATCH_PRIORITY and, with
TX_PORT_USE_CCPN, not above TX_PORT_CCPN. _tx_thread_context_save and
_tx_thread_context_restore have no effect on this port.
TX_PORT_TRICORE_INTERRUPT(isr, priority) in tx_port.h defines such a handler
and places its vector table entry in the section .intvec_tc0_<priority>.
The linker script shall locate this section at BIV + 32 * priority;
sample_threadx.ld does so for the priorities 1 to 15.

Interrupts and traps taken from a thread switch to the CPU0 interrupt stack
(__ISTACK_SIZE, 4 KiB, in sample_threadx.ld). It holds the local data of all
nested handlers, including the scheduler's idle wait, so thread stacks need
no interrupt reserve.


8.  Event Trace

With TX_ENABLE_EVENT_TRACE, the application calls tx_trace_enable with its
trace buffer, at the earliest at the start of tx_application_define, because
kernel initialisation resets the trace. The time stamp is the lower word of
STM0 (TX_TRACE_TIME_SOURCE in tx_port.h).


9.  Execution Profile and Low Power

For TX_EXECUTION_PROFILE_ENABLE and TX_LOW_POWER the port implements only the
callouts. The regression tests show only that the callouts do not disturb
the kernel. TX_LOW_POWER_TIMER_SETUP, TX_LOW_POWER_TICKLESS and
TX_LOW_POWER_USER_* are not defined.


10. Improving Performance

The build scripts and cmake/tricore_tc1_6_2.cmake compile with -O0 -g3 for
debugging. The regression test also passes with -O2. TX_DISABLE_ERROR_CHECKING
removes the ThreadX API error checking.


11. Revision History

For generic code revision information, refer to readme_threadx_generic.txt.
The following details the revision information of this port:

Initial ThreadX version 6.5.2 for TriCore TC1.6.2 using AURIX GCC tools.
