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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_timer_interrupt                                ARMv8-A-SMP      */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  xx-xx-xxxx     Tiejun Zhou              Modified comment(s), added    */
/*                                            #include tx_user.h,         */
/*                                            resulting in version 6.x    */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_timer_interrupt(VOID)
// {
    .global _tx_timer_interrupt
    .type   _tx_timer_interrupt, @function
_tx_timer_interrupt:

    MRS     x2, MPIDR_EL1                       // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x3, x2, #16, #8                     // Isolate cluster ID
#endif
    UBFX    x2, x2, #8, #8                      // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x3, x2, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x2, x2, #0, #8                      // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x2, x2, x3, LSL #2                  // Calculate CPU ID
#endif
    CMP     x2, #0                              // Is this core 0?
    BEQ     __tx_process_timer                  // If desired core, continue processing
    RET                                         // Simply return if different core
__tx_process_timer:

    /* Upon entry to this routine, it is assumed that context save has already
       been called, and therefore the compiler scratch registers are available
       for use.  */

    STP     x27, x28, [sp, #-16]!               // Save x27, x28
    STP     x29, x30, [sp, #-16]!               // Save x29 (frame pointer), x30 (link register)

    /* Get inter-core protection.  */

    BL      _tx_thread_smp_protect              // Get inter-core protection
    MOV     x28, x0                             // Save the return value in preserved register

    /* Increment the system clock.  */
    // _tx_timer_system_clock++;

    LDR     x1, =_tx_timer_system_clock         // Pickup address of system clock
    LDR     w0, [x1, #0]                        // Pickup system clock
    ADD     w0, w0, #1                          // Increment system clock
    STR     w0, [x1, #0]                        // Store new system clock

    /* Test for timer expiration.  */
    // if (*_tx_timer_current_ptr)
    // {

    LDR     x1, =_tx_timer_current_ptr          // Pickup current timer pointer addr
    LDR     x0, [x1, #0]                        // Pickup current timer
    LDR     x2, [x0, #0]                        // Pickup timer list entry
    CMP     x2, #0                              // Is there anything in the list?
    BEQ     __tx_timer_no_timer                 // No, just increment the timer

        /* Set expiration flag.  */
        // _tx_timer_expired =  TX_TRUE;

    LDR     x3, =_tx_timer_expired              // Pickup expiration flag address
    MOV     w2, #1                              // Build expired value
    STR     w2, [x3, #0]                        // Set expired flag
    B       __tx_timer_done                     // Finished timer processing

    // }
    // else
    // {
__tx_timer_no_timer:

        /* No timer expired, increment the timer pointer.  */
        // _tx_timer_current_ptr++;

    ADD     x0, x0, #8                          // Move to next timer

        /* Check for wrap-around.  */
        // if (_tx_timer_current_ptr == _tx_timer_list_end)

    LDR     x3, =_tx_timer_list_end             // Pickup addr of timer list end
    LDR     x2, [x3, #0]                        // Pickup list end
    CMP     x0, x2                              // Are we at list end?
    BNE     __tx_timer_skip_wrap                // No, skip wrap-around logic

            /* Wrap to beginning of list.  */
            // _tx_timer_current_ptr =  _tx_timer_list_start;

    LDR     x3, =_tx_timer_list_start           // Pickup addr of timer list start
    LDR     x0, [x3, #0]                        // Set current pointer to list start

__tx_timer_skip_wrap:

    STR     x0, [x1, #0]                        // Store new current timer pointer
    // }

__tx_timer_done:

    /* Did a timer expire?  */
    // if (_tx_timer_expired)
    // {

    LDR     x1, =_tx_timer_expired              // Pickup addr of expired flag
    LDR     w0, [x1, #0]                        // Pickup timer expired flag
    CMP     w0, #0                              // Check for timer expiration
    BEQ     __tx_timer_dont_activate            // If not set, skip timer activation

        /* Process timer expiration.  */
        // _tx_timer_expiration_process();

    BL      _tx_timer_expiration_process        // Call the timer expiration handling routine

    // }
__tx_timer_dont_activate:

    /* Call time-slice processing.  */
    // _tx_thread_time_slice();
    BL      _tx_thread_time_slice               // Call time-slice processing

    /* Release inter-core protection.  */

    MOV     x0, x28                             // Pass the previous status register back
    BL      _tx_thread_smp_unprotect            // Release protection

    LDP     x29, x30, [sp], #16                 // Recover x29, x30
    LDP     x27, x28, [sp], #16                 // Recover x27, x28
    RET                                         // Return to caller

// }
