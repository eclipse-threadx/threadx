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

INT_MASK        EQU         0xC0                ; Interrupt bit mask
IRQ_MASK        EQU         0x80                ; Interrupt bit mask
#ifdef TX_ENABLE_FIQ_SUPPORT
FIQ_MASK        EQU         0x40                ; Interrupt bit mask
#endif

;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_interrupt_restore                       Cortex-A7/IAR    */ 
;/*                                                           6.3.0        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function is responsible for restoring interrupts to the state  */
;/*    returned by a previous _tx_thread_interrupt_disable call.           */
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    old_posture                           Old interrupt lockout posture */ 
;/*                                                                        */ 
;/*  OUTPUT                                                                */ 
;/*                                                                        */ 
;/*    None                                                                */ 
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
;void   _tx_thread_interrupt_restore(UINT old_posture)
;{
    RSEG    .text:CODE:NOROOT(2)
    PUBLIC  _tx_thread_interrupt_restore
#ifdef THUMB_MODE
    THUMB
#else
    ARM
#endif
_tx_thread_interrupt_restore

;   /* Apply the new interrupt posture.  */

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

    BX      lr                                  ; Return to caller
;}
;
    END

