MEMORY {
    flash_rsvd1  : ORIGIN = 0x10000000, LENGTH = 0
    flash_memory : ORIGIN = .,          LENGTH = 128M
    flash_rsvd2  : ORIGIN = .,          LENGTH = 0

    dram_rsvd1   : ORIGIN = 0x20000000, LENGTH = 0
    dram_memory  : ORIGIN = .,	 	LENGTH = 256M
    dram_rsvd2   : ORIGIN = ., 	        LENGTH = 0
}
DEFAULTS {

    heap_reserve = 8M
    stack_reserve = 1M

}
//
// Program layout for starting in ROM, copying data to RAM,
// and continuing to execute out of ROM.
//

SECTIONS
{

//
// ROM SECTIONS
//

    .text 					        : > flash_memory
    .syscall					        : > .
    .rodata 					        : > .
    .secinfo 					        : > .
    .fixaddr 					        : > .
    .fixtype 					        : > .
    
    .CROM.data 	                            CROM(.data) : > .

//
// RAM SECTIONS
//

    .data 					        : > dram_memory	
    .bss 					        : > .
    .ghcovfz					  CLEAR : > .
    .ghcovcz					  CLEAR : > .
    .ghcovdz					  CLEAR : > .
    .heap                   ALIGN(16) PAD(heap_reserve) : > .
    .stack                 ALIGN(16) PAD(stack_reserve) : > .


//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
    __ghs_romstart  = MEMADDR(flash_rsvd1);
    __ghs_romend    = MEMENDADDR(flash_rsvd2);
    __ghs_ramstart  = MEMADDR(dram_rsvd1);
    __ghs_ramend    = MEMENDADDR(dram_rsvd2);

//
// These special symbols mark the bounds of RAM and ROM images of boot code.
// They are used by the GHS startup code (_start and __ghs_ind_crt0).
//
    __ghs_rombootcodestart  = ADDR(.text);
    __ghs_rombootcodeend    = ENDADDR(.fixtype);
    __ghs_rambootcodestart  = 0;
    __ghs_rambootcodeend    = 0;

}

