/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/*
 *  ThreadX RISC-V32 GNU port: QEMU virt machine reset/entry stub.
 *
 *  Linked at the load address of the QEMU `virt` machine (see link.lds).
 *  Placed in the .init section so the linker keeps it at the very start
 *  of the image regardless of file ordering during link, matching the
 *  PC value QEMU uses on reset.
 *
 *  Responsibilities:
 *    1. Park secondary harts on `wfi` (single-hart bring-up only; SMP TBD).
 *    2. Zero the integer register file on the boot hart, including x3
 *       (the global pointer) which we load explicitly with the linker
 *       symbol `__global_pointer$` below, with relaxation disabled so
 *       the `la gp, ...` itself is not subject to GP-relative rewriting.
 *    3. Set up the system stack and clear .bss before calling main().
 */

.section .init
.align 4
.global _start
.extern main
.extern _sysstack_start
.extern _bss_start
.extern _bss_end
_start:
	csrr t0, mhartid
	bne  t0, zero, 1f

	/* Zero general purpose registers (x3/gp handled by the la below). */
	li x1, 0
	li x2, 0
.option push
.option norelax
	la gp, __global_pointer$        /* x3 = gp; norelax keeps this load absolute */
.option pop
	li x4, 0
	li x5, 0
	li x6, 0
	li x7, 0
	li x8, 0
	li x9, 0
	li x10, 0
	li x11, 0
	li x12, 0
	li x13, 0
	li x14, 0
	li x15, 0
	li x16, 0
	li x17, 0
	li x18, 0
	li x19, 0
	li x20, 0
	li x21, 0
	li x22, 0
	li x23, 0
	li x24, 0
	li x25, 0
	li x26, 0
	li x27, 0
	li x28, 0
	li x29, 0
	li x30, 0
	li x31, 0

	/* Set up the system stack: top-of-stack = _sysstack_start + 0x1000. */
	la  t0, _sysstack_start
	li  t1, 0x1000
	add sp, t0, t1

	/* Clear .bss [_bss_start, _bss_end). */
	la  t0, _bss_start
	la  t1, _bss_end
_bss_clean_start:
	bgeu t0, t1, _bss_clean_end
	sb   zero, 0(t0)
	addi t0, t0, 1
	j    _bss_clean_start
_bss_clean_end:
	call main

1:
	/* Secondary harts: park here. SMP bring-up is not yet supported. */
	wfi
	j 1b
