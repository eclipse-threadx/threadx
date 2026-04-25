/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the Microsoft Software License Terms for Microsoft
 * Azure RTOS. Full text of the license can be found in the LICENSE
 * file at https://aka.ms/AzureRTOS_EULA.
 *
 * ThreadX RISC-V32 GNU port: linker script for the QEMU `virt` machine.
 *
 *   - Image base 0x80000000 matches the PC QEMU sets when launched with
 *     `-machine virt -kernel kernel.elf` (DRAM start on this machine).
 *   - .init is placed first so entry.s / _start is the very first
 *     instruction the CPU executes after reset.
 *   - __global_pointer$ is provided at the start of .data + 0x800 so
 *     that GP-relative loads/stores reach symbols in [GP-0x800, GP+0x7FF],
 *     the standard small-data window that `-mrelax` uses (see entry.s
 *     for the matching `la gp, __global_pointer$` initialization).
 *   - A 4 KiB system stack is reserved after .bss, with _sysstack_start
 *     consumed by entry.s as the boot stack base.
 */

OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  /* QEMU virt: -kernel jumps to 0x80000000 (DRAM start). */
  . = 0x80000000;

  .init : {
    KEEP (*(.init))
  }

  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    /* Centre __global_pointer$ in the small-data window so the +/-2 KiB
       reach of GP-relative addressing covers the .sdata/.sbss area. */
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    _bss_start = .;        /* consumed by entry.s for .bss zero-init */
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
    _bss_end = .;
  }

  .stack : {
    . = ALIGN(4096);
    _sysstack_start = .;   /* boot stack base used by entry.s */
    . += 0x1000;           /* 4 KiB system stack */
    _sysstack_end = .;
  }

  PROVIDE(_end = .);
}
