;LOAD 0x0000  0x00100000
;{
;  STARTUP 0x0000
;  {
;    startup.o(StartUp, +FIRST)
;    * (InRoot$$Sections)      ; Selects all sections that must be in a root region
;    * (+RO)
;  }
;  
;  SHARED_DATA +0x0 
;  {
;    * (+RW,+ZI)
;  }
;
;  IRQ_STACK         0x001FE000 EMPTY 0x1000 {}
;  ARM_LIB_STACKHEAP 0x001FC000 EMPTY 0x2000 {}
;}
;
;
;PAGETABLES 0x00500000   0x00100000
;{
;  PAGETABLES 0x00500000 EMPTY 0x00100000
;  {
;  }
;}

LOAD_ROOT 0x0
{
	Root +0 0x10000
	{
		startup.o (StartUp, +FIRST)   ;startup code
		* (InRoot$$Sections)          ;All library sections that must be in a root region
		
	}
}
LOAD 0x48000000
{
    INIT +0 0x10000
    {
    	  demo_threadx.o (+RO)              ; Place main() in a root region for the benefit of software breakpoints
    }
    
    ; increased from 32k to 64k
    CODE +0 0x10000
    {
    	  * (+RO)                    ; Application code, including C library
    }
    
    SHARED_DATA +0 0x4000
    {
        * (+RW,+ZI)                         ; All RW and ZI Data
    }
  ; App heap for all CPUs
  ARM_LIB_HEAP  +0 ALIGN 8 EMPTY 0x2000 {}

  ; App stacks for all CPUs - see startup.S
  ARM_LIB_STACK +0 ALIGN 8 EMPTY 4*0x1000 {}

  ; IRQ stacks for all CPUs - see startup.s
  IRQ_STACK    +0 ALIGN 8 EMPTY 4*256 {}

}

PAGETABLES 0x00500000   0x00100000
{
  PAGETABLES 0x00500000 EMPTY 0x00100000
  {
  }
}
