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


#include "tx_port.h"
#include "xtensa_rtos.h"
#include "tx_api_asm.h"

    .text

/**************************************************************************/ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function waits for a thread control block pointer to appear in */ 
/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */ 
/*    in the variable, the corresponding thread is resumed.               */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */ 
/**************************************************************************/ 

//  VOID   _tx_thread_schedule(VOID)
//  {
    .globl  _tx_thread_schedule
    .type   _tx_thread_schedule,@function
    .align  4
_tx_thread_schedule:

#if XCHAL_HAVE_XEA3

    /* Skip "entry" - nothing to save, never returns. */

    movi    a2, PS_STACK_KERNEL | PS_DI     /* Set PS.STACK = Kernel and    */
    movi    a3, PS_STACK_MASK | PS_DI_MASK  /* disable interrupts.          */
    xps     a2, a3

#ifdef __XTENSA_CALL0_ABI__
    mov    a15,  a1                         /* Dispatch code expects a15 = old SP */
#endif

    movi    a0, _xt_dispatch + 3            /* Jump to dispatch code. It will */
    ret                                     /* check for ready thread or idle */
                                            /* and handle accordingly.        */

    ill                                     /* Should never get back here. */

#else

    /*
    Note on Windowed ABI:
    Callers of this don't expect it to return to them. Most use 'call0'.
    The only windowed (C) caller is _tx_initialize_kernel_enter().
    There are no args or results to pass. So we don't really care if the 
    window gets rotated. We can omit the 'entry' altogether and avoid the 
    need for a special "no entry" entrypoint to this function.
    */

    #ifdef XT_ENABLE_TIMING_TEST_HACK
    /* For timing_test "TS" numbers. INTERNAL USE ONLY. */
    /* Always use CALL0. We may be here with windowing disabled. */
    .extern scheduler_return
    call0   scheduler_return
    #endif

    /* 
    Wait for a thread to execute (Idle Loop).
    First ensure interrupts (except hi-pri) are disabled so result 
    of reading _tx_thread_execute_ptr can't change before testing.
    While there's no thread ready, enable interrupts and wait in a 
    low power state, then disable interrupts and repeat the test.
    */
    //  do
    //  {
    movi    a3, _tx_thread_execute_ptr
.L_tx_thread_schedule_loop:             /* Idle Loop. */
    XT_INTS_DISABLE(a2)                 /* disable interrupts if not already */
    l32i    a2, a3, 0                   /* a2 = _tx_thread_execute_ptr */
    bnez    a2, .L_tx_thread_schedule_ready
    waiti   0                           /* enable interrupts and wait for */
                                        /*   interrupt in low power state */
    j       .L_tx_thread_schedule_loop

    //  }
    //  while(_tx_thread_execute_ptr == TX_NULL);

.L_tx_thread_schedule_ready:
    
    /* Yes! We have a thread to execute.  Lockout interrupts and
       transfer control to it. Interrupts are already disabled. */

    /* Setup the current thread pointer.  */
    //  _tx_thread_current_ptr =  _tx_thread_execute_ptr;
    movi    a3, _tx_thread_current_ptr
    l32i    a0, a2, tx_thread_run_count
    s32i    a2, a3, 0                   /* a2 = _tx_thread_current_ptr (TCB) */

    /* Increment the run count for this thread.  */
    //  _tx_thread_current_ptr -> tx_thread_run_count++;
    addi    a3, a0, 1
    movi    a0, _tx_timer_time_slice
    s32i    a3, a2, tx_thread_run_count

    /* Setup time-slice, if present.  */
    //  _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;
    l32i    a3, a2, tx_thread_time_slice
    s32i    a3, a0, 0

    #ifdef TX_THREAD_SAFE_CLIB
    //  Load library-specific global context ptr address.  */

    #if XSHAL_CLIB == XTHAL_CLIB_NEWLIB
    movi    a0, _impure_ptr
    #elif XSHAL_CLIB == XTHAL_CLIB_XCLIB
    movi    a0, _reent_ptr
    #else
    #error TX_THREAD_SAFE_CLIB defined with unsupported C library.
    #endif

    l32i    a3, a2, tx_thread_clib_ptr
    s32i    a3, a0, 0                   /* point to thread's reent struct */
    #endif

    /* Switch to the thread's stack.  */
    //  SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;
    l32i    sp, a2, tx_thread_stack_ptr

    #ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* Call the thread entry function to indicate the thread is executing. */
    #ifdef __XTENSA_CALL0_ABI__
    call0   _tx_execution_thread_enter
    #else
    call8   _tx_execution_thread_enter
    #endif
    #endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */
    l32i    a3, a2, tx_thread_solicited
    bnez    a3, .L_tx_thread_synch_return

.Ln_tx_thread_asynch_return:

    #if XCHAL_CP_NUM > 0
    /* Restore thread's CPENABLE (enable co-processors this thread owns). */
    l16ui   a3, a2, tx_thread_cp_state + XT_CPENABLE
    wsr     a3, CPENABLE
    #endif

    /* Here we return from unsolicited entry with an interrupt stack frame. */
    call0   _xt_context_restore

    /* In Call0 ABI, restore callee-saved regs (A12, A13 already restored). */
    #ifdef __XTENSA_CALL0_ABI__
    l32i    a14, sp, XT_STK_A14   
    l32i    a15, sp, XT_STK_A15   
    #endif

    #if XCHAL_CP_NUM > 0
    rsync                               /* ensure wsr.CPENABLE has completed */
    #endif

    /*
    This does not return to its caller, but to the selected thread.
    Must return via the exit dispatcher corresponding to the entrypoint 
    from which this was called. Interruptee's A0, A1, PS, PC are restored 
    and the interrupt stack frame is deallocated in the exit dispatcher.
    */
    l32i    a0,  sp, XT_STK_EXIT   
    ret

.L_tx_thread_synch_return:

    /* Here we return from a solicited entry with a solicited stack frame. */
    movi    a0,  TX_FALSE
    l32i    a3,  sp, XT_STK_PS   
    s32i    a0,  a2, tx_thread_solicited

    #ifdef __XTENSA_CALL0_ABI__
    l32i    a12, sp, XT_STK_A12   
    l32i    a13, sp, XT_STK_A13   
    l32i    a14, sp, XT_STK_A14   
    l32i    a15, sp, XT_STK_A15   
    #endif

    l32i    a0,  sp, XT_STK_PC          /* return address */

    #if XCHAL_CP_NUM > 0
    /* CPENABLE should already be clear (it was cleared on entry to kernel). */
    rsync                               /* ensure wsr.CPENABLE has completed */
    #endif

    wsr     a3,  PS                     /* no need to sync PS, delay is OK */

    /* This does not return to its caller, but to the selected thread. */
    #ifdef __XTENSA_CALL0_ABI__
    /* 'addi sp, sp, imm' could turn into 'addmi, addi' sequence and make */
    /* the sp briefly point to an illegal stack location. Avoid that.     */
    addi    a2,  sp, XT_STK_FRMSZ
    mov     sp,  a2
    ret
    #else
    retw
    #endif

#endif /* XCHAL_HAVE_XEA3 */
//  }

