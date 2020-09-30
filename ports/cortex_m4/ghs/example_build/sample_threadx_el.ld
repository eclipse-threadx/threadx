# The following explains what the default Green Hills sections are for:
#
# picbase - base of the text sections, relocatable in -pic mode
# text - text section
# syscall - syscall section, for host I/O under Multi
# fixaddr/fixtype - for PIC/PID fixups
# rodata - read only data
# romdata - the ROM image of .data
# romsdata - the ROM image of .sdata
# secinfo - section information section, used by the start-up code
# pidbase - base of the data sections, relocatable in -pid mode
# sdabase - base of the small data area section pointer
# sbss - small BSS (zeroed data) section
# sdata - small data section
# data - non-zeroed writeable data section
# bss - zeroed data section
# heap - the heap, grows upward
# stack - the stack, grows downward

-sec
{
    .reset      0x000000 : 
    .picbase	0x1000 : 		
    .text	:			
	.comment :
    .intercall :
    .interfunc :
    .syscall	:			
    .fixaddr	:
    .fixtype	:
    .rodata	:   
    .romdata	ROM(.data) :
    .romsdata	ROM(.sdata) :
    .secinfo	:			
    .pidbase	align(16) :		
    .sdabase	:		
    .sbss	:		
    .sdata      :		
    .data	:		
    .bss	:		
    .heap align(16) pad(0x1000) :
    .stack align(16) pad(0x1000) :
    .eventlog align(16) pad(0x10000) :
    .free_mem align(16) pad(0x10000) :
    .sys_regs 0xE0000000 pad(0xe000) :
}
