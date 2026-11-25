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

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x80000000;

    .text : {
        KEEP(*(.text.init))
        *(.text .text.*)
        . = ALIGN(0x1000);
        PROVIDE(etext = .);
    } > RAM :text

    .rodata : {
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
    } > RAM :text

    .data : {
        . = ALIGN(0x1000);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
    } > RAM :data

    .bss (NOLOAD) : {
        . = ALIGN(16);
        _bss_start = .;
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    } > RAM :data

    .stack (NOLOAD) : {
        . = ALIGN(4096);
        _sysstack_start = .;
        . += 0x4000;
        _sysstack_end = .;
    } > RAM :data

    /* Fixed 64 KiB newlib sbrk heap window.  _end stays after it, so the
       ThreadX first-unused-memory pointer starts past the heap. */
    .heap (NOLOAD) : {
        . = ALIGN(16);
        _heap_start = .;
        . += 0x10000;
        _heap_end = .;
    } > RAM :data

    PROVIDE(_end = .);

    ASSERT(. <= ORIGIN(RAM) + LENGTH(RAM), "Image exceeds available RAM")
}
