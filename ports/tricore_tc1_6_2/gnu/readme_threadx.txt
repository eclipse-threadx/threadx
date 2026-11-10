           Eclipse ThreadX for TriCore TC1.6.2 (Infineon AURIX TC3)

                              Using the AURIX GCC Tools


1.  Prerequisites

The port runs on CPU0 of an AURIX TC3 device and was developed on the AURIX
TC375 Lite Kit (KIT_A2G_TC375_LITE). It requires the AURIX GCC
(tricore-elf-gcc) on PATH and the Infineon iLLD, which is not part of this
repository. AURIX_ILLD_DIR shall name the directory that holds the iLLD,
Infra and Service folders. The example build was built and tested with
iLLD 1.22.0.

The iLLD headers include the board configuration Ifx_Cfg.h. The one for the
TC375 Lite Kit (DEVICE_TC37X, IFX_PIN_PACKAGE_LQFP176) is in
example_build/aurix_lite_kit_v2.


2.  Building the ThreadX Run-time Library

In example_build/aurix_lite_kit_v2, run:

   AURIX_ILLD_DIR=/path/to/Libraries ./build_threadx.sh

or, with CMake from the repository root:

   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/tricore_tc1_6_2.cmake \
         -DAURIX_ILLD_DIR=/path/to/Libraries
   cmake --build build

The CMake variable AURIX_CFG_DIR selects a different directory for
Ifx_Cfg.h. Both builds produce the library tx.a, which the application links.
tx.a does not contain _tx_initialize_low_level and the timer interrupt,
because both depend on the board; the application provides them (section 4).

inc/tx_port.h holds the definitions for the ThreadX common sources and the
application, including the settings that tx_user.h or the command line may
override. src/tx_port_tricore.h holds the definitions that only the port
sources use: the CSA layout, the initial thread context and the core
registers.


3.  Demonstration System

After tx.a is built, run in example_build/aurix_lite_kit_v2:

   AURIX_ILLD_DIR=/path/to/Libraries ./build_threadx_sample.sh

This builds sample_threadx.elf for the TC375 Lite Kit from:

  sample_threadx.c           samples/demo_threadx.c without main() and with
                             the event trace start (section 8)
  tx_initialize_low_level.c  low-level initialisation and timer interrupt
  Cpu0_Main.c                CPU0 entry, calls tx_kernel_enter (Infineon)
  Cpu1_Main.c, Cpu2_Main.c   CPU1 and CPU2 entry, idle (Infineon)
  Ifx_Cfg_Trap.h             iLLD trap hook for the ThreadX system call
  Ifx_Cfg.h                  clock configuration (Infineon)
  Ifx_Cfg_Ssw.c/.h           iLLD start-up configuration (Infineon)
  Ifx_Cfg_SswBmhd.c          boot mode headers (Infineon)
  Lcf_Gnuc_Tricore_Tc.lsl    linker script (Infineon, with __TX_FREE_MEM)

and the iLLD start-up code and drivers listed in build_threadx_sample.sh.

The files marked (Infineon) are Copyright (C) Infineon Technologies AG and
are licensed under the Boost Software License 1.0, as stated in their
headers, not under the MIT License of ThreadX. They are adapted for the
ThreadX demonstration. The iLLD itself is not part of this repository and
is subject to its own Infineon license.


4.  System Initialization

The iLLD start-up code initialises each CPU and calls core0_main to
core2_main. The linker script places the code and data of Cpu<n>_Main.c in
the memories of CPU<n>. core0_main disables the CPU0 and safety watchdogs,
waits until all CPUs have started and calls tx_kernel_enter. Interrupts stay
disabled from reset until _tx_thread_schedule starts the first thread.

_tx_initialize_low_level:

  - sets the first unused memory to __TX_FREE_MEM, which the linker script
    defines after the heap in the data RAM of CPU0; the memory up to
    __USTACK0_END is free for tx_application_define,
  - sets up the ThreadX timer (section 7),
  - sets up the dispatch interrupt (section 5).


5.  Thread Scheduling

ThreadX enters the scheduler through syscall 0. The iLLD system call trap
calls the hook IFX_CFG_CPU_TRAP_SYSCALL_CPU0_HOOK in Ifx_Cfg_Trap.h, which
shall call _tx_thread_syscall_handler directly.

An interrupt or trap handler that makes another thread ready requests the
dispatch interrupt _tx_thread_dispatch_isr, a software request on
TX_PORT_TRICORE_DISPATCH_SRC (SRC_GPSR_GPSR0_SR0 by default) at
TX_PORT_TRICORE_DISPATCH_PRIORITY (1 by default). It switches threads once no
interrupt of a higher priority is active.


6.  Thread Context

A thread's context is its CSA chain. tx_thread_csa (TX_THREAD_EXTENSION_0)
holds the link to the chain of a suspended thread:

  lower context      saved by SVLCX in the iLLD interrupt or trap entry;
                     A11 holds the resume address
  upper context      saved on entry to the system call or the dispatch
                     interrupt; A10 and PSW of the thread
  call contexts      one upper context for each call level of the thread

The context save walks TX_PORT_TRICORE_SYSCALL_CALL_DEPTH or
TX_PORT_TRICORE_DISPATCH_CALL_DEPTH links back from the handler
(src/tx_port_tricore.h). It copies A10 of the upper context to
tx_thread_stack_ptr for ThreadX stack checking. The thread stack holds only
local data.

_tx_thread_stack_build gives a new thread two CSAs, all other words 0:

  lower context      A11 = entry function, PCXI.PIE = 1, PCXI.UL = 1 and
                     the link to the upper context
  upper context      A10 = stack end, aligned to 8 bytes; PSW.IO = 2,
                     PRS 0, call depth counting disabled; no link

The scheduler restores both like a suspended thread, so the thread starts at
its entry function with interrupts enabled.

The FPU state is part of the CSAs, so threads use the FPU without setup.

CSA budget: a suspended thread holds one CSA per call level plus two; each
active interrupt or trap holds two plus one per call level. ThreadX stack
checking does not cover the CSAs. The CSA area of CPU0 (LCF_CSA0_SIZE) shall
hold the chains of all threads plus the deepest interrupt nesting, up to LCX
as set by Ifx_Ssw_initCSA. If the CSAs for a new thread would reach LCX,
tx_thread_create or tx_thread_reset raises FCD.


7.  Timer and Interrupt Handling

The ThreadX timer uses STM0 compare 0. _tx_initialize_low_level derives the
period from IfxStm_getFrequency and TX_TIMER_TICKS_PER_SECOND (1000 by
default). TimerInterruptHandler (IFX_INTERRUPT, priority
TX_PORT_TRICORE_TIMER_INTERRUPT_PRIORITY, 2 by default) clears the compare
flag, sets the next compare one period after the previous one and calls
_tx_timer_interrupt. If STM has already passed that value, for example after
a debugger halt, the next compare is one period from now and the ticks in
between are lost.

TX_DISABLE clears ICR.IE by default. With TX_PORT_USE_CCPN defined, it
raises ICR.CCPN to TX_PORT_CCPN (31 by default) instead and never lowers it;
interrupts above TX_PORT_CCPN stay enabled in ThreadX critical sections and
shall not call ThreadX services. TX_RESTORE restores the previous ICR.

An ISR that calls ThreadX services is a plain IFX_INTERRUPT function on CPU0
with a priority above TX_PORT_TRICORE_DISPATCH_PRIORITY and, with
TX_PORT_USE_CCPN, not above TX_PORT_CCPN. _tx_thread_context_save and
_tx_thread_context_restore have no effect on this port.

Interrupts and traps taken from a thread switch to the CPU0 interrupt stack
(LCF_ISTACK0_SIZE, 1 KiB). It holds the local data of all nested handlers,
including the scheduler's idle wait, so thread stacks need no interrupt
reserve.

IFX_USE_SW_MANAGED_INT shall not be defined: IFX_INTERRUPT then creates no
vector table entry, and the iLLD dispatcher adds a call level that
TX_PORT_TRICORE_DISPATCH_CALL_DEPTH does not count.


8.  Event Trace

With TX_ENABLE_EVENT_TRACE, the application calls tx_trace_enable with its
trace buffer, at the earliest at the start of tx_application_define, because
kernel initialisation resets the trace. The demonstration does so with a
4000-byte buffer and 30 registry entries. The time stamp is STM0_TIM0
(TX_TRACE_TIME_SOURCE in tx_port.h).


9.  Execution Profile and Low Power

For TX_EXECUTION_PROFILE_ENABLE and TX_LOW_POWER the port implements only the
callouts. The regression configurations execution_profile_build and
low_power_build (test/tx/cmake/tricore_tc1_6_2) show only that the callouts
do not disturb the kernel: no test reads the measured times, and none of
TX_LOW_POWER_TIMER_SETUP, TX_LOW_POWER_TICKLESS and TX_LOW_POWER_USER_* is
defined.


10. Improving Performance

The build scripts and cmake/tricore_tc1_6_2.cmake compile with -O0 -g3 for
debugging. The regression test also passes with -O2. TX_DISABLE_ERROR_CHECKING
removes the ThreadX API error checking.


11. Revision History

For generic code revision information, refer to readme_threadx_generic.txt.
The following details the revision information of this port:

Initial ThreadX version 6.5.2 for TriCore TC1.6.2 using AURIX GCC tools.
