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
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Timer                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_timer_interrupt                               Cortex-M23/GNU    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function processes the hardware timer interrupt.  This         */
/*    processing includes incrementing the system clock and checking for  */
/*    time slice and/or timer expiration.  If either is found, the        */
/*    interrupt context save/restore functions are called along with the  */
/*    expiration functions.                                               */
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
/*    _tx_timer_expiration_process          Timer expiration processing   */
/*    _tx_thread_time_slice                 Time slice interrupted thread */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    interrupt vector                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_timer_interrupt(VOID)
{ */
    .section .text
    .balign 4
    .syntax unified
    .eabi_attribute Tag_ABI_align_preserved, 1
    .global  _tx_timer_interrupt
    .thumb_func
.type _tx_timer_interrupt, function
_tx_timer_interrupt:

    /* Upon entry to this routine, it is assumed that the compiler scratch registers are available
       for use.  */

    /* Increment the system clock.  */
    // _tx_timer_system_clock++;

    MOVW    r1, #:lower16:_tx_timer_system_clock    // Pickup address of system clock
    MOVT    r1, #:upper16:_tx_timer_system_clock
    LDR     r0, [r1, #0]                            // Pickup system clock
    ADDS    r0, r0, #1                              // Increment system clock
    STR     r0, [r1, #0]                            // Store new system clock

    /* Test for time-slice expiration.  */
    // if (_tx_timer_time_slice)
    // {

    MOVW    r3, #:lower16:_tx_timer_time_slice      // Pickup address of time-slice
    MOVT    r3, #:upper16:_tx_timer_time_slice
    LDR     r2, [r3, #0]                            // Pickup time-slice
    CBZ     r2,  __tx_timer_no_time_slice           // Is it non-active?
                                                    // Yes, skip time-slice processing

    /* Decrement the time_slice.  */
    // _tx_timer_time_slice--;

    SUBS    r2, r2, #1                              // Decrement the time-slice
    STR     r2, [r3, #0]                            // Store new time-slice value

    /* Check for expiration.  */
    // if (__tx_timer_time_slice == 0)

    CBNZ    r2, __tx_timer_no_time_slice            // Has it expired?

    /* Set the time-slice expired flag.  */
    // _tx_timer_expired_time_slice =  TX_TRUE;

    MOVW    r3, #:lower16:_tx_timer_expired_time_slice  // Pickup address of expired flag
    MOVT    r3, #:upper16:_tx_timer_expired_time_slice
    MOVW    r0, #1                                  // Build expired value
    STR     r0, [r3, #0]                            // Set time-slice expiration flag

    // }

__tx_timer_no_time_slice:

    /* Test for timer expiration.  */
    // if (*_tx_timer_current_ptr)
    // {

    MOVW    r1, #:lower16:_tx_timer_current_ptr     // Pickup current timer pointer address
    MOVT    r1, #:upper16:_tx_timer_current_ptr
    LDR     r0, [r1, #0]                            // Pickup current timer
    LDR     r2, [r0, #0]                            // Pickup timer list entry
    CBZ     r2, __tx_timer_no_timer                 // Is there anything in the list?
                                                    // No, just increment the timer

    /* Set expiration flag.  */
    // _tx_timer_expired =  TX_TRUE;

    MOVW    r3, #:lower16:_tx_timer_expired         // Pickup expiration flag address
    MOVT    r3, #:upper16:_tx_timer_expired
    MOVW    r2, #1                                  // Build expired value
    STR     r2, [r3, #0]                            // Set expired flag
    B       __tx_timer_done                         // Finished timer processing

    // }
    // else
    // {
__tx_timer_no_timer:

    /* No timer expired, increment the timer pointer.  */
    // _tx_timer_current_ptr++;

    ADDS    r0, r0, #4                              // Move to next timer

    /* Check for wrap-around.  */
    // if (_tx_timer_current_ptr == _tx_timer_list_end)

    MOVW    r3, #:lower16:_tx_timer_list_end        // Pickup addr of timer list end
    MOVT    r3, #:upper16:_tx_timer_list_end
    LDR     r2, [r3, #0]                            // Pickup list end
    CMP     r0, r2                                  // Are we at list end?
    BNE     __tx_timer_skip_wrap                    // No, skip wrap-around logic

        /* Wrap to beginning of list.  */
        // _tx_timer_current_ptr =  _tx_timer_list_start;

    MOVW    r3, #:lower16:_tx_timer_list_start      // Pickup addr of timer list start
    MOVT    r3, #:upper16:_tx_timer_list_start
    LDR     r0, [r3, #0]                            // Set current pointer to list start

__tx_timer_skip_wrap:

    STR     r0, [r1, #0]                            // Store new current timer pointer
    // }

__tx_timer_done:


    /* See if anything has expired.  */
    // if ((_tx_timer_expired_time_slice) || (_tx_timer_expired))
    // {

    MOVW    r3, #:lower16:_tx_timer_expired_time_slice  // Pickup addr of expired flag
    MOVT    r3, #:upper16:_tx_timer_expired_time_slice
    LDR     r2, [r3, #0]                            // Pickup time-slice expired flag
    CBNZ    r2, __tx_something_expired              // Did a time-slice expire?
                                                    // If non-zero, time-slice expired
    MOVW    r1, #:lower16:_tx_timer_expired         // Pickup addr of other expired flag
    MOVT    r1, #:upper16:_tx_timer_expired
    LDR     r0, [r1, #0]                            // Pickup timer expired flag
    CBZ     r0, __tx_timer_nothing_expired          // Did a timer expire?
                                                    // No, nothing expired

__tx_something_expired:

    PUSH    {r0, lr}                                // Save the lr register on the stack
                                                    //   and save r0 just to keep 8-byte alignment

    /* Did a timer expire?  */
    // if (_tx_timer_expired)
    // {

    MOVW    r1, #:lower16:_tx_timer_expired         // Pickup addr of expired flag
    MOVT    r1, #:upper16:_tx_timer_expired
    LDR     r0, [r1, #0]                            // Pickup timer expired flag
    CBZ     r0, __tx_timer_dont_activate            // Check for timer expiration
                                                    // If not set, skip timer activation

    /* Process timer expiration.  */
    // _tx_timer_expiration_process();

    BL      _tx_timer_expiration_process            // Call the timer expiration handling routine

    // }
__tx_timer_dont_activate:

    /* Did time slice expire?  */
    // if (_tx_timer_expired_time_slice)
    // {

    MOVW    r3, #:lower16:_tx_timer_expired_time_slice  // Pickup addr of time-slice expired
    MOVT    r3, #:upper16:_tx_timer_expired_time_slice
    LDR     r2, [r3, #0]                            // Pickup the actual flag
    CBZ     r2, __tx_timer_not_ts_expiration        // See if the flag is set
                                                    // No, skip time-slice processing

    /* Time slice interrupted thread.  */
    // _tx_thread_time_slice(); 

    BL      _tx_thread_time_slice                   // Call time-slice processing
    MOVW    r0, #:lower16:_tx_thread_preempt_disable  // Build address of preempt disable flag
    MOVT    r0, #:upper16:_tx_thread_preempt_disable

    LDR     r1, [r0]                                // Is the preempt disable flag set?
    CBNZ    r1, __tx_timer_skip_time_slice          // Yes, skip the PendSV logic
    MOVW    r0, #:lower16:_tx_thread_current_ptr    // Build current thread pointer address
    MOVT    r0, #:upper16:_tx_thread_current_ptr

    LDR     r1, [r0]                                // Pickup the current thread pointer
    MOVW    r2, #:lower16:_tx_thread_execute_ptr    // Build execute thread pointer address
    MOVT    r2, #:upper16:_tx_thread_execute_ptr

    LDR     r3, [r2]                                // Pickup the execute thread pointer
    LDR     r0, =0xE000ED04                         // Build address of control register
    LDR     r2, =0x10000000                         // Build value for PendSV bit
    CMP     r1, r3                                  // Are they the same?
    BEQ     __tx_timer_skip_time_slice              // If the same, there was no time-slice performed
    STR     r2, [r0]                                // Not the same, issue the PendSV for preemption
__tx_timer_skip_time_slice:
    // }

__tx_timer_not_ts_expiration:

    POP   {r0, r1}                                  // Recover lr register (r0 is just there for
    MOV   lr, r1                                    //   the 8-byte stack alignment

    // }

__tx_timer_nothing_expired:

    DSB                                             // Complete all memory access
    BX      lr                                      // Return to caller

// }
    .end
