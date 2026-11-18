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
/*    _tx_thread_context_restore                            RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function restores the interrupt context if it is processing a  */
/*    nested interrupt. If not, it returns to the interrupt thread if no  */
/*    preemption is necessary. Otherwise, if preemption is necessary or   */
/*    if no thread was running, the function returns to the scheduler. A  */
/*    thread whose saved PLMR is masked is not preempted.                 */
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
/*    _tx_thread_schedule                  Thread scheduling routine      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    rh850_eiint_entry                    Interrupt entry                */
/*                                                                        */
/**************************************************************************/
	.global	C_SYM(_tx_thread_context_restore)
C_SYM(_tx_thread_context_restore):
	di
	mov	hilo(C_SYM(_tx_thread_system_state)), r10
	ld.w	0[r10], r11
	add	-1, r11
	st.w	r11, 0[r10]
	cmp	r0, r11
	bne	3f

	mov	hilo(C_SYM(_tx_thread_current_ptr)), r12
	ld.w	0[r12], r13
	cmp	r0, r13
	be	2f
	mov	hilo(C_SYM(_tx_thread_execute_ptr)), r14
	ld.w	0[r14], r15
	cmp	r13, r15
	be	1f
	mov	hilo(C_SYM(_tx_thread_preempt_disable)), r14
	ld.w	0[r14], r15
	cmp	r0, r15
	bne	1f
	/* A thread with PLMR masked is never switched out */
	ld.w	TX_THREAD_STACK_PTR[r13], r14
	ld.w	TX_FRAME_PLMR[r14], r15
	mov	RH850_CORE_MASK_NONE, r14
	cmp	r14, r15
	bne	1f
#if TX_PORT_USE_FPU && RH850_CORE_HAS_FPEC
	/* Keep a thread with a pending FPI; it is taken after EIRET */
	rh850_stsr	RH850_SR_FPEC, r15
	andi	RH850_FPEC_FPIVD, r15, r15
	bne	1f
#endif

	mov	hilo(C_SYM(_tx_timer_time_slice)), r14
	ld.w	0[r14], r15
	cmp	r0, r15
	be	4f
	st.w	r15, TX_THREAD_TIME_SLICE[r13]
	st.w	r0, 0[r14]
4:	st.w	r0, 0[r12]

	/* EIRET with PSW.EP = 0 also ends the interrupt in the INTC */
2:	mov	hilo(C_SYM(_tx_thread_schedule)), r10
	ldsr	r10, RH850_SR_EIPC
	rh850_stsr	RH850_SR_PSW, r10
	ldsr	r10, RH850_SR_EIPSW
	eiret

1:	ld.w	TX_THREAD_STACK_PTR[r13], sp
	tx_frame_restore

3:	tx_isr_restore
