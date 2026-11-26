/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

OUTPUT_ARCH(v850:rh850)
ENTRY(__start)

INCLUDE memory.ld

__stack_size = DEFINED(__stack_size) ? __stack_size : 0x1000;
__heap_size = DEFINED(__heap_size) ? __heap_size : 0;

SECTIONS
{
	.vectors ORIGIN(FLASH) : {
		KEEP(*(.vectors))
	} > FLASH

	.text : {
		*(.text.crt0)
		*(.text .text.*)
	} > FLASH

	.rodata : {
		*(.rodata .rodata.*)
		*(.rosdata .rosdata.* .rozdata .rozdata.*)
		. = ALIGN(4);
	} > FLASH

	.data : ALIGN(4) {
		__data_start = .;
		*(.data .data.*)
		*(.sdata .sdata.* .tdata .tdata.* .zdata .zdata.*)
		. = ALIGN(4);
		__data_end = .;
	} > RAM AT > FLASH
	__data_load = LOADADDR(.data);

	/* Before .bss, so that .bss.rh850_large is not taken by .bss.* */
	.bss_large (NOLOAD) : ALIGN(8) {
		__bss_large_start = .;
		*(.bss.rh850_large .bss.rh850_large.*)
		. = ALIGN(4);
		__bss_large_end = .;
	} > RAM_LARGE

	.bss (NOLOAD) : ALIGN(4) {
		__bss_start = .;
		*(.bss .bss.*)
		*(.sbss .sbss.* .tbss .tbss.* .zbss .zbss.*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end = .;
	} > RAM

	.stack (NOLOAD) : ALIGN(8) {
		. += __stack_size;
		__stack_top = .;
	} > RAM

	/* The C names of these symbols carry the "_" prefix of GCC v850 */
	.heap (NOLOAD) : ALIGN(8) {
		___heap_start = .;
		. += __heap_size;
		___heap_end = .;
	} > RAM
}

ASSERT(ADDR(.vectors) == 0, "vectors must be at the reset address 0")
