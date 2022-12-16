OUTPUT_ARCH(mips)

/**** Start point ****/

ENTRY(_start)			/* Entry point of application		*/

SECTIONS
{
  /**** Code and read-only data ****/

  .vector_0x000 0x80000000 :
  {
  }

  .vector_0x100 0x80000100 :
  {
  }

  .vector_0x180 0x80000180 :
  {
  }

  .vector_0x200 0x80000200 :
  {
  }

  .vector_0x280 0x80000280 :
  {
  }

  .vector_0x300 0x80000300 :
  {
  }

  .text 0x80100000 :
  {
    _ftext = ABSOLUTE(.) ;	/* Start of code and read-only data	*/
    start.o(.text)		    /* Reset entry point		    */
    *(.text*)
    _ecode = ABSOLUTE(.) ;	/* End of code				*/

    *(.rodata*)

    . = ALIGN(8);
    _etext = ABSOLUTE(.) ;	/* End of code and read-only data	*/
  } = 0

  /**** Initialised data ****/

  .data :
  {
    _fdata = ABSOLUTE(.);	/* Start of initialised data		*/
    *(.data*)
   
    . = ALIGN(8);

    _gp = ABSOLUTE(. + 0x7ff0); /* Base of small data			*/
    LC8 = ABSOLUTE(. + 0x7ff0); /* Base of small data			*/

    *(.lit8) 
    *(.lit4) 
    *(.sdata*) 

    . = ALIGN(8);

    _edata  = ABSOLUTE(.) ;	/* End of initialised data		*/
  }

  /**** Uninitialised data ****/

  .sbss : 
  { 
    _start_sbss = .;
    *(.sbss*) 
    *(.scommon)
    _end_sbss = .;
  }
  .bss :
  {
    _start_bss = .;
    *(.bss*)
    *(COMMON)
    _ebss = ABSOLUTE(.) ;
    _end_bss =  .;
  }

  .stack _ebss :
  {
    /* Allocate room for stack */
    .   =  ALIGN(8) ;
    .   += 4096 ;
    _sp =  . - 16 ;
    _stack_top = ABSOLUTE(.) ;
  }

  _free_memory =  _stack_top + 4 ;

  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }

  /DISCARD/ :
  {
    *(.reginfo)
  }

  PROVIDE(etext = _etext);
  PROVIDE (end = _stack_top);
}

