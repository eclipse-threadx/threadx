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
/*    This function builds a stack frame on the supplied thread's stack.  */
/*    The stack frame looks like an interrupt frame or a solicited frame  */
/*    depending on the exception architecture of the target hardware.     */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */ 
/**************************************************************************/ 

//  VOID   _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
//  {
    .globl  _tx_thread_stack_build
    .type   _tx_thread_stack_build,@function
    .align  4
_tx_thread_stack_build:

    ENTRY0

    /* Get logical base of stack area (top). */
    l32i    a5, a2, tx_thread_stack_end /* get top-1 of stack area */
    addi    a5, a5, 1                   /* undo the -1 */
    srli    a5, a5, 4                   /* force 16-byte alignment */
    slli    a5, a5, 4                   /* a5 = post-dispatch SP (frame top) */

    /* Allocate space for the frame (frame size is already 16-byte aligned). */
    addi    a4, a5, -XT_STK_FRMSZ       /* a4 = pre-dispatch SP (frame base) */

    /* Set the thread's SP. */
    s32i    a4, a2, tx_thread_stack_ptr

#if !XCHAL_HAVE_XEA2
    addi    a4, a4, XT_STK_XTRA_SZ      /* a4 = base of exception frame */
#endif

    /* Clear the entire frame. (XEA3: only exception frame) */
    movi    a6, 0                       /* a6 = 0 */
    mov     a7, a4                      /* a7 = ptr to current word */
1:  s32i    a6, a7, 0                   /* clear current word */
    addi    a7, a7, 4                   /* point to next word */
    bltu    a7, a5, 1b                  /* repeat until frame top */

#if XCHAL_HAVE_XEA2
    s32i    a5, a4, XT_STK_A1           /* save post-dispatch SP in frame */
#endif

    /* Indicate a solicited or interrupted stack frame. */
#if XCHAL_HAVE_XEA2
    movi    a7, 0                       /* interrupted */
#else
    movi    a7, 0                       /* solicited */
#endif
    s32i    a7, a2, tx_thread_solicited

    /*
    Terminate GDB backtrace in this thread at the "return function" by ensuring
    it's A0 == 0. Since frame was cleared, don't need to do this explicitly.
    s32i    a6, a4, XT_STK_A0
    */

    /* Set the return address to the return function. */
    /* Start thread via user exception exit dispatcher (could use any). */
#if XCHAL_HAVE_XEA2
    movi    a5, _xt_user_exit
    s32i    a5, a4, XT_STK_EXIT
#else
    movi    a5, 0
    s32i    a5, a4, XT_STK_ATOMCTL
#endif

    s32i    a3, a4, XT_STK_PC

    /*
    Set thread's initial PS for C code, all int levels enabled.
    XEA2: Since we dispatch via level 1 (_xt_user_exit), must set PS.EXCM,
      which will be cleared by 'rfe' after the dispatcher, to prevent
      interrupts happening when PS is restored during the exit dispatcher.
    XEA3: nothing special, other than setting the thread stack type.
    */
#if XCHAL_HAVE_XEA2
    #ifdef __XTENSA_CALL0_ABI__
    movi    a6, PS_UM | PS_EXCM
    #else
    movi    a6, PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1)  /* pretend 'call4' */
    #endif
#else
    movi    a6, PS_STACK_FIRSTKER
#endif
    s32i    a6, a4, XT_STK_PS

#if XCHAL_HAVE_XEA2
    #ifdef XT_USE_SWPRI
    /* Set the initial virtual priority mask value to all 1's */
    movi    a3, -1
    s32i    a3, a4, XT_STK_VPRI
    #endif
#endif

    RET0

//  }

