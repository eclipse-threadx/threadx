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
/*    This function saves the context of an executing thread in the       */ 
/*    beginning of interrupt processing.  The function also ensures that  */ 
/*    the system stack is used upon return to the calling ISR.            */ 
/*                                                                        */ 
/*    Interrupts remain disabled and no exceptions are triggered!         */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */ 
/**************************************************************************/ 

//  VOID   _tx_thread_context_save(VOID)
//  {
    .globl  _tx_thread_context_save
    .type   _tx_thread_context_save,@function
    .align  4
_tx_thread_context_save:

    /*
    Please note: Control flow might seem strange. This is because it has been
    optimized to avoid taken branches in the longest normal path (the critical
    one for worst-case latency), presumed to be a non-nested interrupt and 
    non-idle) and to hide pipeline interlock cycles where possible.
    */

    /*
    Save a couple of scratch regs to work with that are preserved over the
    call to _xt_context_save. The latter assumes the interruptee's values 
    of these are already saved and these regs contain different data to be
    preserved, so doesn't save them in the stack frame, and thereby requires
    that its caller have already saved them in the interrupt stack frame.
    We end up with a12 = return address, a13 and a0 are scratch.
    */
    s32i    a12, sp, XT_STK_A12
    s32i    a13, sp, XT_STK_A13

    /* Check for a nested interrupt condition and increment nesting count.  */
    //  if (_tx_thread_system_state++)
    //  {
    movi    a13, _tx_thread_system_state /* a13 = & interrupt nesting count */
    mov     a12, a0                      /* a12 = save ret addr (free a0) */
    l32i    a0,  a13, 0                  /* increment interrupt nesting count */
    addi    a0,  a0,  1
    s32i    a0,  a13, 0
    bnei    a0,  1,   .L_tx_thread_nested_save  /* was !=0 before increment? */

    //  }

.Ln_tx_thread_not_nested_save:

    /* Otherwise, not nested, check to see if a thread was running. */
    //  else 
    //  {
    //      if (_tx_thread_current_ptr)
    //      {
    movi    a0,  _tx_thread_current_ptr
    l32i    a13, a0,  0                  /* a13 = current thread ctrl blk */
    beqz    a13, .L_tx_thread_idle_system_save

    /* Save the rest of the interrupted context. */
    call0   _xt_context_save

    /* Save the current stack pointer in the thread's control block. */
    //          _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
    s32i    sp,  a13, tx_thread_stack_ptr

    //      }
    /* Switch to the system stack and return to ISR. */

.L_tx_thread_idle_system_save:

    /* 
    If interrupted in the idle state, it's not necessary to save any context.
    But even in the idle case where we are already on the system stack, it is 
    necessary to reset the (system) stack pointer so a series of consecutive 
    interrupts in the idle state do not keep moving the SP downward.
    */

    //      sp =  _tx_thread_system_stack_ptr;
    movi    a13, _tx_thread_system_stack_ptr
    mov     a0,  a12                     /* retrieve return address */
    l32i    sp,  a13, 0

    ret
    //  }

.L_tx_thread_nested_save:
    /* Nested interrupt condition. */
    /* Save the rest of the interrupted context and return to ISR. */
    call0   _xt_context_save
    
    mov     a0,  a12                     /* retrieve return address */
    ret

//  }

#endif /* XCHAL_HAVE_XEA2 */

