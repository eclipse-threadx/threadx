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
/**   Timer                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#include "xtensa_rtos.h"
#include "tx_api_asm.h"

#ifndef TX_NO_TIMER

    .text

/**************************************************************************/ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function processes the hardware timer interrupt.  This         */ 
/*    processing includes incrementing the system clock and checking for  */ 
/*    time slice and/or timer expiration.  If either is found, the        */ 
/*    interrupt context save/restore functions are called along with the  */ 
/*    expiration functions.                                               */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-2020     Cadence Design Systems   Initial Version 6.1.3         */
/*                                                                        */ 
/**************************************************************************/ 

//  VOID   _tx_timer_interrupt(VOID)
//  {
    .globl  _tx_timer_interrupt
    .type   _tx_timer_interrupt,@function
    .align  4
_tx_timer_interrupt:

    #ifdef __XTENSA_CALL0_ABI__
    /* Define local variable spill offsets in stack frame for Call0 ABI. */
    #define __tx_timer_interrupt_a0     0   /* ENTRY()/RET() saves/restores */
    #define __tx_timer_interrupt_a2     4   /* preserve a2 */
    #define __tx_timer_interrupt_a3     8   /* preserve a3 */
    #endif

    ENTRY(16)

    .globl  tx_timer_user_isr
    .weak   tx_timer_user_isr
    movi    a2, tx_timer_user_isr
    beqz    a2, 1f
    #ifdef __XTENSA_CALL0_ABI__
    callx0  a2
    #else
    callx8  a2
    #endif
1:

    /*
    Xtensa timers work by comparing a cycle counter with a preset value.
    Once the match occurs an interrupt is generated, and the handler has 
    to set a new cycle count into the comparator. To avoid clock drift
    due to interrupt latency, the new cycle count is computed from the old,
    not the time the interrupt was serviced. However if a timer interrupt 
    is ever serviced more than one tick late, it is necessary to process
    multiple ticks until the new cycle count is in the future, otherwise 
    the next timer interrupt would not occur until after the cycle counter 
    had wrapped (2^32 cycles later).

    do {
        ticks++;
        old_ccompare = read_ccompare_i();
        write_ccompare_i( old_ccompare + divisor );
        service one tick;
        diff = read_ccount() - old_ccompare;
    } while ( diff > divisor );
    */

.L_tx_timer_catchup:

    /* Increment the system clock.  */
    //  _tx_timer_system_clock++;
    movi    a2, _tx_timer_system_clock  /* a2 = &_tx_timer_system_clock */
    l32i    a3, a2, 0                   /* a3 =  _tx_timer_system_clock++ */
    addi    a3, a3, 1
    s32i    a3, a2, 0

    /* Update the timer comparator for the next tick. */
    #ifdef XT_CLOCK_FREQ
    movi    a2, XT_TICK_DIVISOR         /* a2 = comparator increment */
    #else
    movi    a3, _xt_tick_divisor
    l32i    a2, a3, 0                   /* a2 = comparator increment */
    #endif
    rsr     a3, XT_CCOMPARE             /* a3 = old comparator value */
    add     a4, a3, a2                  /* a4 = new comparator value */
    wsr     a4, XT_CCOMPARE             /* update comp. and clear interrupt */
    esync

    /* Test for time-slice expiration.  */
    //  if (_tx_timer_time_slice)
    //  {
    movi    a4, _tx_timer_time_slice    /* a4 = &_tx_timer_time_slice */
    l32i    a5, a4, 0                   /* a5 =  _tx_timer_time_slice */
    beqz    a5, .L_tx_timer_no_time_slice

    /* Decrement the time_slice.  */
    //      _tx_timer_time_slice--;
    addi    a5, a5, -1
    s32i    a5, a4, 0

    /* Check for expiration.  */
    //      if (_tx_timer_time_slice == 0)
    bnez    a5, .L_tx_timer_no_time_slice

    /* Set the time-slice expired flag.  */
    //          _tx_timer_expired_time_slice =  TX_TRUE;
    movi    a4, _tx_timer_expired_time_slice
    movi    a5, TX_TRUE
    s32i    a5, a4, 0

    //  }

.L_tx_timer_no_time_slice:

    /* Test for timer expiration.  */
    //  if (*_tx_timer_current_ptr)
    //  {
    movi    a4, _tx_timer_current_ptr   /* a4 = &_tx_timer_current_ptr */
    l32i    a5, a4, 0                   /* a5 =  _tx_timer_current_ptr */
    l32i    a6, a5, 0                   /* a6 = *_tx_timer_current_ptr */
    beqz    a6, .L_tx_timer_no_timer

    /* Set expiration flag.  */
    //      _tx_timer_expired =  TX_TRUE;
    movi    a6, _tx_timer_expired
    movi    a7, TX_TRUE
    s32i    a7, a6, 0
    j       .L_tx_timer_done

    //  }
    //  else
    //  {

.L_tx_timer_no_timer:

    /* No timer expired, increment the timer pointer.  */
    //      _tx_timer_current_ptr++;

    /* Check for wrap-around.  */
    //      if (_tx_timer_current_ptr == _tx_timer_list_end)
    movi    a6, _tx_timer_list_end
    l32i    a6, a6, 0                   /* a6 =  _tx_timer_list_end */
    addi    a5, a5, 4                   /* a5 = ++_tx_timer_current_ptr */
    bne     a5, a6, .L_tx_timer_skip_wrap

    /* Wrap to beginning of list.  */
    //          _tx_timer_current_ptr =  _tx_timer_list_start;
    movi    a6, _tx_timer_list_start
    l32i    a5, a6, 0                   /* a5 =  _tx_timer_list_start */

.L_tx_timer_skip_wrap:

    s32i    a5, a4, 0                   /* _tx_timer_current_ptr = a5 */
    //  }

.L_tx_timer_done:

    /* See if anything has expired.  */
    //  if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
    //  {

    #ifdef __XTENSA_CALL0_ABI__
    /* Preserve a2 and a3 across calls. */
    s32i    a2, sp, __tx_timer_interrupt_a2
    s32i    a3, sp, __tx_timer_interrupt_a3
    #endif

    /* Did a timer expire?  */
    //      if (_tx_timer_expired)
    //      {
    movi    a4, _tx_timer_expired
    l32i    a5, a4, 0
    beqz    a5, .L_tx_timer_dont_activate

    /* Call the timer expiration processing.  */
    //          _tx_timer_expiration_process();
    #ifdef __XTENSA_CALL0_ABI__
    call0   _tx_timer_expiration_process
    #else
    call8   _tx_timer_expiration_process
    #endif

    //      }

.L_tx_timer_dont_activate:

    /* Did time slice expire?  */
    //      if (_tx_timer_expired_time_slice)
    //      {
    movi    a4, _tx_timer_expired_time_slice
    l32i    a5, a4, 0
    beqz    a5, .L_tx_timer_not_ts_expiration

    /* Time slice interrupted thread.  */
    //          _tx_thread_time_slice();
    #ifdef __XTENSA_CALL0_ABI__
    call0   _tx_thread_time_slice
    #else
    call8   _tx_thread_time_slice
    #endif

    //      }

.L_tx_timer_not_ts_expiration:

    #ifdef __XTENSA_CALL0_ABI__
    /* Restore a2 and a3. */
    l32i    a2, sp, __tx_timer_interrupt_a2
    l32i    a3, sp, __tx_timer_interrupt_a3
    #endif

    //  }

.Ln_tx_timer_nothing_expired:

    /* Check if we need to process more ticks to catch up. */
    esync                               /* ensure comparator update complete */
    rsr     a4, CCOUNT                  /* a4 = cycle count */
    sub     a4, a4, a3                  /* diff = ccount - old comparator */
    blt     a2, a4, .L_tx_timer_catchup /* repeat while diff > divisor */

    RET(16)

//  }

#endif /* TX_NO_TIMER */

