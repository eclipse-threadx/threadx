/***************************************************************************
 * Linker script for RISC-V QEMU virt regression tests.
 * Provides 128 MB RAM starting at 0x80000000 (QEMU virt default).
 * System stack is 16 KB to accommodate test framework overhead.
 **************************************************************************/

OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    . = 0x80000000;

    .text : {
        KEEP(*(.text.init))
        *(.text .text.*)
        . = ALIGN(0x1000);
        PROVIDE(etext = .);
    } > RAM

    .rodata : {
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
    } > RAM

    .data : {
        . = ALIGN(16);
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
    } > RAM

    .bss : {
        . = ALIGN(16);
        _bss_start = .;
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    .stack : {
        . = ALIGN(4096);
        _sysstack_start = .;
        . += 0x4000;
        _sysstack_end = .;
    } > RAM

    PROVIDE(_end = .);

    ASSERT(. <= ORIGIN(RAM) + LENGTH(RAM), "Image exceeds available RAM")
}
