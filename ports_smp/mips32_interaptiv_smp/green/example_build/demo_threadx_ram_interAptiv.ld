MEMORY {
    zero_memory  : ORIGIN = 0xffff8000, LENGTH = 64K
    
    dram_rsvd1   : ORIGIN = 0x80000000, LENGTH = 0
    vector_0x000 : ORIGIN = .           LENGTH = 0x100
    vector_0x100 : ORIGIN = .           LENGTH = 0x080
    vector_0x180 : ORIGIN = .           LENGTH = 0x080
    vector_0x200 : ORIGIN = .           LENGTH = 0x080
    vector_0x280 : ORIGIN = .           LENGTH = 0x080
    vector_0x300 : ORIGIN = .           LENGTH = 0x080
    monitor      : ORIGIN = .           LENGTH = 1M - 0x380
    dram_memory  : ORIGIN = .          LENGTH = 10M - 1M
    dram_rsvd2   : ORIGIN = .,          LENGTH = 0

    flash_rsvd1  : ORIGIN = 0xbfc00000, LENGTH = 32K
    flash_memory : ORIGIN = .,          LENGTH = 10M - 32K
    flash_rsvd2  : ORIGIN = .,          LENGTH = 0
}
DEFAULTS {

    heap_reserve = 2M
    stack_reserve = 512K
    
}
//
// Program layout for running out of RAM.
//
//

SECTIONS
{
    .zdata                      ABS : > zero_memory
    .zbss                       ABS : > .
    .rozdata                            ABS : > .

    .vector_0x000   : > vector_0x000
    .vector_0x100   : > vector_0x100
    .vector_0x180   : > vector_0x180
    .vector_0x200   : > vector_0x200
    .vector_0x280   : > vector_0x280
    .vector_0x300   : > vector_0x300
    .text       : { "start.o(.text) *(.text)" } > dram_memory
    .syscall                            : > .
    .secinfo                            : > .
    .fixaddr                            : > .
    .fixtype                            : > .
    .robase                         ALIGN(8) : > .
    .rodata                         ALIGN(8) : > .

    .sdabase                        ALIGN(8) : > .
    .sdata                          ALIGN(8) : > .
_start_sbss = . ;
    .sbss                           ALIGN(8) : > .
_end_sbss = . ;
    .rosdata                        ALIGN(8) : > .
    .data                           ALIGN(8) : > .
    .profile                        ALIGN(8) : > .
_start_bss = . ;
    .bss                            ALIGN(8) : > .
_end_bss = . ;
    .heap                           ALIGN(8) PAD(heap_reserve)  : > .
    .stack                          ALIGN(8) PAD(stack_reserve) : > .
    .eventlog align(16)             pad(0x10000) :
    .free_mem align(16)             pad(0x10000) :

//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
    __ghs_ramstart  = MEMADDR(dram_rsvd1);
    __ghs_ramend    = MEMENDADDR(dram_rsvd2);
    __ghs_romstart  = MEMADDR(flash_rsvd1);
    __ghs_romend    = MEMENDADDR(flash_rsvd2);
    
}
