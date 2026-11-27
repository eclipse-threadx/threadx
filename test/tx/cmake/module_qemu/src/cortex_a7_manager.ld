/* Copyright (c) 2026 Eclipse ThreadX contributors
 * SPDX-License-Identifier: MIT
 */

/* QEMU loads the manager ELF directly into virt RAM. The shared module page
   starts just beyond this region at 0x40200000.  */
MEMORY
{
    RAM (rwx) : ORIGIN = 0x40010000, LENGTH = 0x001f0000
}

ENTRY(reset_handler)

SECTIONS
{
    .vectors : { KEEP(*(.vectors)) } > RAM
    .text : { *(.text .text.*) *(.rodata .rodata.*) } > RAM
    .ARM.exidx : { *(.ARM.exidx*) } > RAM
    .data : { *(.data .data.*) } > RAM

    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start__ = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .stack (NOLOAD) : ALIGN(8)
    {
        . = . + 4096;
        __stack_end__ = .;
        . = . + 2048;
        __irq_stack_end__ = .;
    } > RAM

    __RAM_segment_used_end__ = .;
}
