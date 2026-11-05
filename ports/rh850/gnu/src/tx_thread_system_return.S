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
/*    _tx_thread_system_return                              RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is target processor specific. It is used to transfer  */
/*    control from a thread back to the ThreadX system. Only a minimal    */
/*    context is saved since the compiler assumes temp registers are      */
/*    going to get slicked by a function call anyway. The switch is made  */
/*    through TRAP0.                                                      */
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
/*    _tx_thread_schedule                  Thread scheduling loop         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX components                                                  */
/*                                                                        */
/**************************************************************************/
	.global	C_SYM(_tx_thread_system_return)
C_SYM(_tx_thread_system_return):
/* SYNCE first, so a pending FPU exception is taken in this thread */
#if TX_PORT_USE_FPU && RH850_CORE_HAS_SYNCE
	synce
#endif
	trap	0
	jmp	[lp]

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    rh850_trap0_entry                                     RH850/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Parthiban Nallathambi, Linumiz                                      */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the TRAP0 handler of _tx_thread_system_return. It  */
/*    saves the thread frame, stores the stack pointer and the time       */
/*    slice in the thread control block and enters the scheduler on the   */
/*    system stack.                                                       */
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
/*    _tx_thread_schedule                  Thread scheduling loop         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_system_return             Return to system from thread   */
/*                                                                        */
/**************************************************************************/
	.global	rh850_trap0_entry
rh850_trap0_entry:
	tx_frame_save
	mov	hilo(C_SYM(_tx_thread_current_ptr)), r12
	ld.w	0[r12], r13
	st.w	sp, TX_THREAD_STACK_PTR[r13]
	mov	hilo(C_SYM(_tx_timer_time_slice)), r14
	ld.w	0[r14], r15
	cmp	r0, r15
	be	1f
	st.w	r15, TX_THREAD_TIME_SLICE[r13]
	st.w	r0, 0[r14]
1:	st.w	r0, 0[r12]
	jr	C_SYM(_tx_thread_schedule)
