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

/**************************************************************************/
/*   Copyright (c) Cadence Design Systems, Inc.                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#include "xtensa_rtos.h"
#include "tx_api_asm.h"


#if XCHAL_HAVE_XEA2

    .text

/**************************************************************************/
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function restores the interrupt context if it is processing a  */
/*    nested interrupt. If not, it returns to the interrupt thread if no  */
/*    preemption is necessary.  Otherwise, if preemption is necessary or  */
/*    if no thread was running, the function returns to the scheduler.    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*  12-31-2020      Cadence Design Systems  Initial Version 6.1.3         */
/*  10-31-2022      Scott Larson            Updated EPK definitions,      */
/*                                            resulting in version 6.2.0  */
/*                                                                        */
/**************************************************************************/

//  VOID   _tx_thread_context_restore(VOID)
//  {
    .globl  _tx_thread_context_restore
    .type   _tx_thread_context_restore,@function
    .align  4
_tx_thread_context_restore:

    /*
    Please note: Control flow might seem strange. This is because it has been
    optimized to avoid taken branches in the longest normal path (the critical
    one for worst-case latency), presumed to be a non-nested interrupt that
    preempts) and to hide pipeline interlock cycles where possible.
    */

    /* Lockout interrupts.  */
    XT_INTS_DISABLE(a0)

    #if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the ISR exit function to indicate an ISR is complete. */
    #ifdef __XTENSA_CALL0_ABI__
    call0   _tx_execution_isr_exit
    #else
    call8   _tx_execution_isr_exit
    #endif
    #endif

    /* Determine if interrupts are nested.  */
    //  if (--_tx_thread_system_state)
    //  {

    movi    a2, _tx_thread_system_state /* a2 = & interrupt nesting count */
    l32i    a3, a2, 0                   /* decrement interrupt nesting count */
    addi    a3, a3, -1
    s32i    a3, a2, 0
    bnez    a3, .L_tx_thread_nested_restore

    //  }

.Ln_tx_thread_not_nested_restore:

    /* Determine if a thread was interrupted and no preemption is required.  */
    //  else if (((_tx_thread_current_ptr)
    //              && (_tx_thread_current_ptr == _tx_thread_execute_ptr))
    //           || (_tx_thread_preempt_disable))
    //  {

    movi    a0, _tx_thread_current_ptr  /* a0 = &_tx_thread_current_ptr */
    l32i    a2, a0, 0                   /* a2 =  _tx_thread_current_ptr (old) */
    movi    a3, _tx_thread_execute_ptr
    beqz    a2, .L_tx_thread_idle_system_restore

    l32i    a3, a3, 0                   /* a3 = _tx_thread_execute_ptr (new) */
    beq     a3, a2, .L_tx_thread_no_preempt_restore

    movi    a3, _tx_thread_preempt_disable
    l32i    a3, a3, 0                   /* a3 = _tx_thread_preempt_disable */

    //      /* the no-preempt case has moved down so we fall-thru to preempt */
    bgei    a3, 1, .L_tx_thread_no_preempt_restore

    //  }
    //  else
    //  {

.Ln_tx_thread_preempt_restore:

    /* Save remaining context on the thread's stack.  */
    l32i    a3, a2, tx_thread_stack_ptr /* a3 = thread's stack ptr */

    /* Store standard preserved registers.  */
    /*
    Call0 ABI callee-saved regs a12-15 need to be saved before preemption.
    However a12-13 were saved for scratch by _tx_thread_context_save().
    */
    #ifdef __XTENSA_CALL0_ABI__         /* Call0: now save callee-save regs  */
    s32i    a14, a3, XT_STK_A14
    s32i    a15, a3, XT_STK_A15
    #endif

    /* Save the remaining time-slice and disable it.  */
    //      if (_tx_timer_time_slice)
    //      {
    movi    a3, _tx_timer_time_slice    /* a3 = &_tx_timer_time_slice */
    l32i    a4, a3, 0                   /* a4 =  _tx_timer_time_slice */
    beqz    a4, .L_tx_thread_dont_save_ts

    //          _tx_thread_current_ptr -> tx_thread_time_slice
    //              =  _tx_timer_time_slice;
    //          _tx_timer_time_slice =  0;  */
    s32i    a4, a2, tx_thread_time_slice
    movi    a4, 0
    s32i    a4, a3, 0

    //      }

.L_tx_thread_dont_save_ts:

    /* Clear the current task pointer.  */
    //      _tx_thread_current_ptr =  TX_NULL;
    s32i    a4, a0, 0                   /* a4 == 0 == TX_NULL */

    #if XCHAL_CP_NUM > 0
    /* Save CPENABLE in thread's co-processor save area, and clear CPENABLE. */
    rsr     a3, CPENABLE
    s16i    a3, a2, tx_thread_cp_state + XT_CPENABLE
    wsr     a4, CPENABLE                /* disable all co-processors */
    #endif

.L_tx_thread_idle_system_restore:

    /*
    Return via the scheduler.
    Scheduler returns eventually to this function's caller as if called by it.
    At this point we are still on the system stack.
    */
    //      _tx_thread_schedule();
    call0   _tx_thread_schedule         /* never returns here */

    //  }

    /* Flow never falls through here. */

.L_tx_thread_no_preempt_restore:

    /* Restore interrupted thread.  */

    /* Pickup the saved stack pointer.  */
    //      SP =  _tx_thread_current_ptr -> tx_thread_stack_ptr;
    l32i    sp, a2, tx_thread_stack_ptr

.L_tx_thread_nested_restore:

    /* Recover the saved context and return to the point of interrupt.  */

    call0   _xt_context_restore

    /*
    Must return via the exit dispatcher corresponding to the entrypoint
    from which this was called. Interruptee's A0, A1, PS, PC are restored
    and the interrupt stack frame is deallocated in the exit dispatcher.
    At this point we are on the thread's stack.
    */
    l32i    a0, sp, XT_STK_EXIT
    ret

//  }

#endif /* XCHAL_HAVE_XEA2 */

