OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    L2 (rwx) : ORIGIN = 0x1c000000, LENGTH = 0x80000
}

SECTIONS
{
    PROVIDE(__boot_address = 0x1c000880);
    __stack_size = DEFINED(__stack_size) ? __stack_size : 0x2000;
    PROVIDE(__stack_size = __stack_size);

    .vectors 0x1c000800 :
    {
        __vector_start = .;
        KEEP(*(.vectors))
        . = __vector_start + 0x80;
        __vector_end = .;
    } > L2

    .text __boot_address :
    {
        *(.text.start)
        *(.text .text.*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
    } > L2

    .data :
    {
        __data_begin = .;
        __sdata_begin = .;
        *(.sdata .sdata.*)
        *(.data .data.*)
        . = ALIGN(4);
        __data_end = .;
    } > L2

    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
        _end = .;
    } > L2

    PROVIDE(__tx_free_memory_start = __bss_end);
    PROVIDE(__global_pointer$ = MIN(__sdata_begin + 0x800,
                                    MAX(__data_begin + 0x800, __bss_end - 0x800)));

    __stack_top = ORIGIN(L2) + LENGTH(L2);

    ASSERT(SIZEOF(.vectors) <= 128, "Vector table exceeds reserved 128 bytes")

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.note.*)
    }
}
