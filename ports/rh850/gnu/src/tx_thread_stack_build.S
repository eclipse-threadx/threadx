/***************************************************************************
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_port_asm.h"

	.section .text.tx_port, "ax"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_build                                RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function builds a stack frame on the supplied thread's stack.  */
/*    The stack frame results in a fake interrupt return to the supplied  */
/*    function pointer.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                           Pointer to thread control blk  */
/*    function_ptr                         Pointer to return function     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_create                    Create thread service          */
/*                                                                        */
/**************************************************************************/
	.global	C_SYM(_tx_thread_stack_build)
C_SYM(_tx_thread_stack_build):
	ld.w	TX_THREAD_STACK_END[r6], r10
	add	1, r10
	mov	-8, r11
	and	r11, r10
	addi	-TX_FRAME_SIZE, r10, r10
	.set	off, 0
	.rept	TX_FRAME_SIZE / 4
	st.w	r0, off[r10]
	.set	off, off + 4
	.endr
	st.w	r7, TX_FRAME_EIPC[r10]
#if TX_PORT_USE_FPU
	mov	RH850_PSW_EBV | RH850_PSW_CU0, r11
	st.w	r11, TX_FRAME_EIPSW[r10]
	mov	RH850_FPSR_FS, r11
	st.w	r11, TX_FRAME_FPSR[r10]
#else
	mov	RH850_PSW_EBV, r11
	st.w	r11, TX_FRAME_EIPSW[r10]
#endif
	mov	RH850_CORE_MASK_NONE, r11
	st.w	r11, TX_FRAME_PLMR[r10]
	st.w	r10, TX_THREAD_STACK_PTR[r6]
	jmp	[lp]
