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
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_port_asm.h"

	.section .text.tx_port, "ax"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                              RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function makes the reset stack the system stack and gives the  */
/*    free memory block of the board, tx_port_free_memory, to             */
/*    tx_application_define.                                              */
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
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*                                                                        */
/**************************************************************************/
	.global	C_SYM(_tx_initialize_low_level)
C_SYM(_tx_initialize_low_level):
	mov	hilo(__stack_top), r10
	mov	hilo(C_SYM(_tx_thread_system_stack_ptr)), r11
	st.w	r10, 0[r11]
	mov	hilo(C_SYM(tx_port_free_memory)), r10
	mov	hilo(C_SYM(_tx_initialize_unused_memory)), r11
	st.w	r10, 0[r11]
	jr	C_SYM(tx_port_setup_timer_interrupt)

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    rh850_eiint_entry                                     RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the entry of every EIINT. It saves the interrupt   */
/*    context, reenables interrupts so that higher priorities nest,       */
/*    calls the interrupt controller dispatch with the EIIC value and     */
/*    returns through the context restore.                                */
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
/*    _tx_thread_context_save              Save interrupt context         */
/*    rh850_intc_dispatch                  Board interrupt dispatch       */
/*    _tx_thread_context_restore           Restore interrupt context      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    EIINT vectors                        Interrupt request              */
/*                                                                        */
/**************************************************************************/
	.global	rh850_eiint_entry
rh850_eiint_entry:
	addi	-8, sp, sp
	st.w	r10, 0[sp]
	st.w	r11, 4[sp]
	mov	hilo(C_SYM(_tx_thread_system_state)), r10
	ld.w	0[r10], r11
	cmp	r0, r11
	ld.w	0[sp], r10
	ld.w	4[sp], r11
	/* movea keeps the flags of cmp */
	movea	8, sp, sp
	bne	1f
	tx_frame_save
	br	2f
1:	tx_isr_save
2:	jarl	C_SYM(_tx_thread_context_save), lp
	rh850_stsr	RH850_SR_EIIC, r6
	ei
	jarl	C_SYM(rh850_intc_dispatch), lp
	di
	jr	C_SYM(_tx_thread_context_restore)
