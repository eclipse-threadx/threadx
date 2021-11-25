//
// This is the linker script example (SRV3-style).
// (c) Synopsys, 2013
//  
//

//number of exceptions and interrupts 
NUMBER_OF_EXCEPTIONS = 16;//it is fixed (16)
NUMBER_OF_INTERRUPTS = 5;//depends on HW configuration

//define Interrupt Vector Table size
IVT_SIZE_ITEMS = (NUMBER_OF_EXCEPTIONS + NUMBER_OF_INTERRUPTS);//the total IVT size (in "items")
IVT_SIZE_BYTES = IVT_SIZE_ITEMS * 4;//in bytes 

//define ICCM and DCCM locations
MEMORY {
    ICCM:      ORIGIN = 0x00000000, LENGTH = 128K
    DCCM:      ORIGIN = 0x80000000, LENGTH = 128K
}
    
//define sections and groups    
SECTIONS {
    GROUP: {
        .ivt  (TEXT) :  # Interrupt table
        {
            ___ivt1 = .;
            * (.ivt)
            ___ivt2 = .;
            // Make the IVT at least IVT_SIZE_BYTES  
            . += (___ivt2 - ___ivt1 < IVT_SIZE_BYTES) ? (IVT_SIZE_BYTES - (___ivt2 - ___ivt1)) : 0;
        }
        .ivh  (TEXT) :  // Interrupt handlers
          
        //TEXT sections
        .text? : { *('.text$crt*') }
        * (TEXT): {}
        //Literals
        * (LIT): {}
    } > ICCM
    
    GROUP: {
        //data sections
        .sdata?: {}
        .sbss?: {}
        *(DATA): {}
        *(BSS): {}
        //stack
        .stack_top: {}
        .stack ALIGN(4) SIZE(DEFINED _STACKSIZE?_STACKSIZE:4096): {}
        .stack_base: {}
        //heap (empty)
        .heap? ALIGN(4) SIZE(DEFINED _HEAPSIZE?_HEAPSIZE:0): {}
        .free_memory: {}
        } > DCCM
     }
