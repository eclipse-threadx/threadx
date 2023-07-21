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

INT_MASK        =   0x0C0
IRQ_MASK        =   0x080
#ifdef TX_ENABLE_FIQ_SUPPORT
FIQ_MASK        =   0x040
#endif

    .text
    .align 2
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_control                         ARMv7-A        */
/*                                                           6.x          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for changing the interrupt lockout     */
/*    posture of the system.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_posture                           New interrupt lockout posture */
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
    .global _tx_thread_interrupt_control
    .type   _tx_thread_interrupt_control,function
_tx_thread_interrupt_control:
    MRS     r1, CPSR                            // Pickup current CPSR

#ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ
#else
    CPSID   i                                   // Disable IRQ
#endif

    TST     r0, #IRQ_MASK
    BNE     no_irq
    CPSIE   i
no_irq:
#ifdef TX_ENABLE_FIQ_SUPPORT
    TST     r0, #FIQ_MASK
    BNE     no_fiq
    CPSIE   f
no_fiq:
#endif

    AND     r0, r1, #INT_MASK
    BX      lr
