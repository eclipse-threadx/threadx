OUTPUT_ARCH( "riscv" )
ENTRY( _start )

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80000000;

  .text : {
    KEEP(*(.text.boot))   /* entry.s _start — must be first at 0x80000000 */
    *(.text .text.*)
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    /* Centre __global_pointer$ in the small-data window so the +/-2 KiB
       reach of GP-relative addressing covers the .sdata/.sbss area. */
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    _bss_start = .;
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
    _bss_end = .;
  }

  .stack : {
    . = ALIGN(4096);
    _sysstack_start = .;
    . += 0x1000;
    _sysstack_end = .;
  }

  PROVIDE(_end = .);
}
