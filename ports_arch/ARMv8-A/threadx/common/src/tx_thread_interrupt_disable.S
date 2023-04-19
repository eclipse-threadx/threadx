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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_disable                         ARMv8-A        */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for disabling interrupts               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    old_posture                           Old interrupt lockout posture */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-31-2022     Andres Mlinar            Updated comments,             */
/*                                            resulting in version 6.1.10 */
/*  xx-xx-xxxx     Tiejun Zhou              Modified comment(s), added    */
/*                                            #include tx_user.h,         */
/*                                            resulting in version 6.x    */
/*                                                                        */
/**************************************************************************/
// UINT   _tx_thread_interrupt_disable(void)
// {
    .global _tx_thread_interrupt_disable
    .type   _tx_thread_interrupt_disable, @function
_tx_thread_interrupt_disable:

    /* Pickup current interrupt lockout posture.  */

    MRS     x0, DAIF                            // Pickup current interrupt lockout posture

    /* Mask interrupts.  */

    MSR     DAIFSet, 0x3                        // Lockout interrupts
    RET                                         // Return to caller
// }
