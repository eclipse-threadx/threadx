;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/*       This software is licensed under the Microsoft Software License   */
;/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
;/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
;/*       and in the root directory of this software.                      */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/
;/**************************************************************************/
;/**                                                                       */ 
;/** ThreadX Component                                                     */ 
;/**                                                                       */
;/**   Thread                                                              */
;/**                                                                       */
;/**************************************************************************/
;/**************************************************************************/



;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_interrupt_disable                       Cortex-A7/IAR    */ 
;/*                                                           6.3.0        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function is responsible for disabling interrupts               */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  OUTPUT                                                                */ 
;/*                                                                        */ 
;/*    old_posture                           Old interrupt lockout posture */ 
;/*                                                                        */ 
;/*  CALLS                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    Application Code                                                    */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*  10-31-2023     Yajun Xia                Modified comment(s),          */
;/*                                            Added thumb mode support,   */
;/*                                            resulting in version 6.3.0  */
;/*                                                                        */
;/**************************************************************************/
;UINT   _tx_thread_interrupt_disable(VOID)
;{
    RSEG    .text:CODE:NOROOT(2)
    PUBLIC _tx_thread_interrupt_disable
#ifdef THUMB_MODE
    THUMB
#else
    ARM
#endif
_tx_thread_interrupt_disable
;
;    /* Pickup current interrupt lockout posture.  */
;
    MRS     r0, CPSR                            ; Pickup current CPSR
;
;    /* Mask interrupts.  */
;
#ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  ; Disable IRQ and FIQ
#else
    CPSID   i                                   ; Disable IRQ 
#endif

    BX      lr                                  ; Return to caller

;}
;
    END

