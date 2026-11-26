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
/*    _tx_thread_context_save                               RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function saves the context of an executing thread in the       */
/*    beginning of interrupt processing. The function also ensures that   */
/*    the system stack is used upon return to the calling ISR.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
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
/*    rh850_eiint_entry                    Interrupt entry                */
/*                                                                        */
/**************************************************************************/
	.global	C_SYM(_tx_thread_context_save)
C_SYM(_tx_thread_context_save):
	mov	hilo(C_SYM(_tx_thread_system_state)), r10
	ld.w	0[r10], r11
	add	1, r11
	st.w	r11, 0[r10]
	cmp	1, r11
	bne	1f
	mov	hilo(C_SYM(_tx_thread_current_ptr)), r10
	ld.w	0[r10], r11
	cmp	r0, r11
	be	1f
	st.w	sp, TX_THREAD_STACK_PTR[r11]
	mov	hilo(C_SYM(_tx_thread_system_stack_ptr)), r10
	ld.w	0[r10], sp
1:	jmp	[lp]
