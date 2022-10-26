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

    .text

/**************************************************************************/
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is target processor specific.  It is used to transfer */
/*    control from a thread back to the system.  Only a minimal context   */
/*    is saved since the compiler assumes temp registers are going to get */
/*    slicked by a function call anyway.                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*  12-31-2020      Cadence Design Systems  Initial Version 6.1.3         */
/*  10-31-2022      Scott Larson            Updated EPK definitions,      */
/*                                            resulting in version 6.2.0  */
/*                                                                        */
/**************************************************************************/

//  VOID   _tx_thread_system_return(VOID)
//  {
    .globl  _tx_thread_system_return
    .type   _tx_thread_system_return,@function
    .align  4
_tx_thread_system_return:
    /*
    Set up solicited stack frame and save minimal context (including a0).
    Since this is solicited, no need to save regs compiler doesn't preserve.
    */

#if XCHAL_HAVE_XEA3
    #ifdef __XTENSA_CALL0_ABI__
    addi    sp, sp, -16
    #else
    entry   sp, 48
    #endif
    s32i    a0, sp, 0                       /* save return address */
#else
    #ifdef __XTENSA_CALL0_ABI__
    addi    a2, sp, -XT_STK_FRMSZ           /* avoid addi/addmi relaxation that */
    mov     sp, a2                          /* might temporarily move sp up     */
    #else
    entry   sp, XT_STK_FRMSZ
    #endif
#endif

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the thread exit function to indicate the thread is no longer executing. */
#ifdef __XTENSA_CALL0_ABI__
    call0   _tx_execution_thread_exit
#else
    call8   _tx_execution_thread_exit
#endif
#endif

#if XCHAL_HAVE_XEA3

#ifdef __XTENSA_CALL0_ABI__

#else
    ssai    0
    spillw                                  /* spill all registers          */
#endif

    /*
    Save register state into exception frame. This is safe to do with
    interrupts enabled, but we will have to revert SP to point above
    the exception frame because that is what the dispatch code expects.
    Must disable interrupts before that.
    */

    movi    a0, .Lret
    rsr.ps  a2
    addi    sp, sp, -XT_STK_XFRM_SZ
    s32i    a0, sp, XT_STK_PC               /* save return PC               */
    s32i    a2, sp, XT_STK_PS               /* save PS                      */
#ifdef __XTENSA_CALL0_ABI__
    s32i    a12, sp, XT_STK_A12             /* callee-saved registers       */
    s32i    a13, sp, XT_STK_A13
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15
#endif

    movi    a2, PS_STACK_KERNEL | PS_DI     /* Set PS.STACK = Kernel and    */
    movi    a8, PS_STACK_MASK | PS_DI_MASK  /* disable interrupts.          */
    xps     a2, a8

    movi    a3, _tx_thread_current_ptr      /* a3 = &_tx_thread_current_ptr */
    movi    a2, TX_TRUE
    l32i    a4, a3, 0                       /* a4 = _tx_thread_current_ptr  */
    movi    a5, 0
    s32i    a2, a4, tx_thread_solicited     /* mark as solicited switch     */

#if XCHAL_CP_NUM > 0
    /* Save coprocessor callee-saved state (if any). At this point CPENABLE */
    /* should still reflect which CPs were in use (enabled).                */
    call0   _xt_coproc_savecs

    /* Clear CPENABLE and give up all co-procs. */
    s16i    a5, a4, tx_thread_cp_state + XT_CPENABLE
    wsr     a5, CPENABLE                    /* disable all co-processors    */
#endif

    addi    sp, sp, XT_STK_XFRM_SZ          /* restore SP                   */
    addi    a2, sp, -XT_STK_FRMSZ
    s32i    a2, a4, tx_thread_stack_ptr     /* Save SP in TCB               */

#ifdef __XTENSA_CALL0_ABI__
    mov     a15, sp                         /* Dispatch code expects a15 = old a1 */
#endif

    s32i    a5, a3, 0                       /* Clear _tx_thread_current_ptr */

    movi    a0, _xt_dispatch + 3            /* Jump to dispatch code */
    ret

    /* Execution returns here. Interrupts should be disabled. */
    /* NOTE: we expect original SP to have been restored. */

    .align  4

.Lret:
    addi    sp, sp, -XT_STK_XFRM_SZ         /* Prepare to restore state        */
    l32i    a2, sp, XT_STK_PS               /* Retrieve PS value               */
#ifdef __XTENSA_CALL0_ABI__
    l32i    a12, sp, XT_STK_A12             /* Callee-saved registers          */
    l32i    a13, sp, XT_STK_A13
    l32i    a14, sp, XT_STK_A14
    l32i    a15, sp, XT_STK_A15
#endif
    addi    sp, sp, XT_STK_XFRM_SZ
    wsr.ps  a2                              /* Safe to enable interrupts       */
    rsync

#ifdef __XTENSA_CALL0_ABI__
    l32i    a0, sp, 0
    addi    sp, sp, 16
    ret
#else
    l32i    a0, sp, 0
    retw
#endif

#else /* XEA1 or XEA2 */

    rsr     a2,  PS
    s32i    a0,  sp, XT_STK_PC
    s32i    a2,  sp, XT_STK_PS
    #ifdef __XTENSA_CALL0_ABI__
    s32i    a12, sp, XT_STK_A12
    s32i    a13, sp, XT_STK_A13
    s32i    a14, sp, XT_STK_A14
    s32i    a15, sp, XT_STK_A15
    #else
    /*
    Spill register windows. Calling xthal_window_spill() causes extra spills and
    reloads, so we set things up to call the _nw version instead to save cycles.
    */
    movi    a6, ~(PS_WOE_MASK|PS_INTLEVEL_MASK)     // (using a6 ensures any window using this a4..a7 is spilled)
    mov     a4, a0                                  // save a0
    and     a2, a2, a6                              // clear WOE, INTLEVEL
    addi    a2, a2, XCHAL_EXCM_LEVEL                // set INTLEVEL
    wsr     a2, PS
    rsync
    call0   xthal_window_spill_nw
    l32i    a0,  sp, XT_STK_PS
    wsr     a0,  PS                                 // Restore PS value
    rsync
    #endif

    #if XCHAL_CP_NUM > 0
    /* Save coprocessor callee-saved state (if any). At this point CPENABLE */
    /* should still reflect which CPs were in use (enabled).                */
    call0   _xt_coproc_savecs
    #endif

    /*
    We do not return directly from this function to its caller.
    Register usage from here on:
        a0  = scratch (return address has been saved in stack frame)
        a1  = stack ptr (thread, then system)
        a2  = &_tx_thread_current_ptr
        a3  = _tx_thread_current_ptr (thread control block)
        a4  = &_tx_timer_time_slice
    */

    /* Lock out interrupts (except hi-pri). */
    /* Grab thread control block of current thread. */
    movi    a2, _tx_thread_current_ptr      /* a2 = &_tx_thread_current_ptr */
    XT_INTS_DISABLE(a0)
    l32i    a3, a2, 0                       /* a3 points to TCB */

    /* Mark as having solicited entry to kernel (used on exit). */
    movi    a0, TX_TRUE
    s32i    a0, a3, tx_thread_solicited

    /* Save current stack and switch to system stack. */
    //  _tx_thread_current_ptr -> tx_thread_stack_ptr =  SP;
    //  SP = _tx_thread_system_stack_ptr;
    movi    a5, _tx_thread_system_stack_ptr /* a5 = & system stack ptr */
    s32i    sp, a3, tx_thread_stack_ptr
    movi    a4, _tx_timer_time_slice        /* a4 = &_tx_timer_time_slice */
    l32i    sp, a5, 0                       /* sp = system stack ptr */

    /* Determine if the time-slice is active. */
    //  if (_tx_timer_time_slice)
    //  {
    l32i    a0, a4, 0
    beqz    a0, .L_tx_thread_dont_save_ts

    /* Save time-slice for the thread and clear current time-slice. */
    //      _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
    //      _tx_timer_time_slice =  0;
    s32i    a0, a3, tx_thread_time_slice
    movi    a0, 0                           /* a0 == 0 == TX_NULL */
    s32i    a0, a4, 0

    //  }

.L_tx_thread_dont_save_ts:

    /* Clear the current thread pointer. */
    //  _tx_thread_current_ptr =  TX_NULL;
    s32i    a0, a2, 0                       /* a0 == 0 == TX_NULL */

    #if XCHAL_CP_NUM > 0
    /* Clear CPENABLE and give up all co-procs. */
    s16i    a0, a3, tx_thread_cp_state + XT_CPENABLE
    wsr     a0, CPENABLE                /* disable all co-processors */
    #endif

    /*
    Return via the scheduler.
    Scheduler returns eventually to this function's caller as if called by it.
    */
    call0   _tx_thread_schedule             /* never returns here */

#endif /* XCHAL_HAVE_XEA3 */
//  }

