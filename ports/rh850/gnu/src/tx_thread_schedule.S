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
/*    _tx_thread_schedule                                   RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function waits for a thread control block pointer to appear    */
/*    in the _tx_thread_execute_ptr variable. Once a thread pointer       */
/*    appears in the variable, the corresponding thread is resumed.       */
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
/*    tx_low_power_enter                   Enter low power (TX_LOW_POWER) */
/*    tx_low_power_exit                    Exit low power (TX_LOW_POWER)  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*    _tx_thread_system_return             Return to system from thread   */
/*    _tx_thread_context_restore           Restore thread's context       */
/*                                                                        */
/**************************************************************************/
	.global	C_SYM(_tx_thread_schedule)
C_SYM(_tx_thread_schedule):
	di
	mov	hilo(C_SYM(_tx_thread_system_stack_ptr)), r10
	ld.w	0[r10], sp
	rh850_stsr	RH850_SR_PSW, r10
	/* EP = 1: the EIRET into a thread ends no interrupt in the INTC */
	ori	RH850_PSW_EP, r10, r10
	ldsr	r10, RH850_SR_PSW

	mov	hilo(C_SYM(_tx_thread_execute_ptr)), r10
1:	ld.w	0[r10], r11
	cmp	r0, r11
	bne	2f
	mov	RH850_CORE_MASK_NONE, r12
	ldsr	r12, RH850_CORE_MASK_SR
#ifdef TX_LOW_POWER
	/* A request ends HALT even with ID = 1, so the time slept is counted first */
	jarl	C_SYM(tx_low_power_enter), lp
	halt
	jarl	C_SYM(tx_low_power_exit), lp
	mov	hilo(C_SYM(_tx_thread_execute_ptr)), r10
#endif
	ei
	halt
	di
	br	1b

2:	mov	hilo(C_SYM(_tx_thread_current_ptr)), r12
	st.w	r11, 0[r12]
	ld.w	TX_THREAD_RUN_COUNT[r11], r13
	add	1, r13
	st.w	r13, TX_THREAD_RUN_COUNT[r11]
	ld.w	TX_THREAD_TIME_SLICE[r11], r13
	mov	hilo(C_SYM(_tx_timer_time_slice)), r12
	st.w	r13, 0[r12]
	ld.w	TX_THREAD_STACK_PTR[r11], sp
	tx_frame_restore
