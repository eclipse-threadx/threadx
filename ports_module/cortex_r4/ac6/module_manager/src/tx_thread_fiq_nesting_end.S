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


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/* #include "tx_api.h" */
/* #include "tx_thread.h" */

#define FIQ_MODE        0x11            // FIQ Mode bits

#ifdef TX_THUMB_MODE
    .thumb
#else
    .arm
#endif
    .text
    .eabi_attribute Tag_ABI_align_preserved, 1
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_fiq_nesting_end                        Cortex-R4/AC6     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is called by the application from FIQ mode after      */
/*    _tx_thread_fiq_nesting_start has been called and switches the FIQ   */
/*    processing from system mode back to FIQ mode prior to the ISR       */
/*    calling _tx_thread_fiq_context_restore.  Note that this function    */
/*    assumes the system stack pointer is in the same position after      */
/*    nesting start function was called.                                  */
/*                                                                        */
/*    This function assumes that the system mode stack pointer was setup  */
/*    during low-level initialization (tx_initialize_low_level.s).        */
/*                                                                        */
/*    This function returns with FIQ interrupts disabled.                 */
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
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_fiq_nesting_end(VOID) */
/* { */
    .global _tx_thread_fiq_nesting_end
    .type   _tx_thread_fiq_nesting_end, "function"
_tx_thread_fiq_nesting_end:
    MOV     r3, lr                              // Save ISR return address
    
    #ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ interrupts
    #else
    CPSID   i                                   // Disable IRQ interrupts
    #endif
    
    LDMIA   sp!, {r1, lr}                       // Pickup saved lr (and r1 throw-away for
                                                //   8-byte alignment logic)
    CPS     #FIQ_MODE                           // Switch back to FIQ mode
    BX      r3                                  // Return to caller
/* } */



