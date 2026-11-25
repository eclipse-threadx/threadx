                    Eclipse Foundation's RTOS, ThreadX for RISC-V32

                                  Using the Clang Tools


1. Building the ThreadX run-time Library

Prerequisites
- Install a RISC-V32 bare-metal Clang toolchain
- Install a RISC-V32 bare-metal GNU toolchain with riscv32-unknown-elf prefix.
  Common source: https://github.com/riscv-collab/riscv-gnu-toolchain

The GNU toolchain is needed because the Clang toolchain does not include some
standard headers and libraries, i.e. "string.h".

Verify the Clang toolchain:
  clang --version

Verify the GCC toolchain:
  riscv32-unknown-elf-gcc --version
  riscv32-unknown-elf-objdump --version

CMake-based build (recommended)

From the ThreadX top-level directory:

  Set environment variable "GCC_INSTALL_PREFIX" with the location of the
  GNU toolchain, i.e., export GCC_INSTALL_PREFIX=/opt/riscv_rv32ima

  cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32_clang.cmake .
  cmake --build ./build/

This uses cmake/riscv32_clang.cmake and ports/risc-v32/clang/CMakeLists.txt to
configure the cross-compiler flags and produce the ThreadX run-time library
and example binaries.

Example build script

The example demonstration contains a build script. See:

  ports/risc-v32/clang/example_build/qemu_virt/build_libthreadx.sh

This script builds the library and the demo application kernel.elf.


2. Demonstration System (QEMU)

The provided example is targeted at QEMU's virt platform. After building the
example, the produced kernel.elf can be executed in QEMU:

  qemu-system-riscv32 -nographic -smp 1 -bios none -m 128M -machine virt -kernel kernel.elf

Typical QEMU features used:
- Single-core CPU
- UART serial console
- PLIC (Platform-Level Interrupt Controller)
- CLINT (Core-Local Interruptor) for timer


3. System Initialization

Entry Point

The example startup code begins at the _start label in entry.s. This startup
code performs hardware initialization including:
- Check hart ID (only hart 0 continues; others enter WFI loop)
- Zero general-purpose registers
- Set up initial stack pointer
- Clear BSS section
- Jump to main()

Low-Level Port Initialization (tx_initialize_low_level.S)

The _tx_initialize_low_level function:
- Saves the system stack pointer to _tx_thread_system_stack_ptr
- Records first free RAM address from __tx_free_memory_start symbol
- Initializes floating-point control/status register (FCSR) if floating point enabled

Board Initialization (board.c)

After tx_initialize_low_level returns, main() calls board_init() to:
- Initialize PLIC (Platform-Level Interrupt Controller)
- Initialize UART
- Initialize hardware timer (CLINT)
- Set trap vector (mtvec) to point to trap handler


4. Register Usage and Stack Frames

The RISC-V32 ABI defines t0-t6 and a0-a7 as caller-saved (scratch) registers.
All other registers used by a function must be preserved by the function.

ThreadX takes advantage of this: when a context switch happens during a
function call, only the non-scratch registers need to be saved.

Stack Frame Types

Two types of stack frames exist:

A. Interrupt Frame (stack type = 1)
   Created when an interrupt occurs during thread execution.
   Saves all registers including caller-saved registers.
   Size: 65*4 = 260 bytes (with FP), or 32*4 = 128 bytes (without FP)

B. Solicited Frame (stack type = 0)
   Created when a thread voluntarily yields via ThreadX service calls.
   Saves only callee-saved registers (s0-s11) and mstatus.
   Size: 29*4 = 116 bytes (with FP), or 16*4 = 64 bytes (without FP)


Stack Layout for Interrupt Frame (with FP enabled):

   Index    Offset    Register    Description
   ─────────────────────────────────────────────────
     0       0x00        --       Stack type (1 = interrupt)
     1       0x04       s11       Preserved register
     2       0x08       s10       Preserved register
     3       0x0C        s9       Preserved register
     4       0x10        s8       Preserved register
     5       0x14        s7       Preserved register
     6       0x18        s6       Preserved register
     7       0x1C        s5       Preserved register
     8       0x20        s4       Preserved register
     9       0x24        s3       Preserved register
    10       0x28        s2       Preserved register
    11       0x2C        s1       Preserved register
    12       0x30        s0       Preserved register
    13       0x34        t6       Scratch register
    14       0x38        t5       Scratch register
    15       0x3C        t4       Scratch register
    16       0x40        t3       Scratch register
    17       0x44        t2       Scratch register
    18       0x48        t1       Scratch register
    19       0x4C        t0       Scratch register
    20       0x50        a7       Argument register
    21       0x54        a6       Argument register
    22       0x58        a5       Argument register
    23       0x5C        a4       Argument register
    24       0x60        a3       Argument register
    25       0x64        a2       Argument register
    26       0x68        a1       Argument register
    27       0x6C        a0       Argument register
    28       0x70        ra       Return address
    29       0x74       --        Reserved
    30       0x78      mepc       Machine exception PC
    31-46   0x7C-0xB8   fs0-fs7   Preserved FP registers*
    47-62   0xBC-0xF8   ft0-ft11  Scratch FP registers*
    63       0xFC       fcsr      FP control/status register
   ─────────────────────────────────────────────────
   *Note: In ilp32d ABI, FP registers are 8 bytes each, but current
    port implementation uses 4-byte indexing which may cause
    overlap if fsd/fld are used.


5. Interrupt Handling

Machine Mode Operation

ThreadX operates in machine mode (M-mode), the highest privilege level.
All interrupts and exceptions trap to machine mode.

Interrupt Sources

1. Machine Timer Interrupt (MTI):
   - Triggered by CLINT when mtime >= mtimecmp
   - Handled by _tx_timer_interrupt (src/tx_timer_interrupt.S)
   - Called from trap handler in trap.c

2. External Interrupts (MEI):
   - Routed through PLIC
   - Handler in trap.c calls registered ISR callbacks

3. Software Interrupts (MSI):
   - Supported but not actively used in this port

Interrupt Flow

1. Hardware trap entry (automatic):
   - mepc <- PC (address of interrupted instruction)
   - mcause <- exception/interrupt code
   - mstatus.MPIE <- mstatus.MIE (save interrupt-enable state)
   - mstatus.MIE <- 0 (disable interrupts)
   - mstatus.MPP <- Machine mode
   - PC <- mtvec (points to trap_entry in entry.s)

2. Trap entry (entry.s):
   - Allocates interrupt stack frame (32*4 or 65*4 bytes depending on FP)
   - Saves RA (x1) on stack
   - Calls _tx_thread_context_save

3. Context save (_tx_thread_context_save.S):
   - Increments _tx_thread_system_state (nested interrupt counter)
   - If nested interrupt: saves remaining registers and returns to ISR
   - If first interrupt: saves full context, switches to system stack

4. Trap handler (trap.c):
   - Examines mcause to determine interrupt type
   - Dispatches to appropriate handler (_tx_timer_interrupt or PLIC handler)
   - Returns to context restore

5. Context restore (_tx_thread_context_restore.S):
   - Decrements _tx_thread_system_state
   - Checks if preemption needed
   - Restores thread context or switches to next ready thread via scheduler
   - Returns to interrupted thread or executes new thread


Interrupt Control Macros

TX_DISABLE and TX_RESTORE macros atomically manage the MIE bit in mstatus:

  TX_DISABLE:  Saves and clears MIE bit via csrrci (CSR read-clear immediate)
  TX_RESTORE:  Restores only MIE bit via csrrs (CSR read-set)
               Other mstatus bits remain unchanged

These are defined in ports/risc-v32/gnu/inc/tx_port.h and use the
_tx_thread_interrupt_control() function.


6. Thread Scheduling and Context Switching

Thread Scheduler (src/tx_thread_schedule.S)

The scheduler:
1. Enables interrupts while waiting for next thread
2. Spins until _tx_thread_execute_ptr becomes non-NULL
3. Disables interrupts (critical section)
4. Sets _tx_thread_current_ptr = _tx_thread_execute_ptr
5. Increments thread's run count
6. Switches to thread's stack
7. Determines stack frame type and restores context:
   - Interrupt frame: full context restored, returns via mret
   - Solicited frame: minimal context restored, returns via ret

Initial Thread Stack Frame (src/tx_thread_stack_build.S)

New threads start with a fake interrupt frame containing:
- All registers initialized to 0
- ra (x1) = 0
- mepc = entry function pointer
- Stack type = 1 (interrupt frame)
- Floating-point registers initialized based on ABI


7. Port Configuration and Macros

Default Configurations (in ports/risc-v32/gnu/inc/tx_port.h):

  TX_MINIMUM_STACK                1024    /* Minimum thread stack size */
  TX_TIMER_THREAD_STACK_SIZE      1024    /* Timer thread stack size */
  TX_TIMER_THREAD_PRIORITY        0       /* Timer thread priority */
  TX_MAX_PRIORITIES               32      /* Must be multiple of 32 */

These can be overridden in tx_user.h or on the compiler command line.


8. Build Configuration

CMake Toolchain File: cmake/riscv32_gnu.cmake

Compiler Flags:
  -march=rv32gc          RV32 with IMAFD+C extensions
  -mabi=ilp32d           32-bit integers/pointers, double-precision FP in registers
  -mcmodel=medany        ±2GB addressability
  -D__ASSEMBLER__        For assembly files

ABI Selection

The port uses ilp32d ABI which includes:
- 32-bit integers and pointers
- Double-precision floating-point arguments in registers
- Floating-point registers f0-f31

When building with floating-point ABI:
- FP registers and FCSR are saved/restored in context switches
- Stack frames expand from 32*REGBYTES to 65*REGBYTES
- Conditional compilation uses __riscv_float_abi_double / __riscv_float_abi_single


9. File Organization

Port-specific files (ports/risc-v32/gnu/):

Core assembly files (src/):
  - tx_initialize_low_level.S     Initial setup and system state
  - tx_thread_context_save.S      Save context on interrupt entry
  - tx_thread_context_restore.S   Restore context on interrupt exit
  - tx_thread_schedule.S          Thread scheduler
  - tx_thread_system_return.S     Solicited context save for voluntary yield
  - tx_thread_stack_build.S       Build initial stack frame for new thread
  - tx_thread_interrupt_control.S Interrupt enable/disable control
  - tx_timer_interrupt.S          Timer interrupt handler

Header file (inc/):
  - tx_port.h                     Port-specific defines and macros

Example files (example_build/qemu_virt/):
  - entry.s                       Startup code, trap entry point
  - board.c, uart.c, hwtimer.c   Platform-specific initialization
  - plic.c                        PLIC interrupt controller driver
  - trap.c                        Trap/exception dispatcher
  - link.lds                      Linker script for QEMU virt
  - build_libthreadx.sh           Build script


10. Linker Script Requirements

The linker script must provide:

1. Entry point:
   ENTRY(_start)

2. Memory layout:
   - .text section (code)
   - .rodata section (read-only data)
   - .data section (initialized data)
   - .bss section (uninitialized data)

3. Symbols:
   - _end: First free memory address (used by ThreadX allocation)
   - _bss_start, _bss_end: For zero initialization
   - Initial stack space (example: 4KB)

4. Alignment:
   - 16-byte alignment throughout (RISC-V requirement)

Example from QEMU virt build:

  SECTIONS
  {
    . = 0x80000000;  /* QEMU virt base address */

    .text : { *(.text .text.*) }
    .rodata : { *(.rodata .rodata.*) }
    .data : { *(.data .data.*) }
    .bss : { *(.bss .bss.*) }

    .stack : {
      . = ALIGN(4096);
      _sysstack_start = .;
      . += 0x1000;  /* 4KB initial stack */
      _sysstack_end = .;
    }

    PROVIDE(_end = .);
  }


11. Floating-Point Support

When building with ilp32d ABI and FP enabled:

- FP registers f0-f31 and FCSR are saved/restored during context switches
- Stack frames increase from 32*REGBYTES to 65*REGBYTES (128 to 260 bytes)
- MSTATUS.FS (floating-point state) field is set to indicate dirty FP state

Stack frame differences:
- Without FP: 32*4 = 128 bytes (interrupt), 16*4 = 64 bytes (solicited)
- With FP: 65*4 = 260 bytes (interrupt), 29*4 = 116 bytes (solicited)


12. Performance and Debugging

Performance Optimization

Build optimizations:
- Use -O2 or -O3 for production (example uses -O0 for debugging)
- Enable -Wl,--gc-sections to remove unused code
- Define TX_DISABLE_ERROR_CHECKING to remove parameter checks
- Consider -flto for link-time optimization

Debugging with QEMU and GDB

Start QEMU in debug mode:
  qemu-system-riscv32 -nographic -smp 1 -bios none -m 128M \
    -machine virt -kernel kernel.elf -s -S

  -s: Enable GDB server on TCP port 1234
  -S: Pause at startup waiting for GDB

Connect GDB:
  riscv32-unknown-elf-gdb kernel.elf
  (gdb) target remote :1234
  (gdb) break main
  (gdb) continue

Useful GDB commands:
  (gdb) info registers          # View general registers
  (gdb) info all-registers      # Include CSR and FP registers
  (gdb) p/x $mstatus           # View machine status register
  (gdb) x/32xw $sp             # Examine stack memory
  (gdb) p *_tx_thread_current_ptr  # View current thread control block


13. Platform-Specific Notes (QEMU virt)

PLIC Configuration

The PLIC (Platform-Level Interrupt Controller) is memory-mapped at 0x0C000000:

- Enables up to 1024 interrupt sources
- Supports priority levels 0-7 (0 = disabled)
- Requires per-hart priority threshold and enable register configuration

Example PLIC usage (from plic.c):
  plic_irq_enable(irq_number);        # Enable specific interrupt
  plic_prio_set(irq_number, priority);# Set priority level

CLINT Configuration

The CLINT (Core-Local Interruptor) is memory-mapped at 0x02000000:

- CLINT_MSIP(hartid): 0x0000 + 4*hartid (software interrupt)
- CLINT_MTIMECMP(hartid): 0x4000 + 8*hartid (timer compare)
- CLINT_MTIME: 0xBFF8 (timer value, read-only)

Timer frequency is platform-dependent (example uses 10MHz).

Multi-Core Considerations

The current port is single-core focused:
- Only hart 0 continues from reset; others enter WFI loop
- _tx_thread_system_state is a global variable
- No per-hart data structures


14. Revision History

For generic code revision information, refer to readme_threadx_generic.txt.

The following details the revision history for this RISC-V32 GNU port:

01-26-2026  Akif Ejaz              Brief rewrite with accurate
                                   technical details matching implementation,
                                   register naming per RISC-V ABI, and
                                   complete interrupt flow documentation
                                   (Adapted from RISC-V64 port)


Copyright (c) 1996-2026 Microsoft Corporation

https://azure.com/rtos
