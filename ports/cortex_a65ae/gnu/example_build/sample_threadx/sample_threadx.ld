/* Linker script to place sections and symbol values.
 * It references following symbols, which must be defined in code:
 *   start64 : Entry point
 * 
 * It defines following symbols, which code can use without definition:
 *   __cs3_peripherals
 *   __code_start
 *   __exidx_start
 *   __exidx_end
 *   __data_start
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   __stack
 *   __el3_stack
 *   __ttb0_l1
 *   __ttb0_l2_ram
 *   __ttb0_l2_private
 *   __ttb0_l2_periph
 *   __top_of_ram
 */

ENTRY(start64)

SECTIONS
{
    /*
     * CS3 Peripherals is a 64MB region from 0x1c000000
     * that includes the following:
     * System Registers             at 0x1C010000
     * UART0 (PL011)                at 0x1C090000
     * Color LCD Controller (PL111) at 0x1C1F0000
     * plus a number of others.
     * CS3_PERIPHERALS is used by the startup code for page-table generation
     * This region is not truly empty, but we have no
     * predefined objects that live within it
     */
    __cs3_peripherals  = 0x1c000000;

    /*
     * GICv3 distributor
     */
    .gicd 0x2f000000 (NOLOAD):
    {
        *(.gicd)
    }

    /*
     * GICv3 redistributors
     * 128KB for each redistributor in the system
     */
    .gicr 0x2f100000 (NOLOAD):
    {
        *(.gicr)
    }

    .vectors 0x80000000:
    {
        __code_start = .;
        KEEP(*(StartUp))
        KEEP(*(EL1VECTORS EL2VECTORS EL3VECTORS))
    }

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    }

    .text :
    {
        *(.text*)
    }

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
    }

    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

    .eh_frame :
    {
        KEEP (*(.eh_frame))
    }

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    }

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array ))
        PROVIDE_HIDDEN (__init_array_end = .);
    }

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array ))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }

    .ctors :
    {
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    }

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    }

    .jcr :
    {
        KEEP (*(.jcr))
    }

    .data :
    {
        __data_start = . ;
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    }

    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    }

    .heap (NOLOAD):
    {
        . = ALIGN(64);
        __end__ = .;
        PROVIDE(end = .);
        . = . + 0x1000;
    }

    .stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + 8 * 0x4000;
        __handler_stack = .;
    }

    .stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + 8 * 0x4000;
        __stack = .;
    }

    .el3_stack (NOLOAD):
    {
        . = ALIGN(64);
        . = . + 8 * 0x1000;
        __el3_stack = .;
    }

    .ttb0_l1 (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l1 = .;
        . = . + 0x1000;
    }

    .ttb0_l2_ram (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l2_ram = .;
        . = . + 0x1000;
    }

    .ttb0_l2_private (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l2_private = .;
        . = . + 0x1000;
    }

    .ttb0_l2_periph (NOLOAD):
    {
        . = ALIGN(4096);
        __ttb0_l2_periph = .;
        . = . + 0x1000;
    }

    /*
     * The startup code uses the end of this region to calculate
     * the top of memory - don't place any RAM regions after it
     */
    __top_of_ram = .;
}
