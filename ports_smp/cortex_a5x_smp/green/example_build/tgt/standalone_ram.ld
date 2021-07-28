//
// memory map for the Xilinx ZCU102 with 2GB or SDRAM
//
MEMORY {
    dram_rsvd1   : ORIGIN = 0x0000000000, LENGTH = 0
    dram_memory  : ORIGIN = .,	 	        LENGTH = 2048M
    dram_rsvd2   : ORIGIN = ., 	          LENGTH = 0
}
DEFAULTS {

    heap_reserve = 8M
    stack_reserve = 16K
    stack_1_3_reserve = 16K

}
//
// Program layout for running out of RAM.
//
//

SECTIONS
{

//
// The text segment
//
    .vectors                                            : > dram_memory
    .boot                                               : > .
    .text                                               : > .
    .syscall                                            : > .
    .fixaddr                                            : > .
    .fixtype                                            : > .
    .rodata                                             : > .
    .secinfo                                            : > .

//
// The data segment
//
    .data                                               : > .
    .bss                                                : > .
    .ghcovfz					  CLEAR : > .
    .ghcovcz					  CLEAR : > .
    .ghcovdz					  CLEAR : > .

    .mmu_tbl0 ALIGN(4096) : > .
    .mmu_tbl1 ALIGN(4096) : > .
    .mmu_tbl2 ALIGN(4096) : > .

    .stack                 ALIGN(16) PAD(stack_reserve) : > .
    __el3_stack = .;
    .stack_1               ALIGN(16) PAD(stack_1_3_reserve) : > .
    __el3_1_stack = .;
    .stack_2               ALIGN(16) PAD(stack_1_3_reserve) : > .
    __el3_2_stack = .;
    .stack_3               ALIGN(16) PAD(stack_1_3_reserve) : > .
    __el3_3_stack = .;
    .heap                   ALIGN(16) PAD(heap_reserve) : > .

    __ghsbegin_free_mem = .;

//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
//    __ghs_romstart  = MEMADDR(flash_rsvd1);
//    __ghs_romend    = MEMENDADDR(flash_rsvd2);
    __ghs_ramstart  = MEMADDR(dram_rsvd1);
    __ghs_ramend    = MEMENDADDR(dram_rsvd2);
        
}
