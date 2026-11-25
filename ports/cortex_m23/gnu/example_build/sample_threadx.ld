MEMORY
{
  RAM (wx) : ORIGIN = 0x20000000, LENGTH = 0x00800000
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00400000
}

__STACKSIZE__ = 1024;
__STACKSIZE_PROCESS__ = 0;
__HEAPSIZE__ = 128;

SECTIONS
{
  .vectors :
  {
    KEEP(*(.vectors .vectors.*))
  } > FLASH

	.text :
	{
		__text_start__ = .;

		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		__ctors_load_start__ = .;
		__ctors_start__ = ALIGN(4);
		*(SORT(.ctors.*))
		*(.ctors)
		__ctors_end__ = ALIGN(4);

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		__dtors_load_start__ = .;
		__dtors_start__ = ALIGN(4);
		*(SORT(.dtors.*))
		*(.dtors)
		__dtors_end__ = ALIGN(4);

		KEEP(*(.eh_frame*))

		__text_end__ = ALIGN(4);
	} > FLASH

	__text_load_start__ = LOADADDR(.text);

	.rodata :
	{
		__rodata_start__ = .;
		*(.rodata*)
		__rodata_end__ = ALIGN(4);
	} > FLASH

	__rodata_load_start__ = LOADADDR(.rodata);

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > FLASH
	__exidx_end = .;

	__data_load_start__ = ALIGN (4);

	/* Fast section: for time-critical code/data to be copied from FLASH to RAM at startup.
	   No .fast content in this sample; symbols defined so crt0 startup copy is a no-op. */
	.fast (NOLOAD) :
	{
		__fast_start__ = .;
		*(.fast*)
		__fast_end__ = .;
	} > RAM

	__fast_load_start__ = __fast_start__;

	.data : AT (__data_load_start__)
	{
		__data_start__ = .;

		*(vtable)
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */

    __data_end__ = .;

	} > RAM

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;

	} > RAM

	.heap (COPY):
	{
    __heap_start__ = ALIGN(4);
    *(.heap)
    . = ALIGN(. + __HEAPSIZE__, 4);
    __heap_end__ = ALIGN(4);
	} > RAM

  .stack ALIGN(4) (NOLOAD) :
  {
    __stack_start__ = ALIGN(4);
    *(.stack)
    . = ALIGN(. + __STACKSIZE__, 4);
    __stack_end__ = ALIGN(4);
  } > RAM

  __RAM_segment_used_end__ = .;
}
