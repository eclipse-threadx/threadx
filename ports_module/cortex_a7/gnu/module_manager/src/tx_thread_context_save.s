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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    .syntax unified
#if defined(THUMB_MODE)
    .thumb
#else
    .arm
#endif

    .global     _tx_thread_system_state
    .global     _tx_thread_current_ptr
    .global     __tx_irq_processing_return
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    .global     _tx_execution_isr_enter
#endif

/* No 16-bit Thumb mode veneer code is needed for _tx_thread_context_save
   since it will never be called 16-bit mode.  */

    .text
    .align 2
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                              ARMv7-A        */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function saves the context of an executing thread in the       */
/*    beginning of interrupt processing.  The function also ensures that  */
/*    the system stack is used upon return to the calling ISR.            */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                                                */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  10-15-2021     William E. Lamie         Modified comment(s), added    */
/*                                            execution profile support,  */
/*                                            resulting in version 6.1.9  */
/*  04-25-2022     Zhen Kong                Updated comments,             */
/*                                            resulting in version 6.1.11 */
/*  xx-xx-xxxx     Yajun Xia                Updated comments,             */
/*                                            Added thumb mode support,   */
/*                                            resulting in version 6.x    */
/*                                                                        */
/**************************************************************************/
#if defined(THUMB_MODE)
    .thumb_func
#endif
    .global _tx_thread_context_save
    .type   _tx_thread_context_save,function
_tx_thread_context_save:

    /* Upon entry to this routine, it is assumed that IRQ interrupts are locked
       out, we are in IRQ mode, and all registers are intact.  */

#ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID   if                              // Disable FIQ interrupts
#endif

    PUSH    {r0-r3}                         // Save some working registers

    /* Save minimal context of interrupted thread.  */
    MRS     r0, SPSR                        // Pickup saved SPSR
    SUB     lr, lr, #4                      // Adjust point of interrupt
    PUSH    {r0, r10, r12, lr}              // Store other registers

    LDR     r1, =_tx_thread_system_state    // Pickup address of system state variable
    LDR     r0, [r1]                        // Pickup system state
    ADD     r0, #1                          // Increment the interrupt counter
    STR     r0, [r1]                        // Store it back in the variable

    MOV     r10, #0                         // Clear stack limit

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the ISR enter function to indicate an ISR is executing.  */
    PUSH    {lr}                            // Save ISR lr
    BL      _tx_execution_isr_enter         // Call the ISR enter function
    POP     {lr}                            // Recover ISR lr
#endif

    B       __tx_irq_processing_return      // Continue IRQ processing
