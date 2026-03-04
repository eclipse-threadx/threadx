                    Eclipse Foundation's RTOS, ThreadX for XuanTie E906

                                  Using the GNU Tools

The XuanTie E906 is a fully synthesizable, middle-end, microcontroller-class processor that is compatible to the RISC-V RV32IMA[F][D]C[P] ISA. It
delivers considerable integer and enhanced, energy-efficient floating-point compute performance especially the double precision.

1. Building the ThreadX run-time Library

Prerequisites
- Install a XuanTie bare-metal GNU toolchain with riscv64-unknown-elf prefix
- Download URL: https://www.xrvm.cn/community/download?versionId=4460156621967921152
- Toolchain archive name: XuanTie-900-gcc-elf-newlib-x86_64-V3.2.0-20250627.tar.gz

Verify the toolchain:
  riscv64-unknown-elf-gcc --version
  riscv64-unknown-elf-objdump --version

Library build script
  build_libthreadx.sh

Example build script

The example demonstration contains a build script. See:

  build_threadx_sample.sh

This script builds the library and the demo application demo_threadx.elf.


2. Demonstration System (QEMU)

Prerequisites
- Install a XuanTie QEMU
- Download URL: https://www.xrvm.cn/community/download?versionId=4468398114511851520
- QEMU archive name: XuanTie-qemu-x86_64-Ubuntu-20.04-V5.2.8-B20250721-0303.tar.gz

The provided example is targeted at XuanTie QEMU's smartl platform. After building the
example, the produced demo_threadx.elf can be executed in QEMU:

  qemu-system-riscv32 -nographic -machine smartl -cpu e906fdp -kernel demo_threadx.elf

Typical QEMU features used:
- Single-core CPU
- UART serial console
- CLIC (Core-Local Interrupt Controller)

3. System Initialization

Entry Point

The example startup code begins at the Reset_Handler label in startup.S. This startup
code performs hardware initialization including:
- Initialize gp register
- Set up the entry for interrupt and exception handler
- Set up initial stack pointer
- Jump to SystemInit() for initialize CLIC、Clear BSS、Cache、System Clock、System Tick
- Jump to pre_main()

In pre_main(), the following initialization is performed:
Board Initialization (board_init.c)
- Initialize UART

Then jump to main().

4. Interrupt Handling

Machine Mode Operation

ThreadX operates in machine mode (M-mode), the highest privilege level.
All interrupts and exceptions trap to machine mode.

Interrupt Sources

1. Machine Timer Interrupt (MTI):
   - Triggered by CLINT when mtime >= mtimecmp
   - Handled by _tx_timer_interrupt (src/tx_timer_interrupt.c)
   - Called from tick_irq_handler() in example_build/xuantie_smartl_fpga/components/chip_riscv_dummy/src/sys/tick.c

2. External Interrupts (MEI):
   - Routed through CLIC
   - Handled by do_irq() in example_build/xuantie_smartl_fpga/components/chip_riscv_dummy/src/sys/irq.c

3. Software Interrupts (MSI):
   - Routed through CLIC
   - Handled by tspend_handler() in src/tx_thread_context.S

Interrupt Control Macros

TX_DISABLE and TX_RESTORE macros atomically manage the MIE bit in mstatus:

  TX_DISABLE:  Saves and clears MIE bit via csrrci (CSR read-clear immediate)
  TX_RESTORE:  Restores only MIE bit via csrrs (CSR read-set)
               Other mstatus bits remain unchanged

These are defined in ports/xuantie/e906/gnu/inc/tx_port.h and use the
_tx_thread_interrupt_control() function.


5. Thread Scheduling and Context Switching

Fist Thread Switch (src/tx_thread_schedule.S)
1. Enables interrupts while waiting a thread
2. Spins until _tx_thread_execute_ptr becomes non-NULL
3. Disables interrupts (critical section)
4. Sets _tx_thread_current_ptr = _tx_thread_execute_ptr
5. Increments thread's run count
6. Switches to thread's stack
7. Restores the thread's context, then returns via mret

Thread Scheduler (src/tx_thread_system_return.S, src/tx_thread_context.S)
1. Set a software interrupt to trigger context switch
2. Come to software interrupt handler (tspend_handler)
3. Save previous thread's context
4. Check and waiting for _tx_thread_execute_ptr != NULL
5. Switch thread sp to _tx_thread_execute_ptr
6. Restores the thread's context, then returns via mret

Initial Thread Stack Frame (src/tx_thread_stack_build.S)

New threads start with a fake interrupt frame containing:
- All registers initialized to 0
- ra (x1) = _tx_thread_exit
- mepc = entry function pointer
- mstatus = mstatus.FS=1 | mstatus.MPP=3 | mstatus.MPIE=1
- Floating-point registers initialized based on ABI


6. Port Configuration and Macros

Default Configurations (in ports/risc-v64/gnu/inc/tx_port.h):

  TX_MINIMUM_STACK                1024    /* Minimum thread stack size */
  TX_TIMER_THREAD_STACK_SIZE      1024    /* Timer thread stack size */
  TX_TIMER_THREAD_PRIORITY        0       /* Timer thread priority */
  TX_MAX_PRIORITIES               32      /* Must be multiple of 32 */

These can be overridden in tx_user.h or on the compiler command line.


7. Build Configuration

CMake Toolchain File: example_build/xuantie_smartl_fpga/xuantie_e906_gnu.cmake

Compiler Flags:
  -mcpu=e906fdp          RISC-V RV32IMA[F][D]C[P]
  -mcmodel=medlow        ±2GB addressability
  -D__ASSEMBLER__        For assembly files


8. File Organization

Example files (example_build/xuantie_smartl_fpga/):
  - components/chip_riscv_dummy/src/arch/e906fdp        Startup code, Interrupt handlers
  - components/chip_riscv_dummy/src/drivers             The basic peripheral drivers for the platform
  - components/chip_riscv_dummy/src/sys                 System initialization
  - components/chip_riscv_dummy/gcc_flash_smartl.ld     Linker script for QEMU smartl
  - components/csi                                      CPU core and peripheral API
  - components/libc_threadx                             Minimum printf and malloc implementation
  - boards/board_riscv_dummy/src                        Bsp initialization
  - pre_main.c                                          Call main function
  - tx_user.h                                           ThreadX user defines
  - build_libthreadx.sh                                 Build script


9. Linker Script Requirements

The linker script must provide:

1. Entry point:
   ENTRY(Reset_Handler)

2. Memory layout:
   - .text section (code)
   - .rodata section (read-only data)
   - .data section (initialized data)
   - .bss section (uninitialized data)

3. Symbols:
   - __data_start__, __data_end__: For data copy from Flash when executed in DRAM
   - __bss_start__, __bss_end__: For zero initialization
   - __heap_start, __heap_end: For ThreadX allocation memory

4. Alignment:
   - 4-byte alignment throughout

10. Performance and Debugging

Performance Optimization

Build optimizations:
- Use -O2 or -O3 for production (example uses -O0 for debugging)
- Enable -Wl,--gc-sections to remove unused code
- Define TX_DISABLE_ERROR_CHECKING to remove parameter checks
- Consider -flto for link-time optimization

Debugging with QEMU and GDB

Start QEMU in debug mode:
  qemu-system-riscv32 -nographic -machine smartl -cpu e906fdp -kernel demo_threadx.elf -s -S

  -s: Enable GDB server on TCP port 1234
  -S: Pause at startup waiting for GDB

Connect GDB:
  riscv64-unknown-elf-gdb demo_threadx.elf
  (gdb) target remote :1234
  (gdb) break main
  (gdb) continue

Useful GDB commands:
  (gdb) info registers              # View general registers
  (gdb) info all-registers          # Include CSR and FP registers
  (gdb) p/x $mstatus                # View machine status register
  (gdb) x/32gx $sp                  # Examine stack memory
  (gdb) p *_tx_thread_current_ptr   # View current thread control block


11. Platform-Specific Notes (QEMU smartl)
See https://www.xrvm.com/soft-tools/tools/QEMU

Timer frequency is platform-dependent (example uses 100MHz).


12. Revision History

For generic code revision information, refer to ports/risc-v32/gnu/example_build/xuantie_smartl_fpga/readme_e906.txt.

The following details the revision history for this xuantie/e906 GNU port:

12-02-2026  Steven Lin             Support XuanTie E906


Copyright (c) 1996-2026 Microsoft Corporation

https://azure.com/rtos
