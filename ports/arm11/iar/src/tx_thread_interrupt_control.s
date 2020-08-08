;/**************************************************************************/ 
;/*                                                                        */ 
;/*            Copyright (c) 1996-2018 by Express Logic Inc.               */ 
;/*                                                                        */ 
;/*  This software is copyrighted by and is the sole property of Express   */ 
;/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
;/*  in the software remain the property of Express Logic, Inc.  This      */ 
;/*  software may only be used in accordance with the corresponding        */ 
;/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
;/*  distribution, or disclosure of this software is expressly forbidden.  */ 
;/*                                                                        */
;/*  This Copyright notice may not be removed or modified without prior    */ 
;/*  written consent of Express Logic, Inc.                                */ 
;/*                                                                        */ 
;/*  Express Logic, Inc. reserves the right to modify this software        */ 
;/*  without notice.                                                       */ 
;/*                                                                        */ 
;/*  Express Logic, Inc.                     info@expresslogic.com         */
;/*  11423 West Bernardo Court               http://www.expresslogic.com   */
;/*  San Diego, CA  92127                                                  */
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
;
;#define TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_thread.h"
;
;
#ifdef TX_ENABLE_FIQ_SUPPORT
INT_MASK        DEFINE      0xC0                ; Interrupt bit mask
#else
INT_MASK        DEFINE      0x80                ; Interrupt bit mask
#endif
;
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_interrupt_control                        ARM11/IAR       */ 
;/*                                                           6.0.1        */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is responsible for changing the interrupt lockout     */ 
;/*    posture of the system.                                              */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    new_posture                           New interrupt lockout posture */ 
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
;/*  06-30-2020     William E. Lamie         Initial Version 6.0.1         */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;UINT   _tx_thread_interrupt_control(UINT new_posture)
;{
    RSEG    .text:CODE:NOROOT(2)
    PUBLIC  _tx_thread_interrupt_control
    CODE32
_tx_thread_interrupt_control
;
;    /* Pickup current interrupt lockout posture.  */
;
    MRS     r3, CPSR                            ; Pickup current CPSR
    BIC     r1, r3, #INT_MASK                   ; Clear interrupt lockout bits
    ORR     r1, r1, r0                          ; Or-in new interrupt lockout bits
;
;    /* Apply the new interrupt posture.  */
;
    MSR     CPSR_cxsf, r1                       ; Setup new CPSR
    AND     r0, r3, #INT_MASK                   ; Return previous interrupt mask
#ifdef TX_THUMB
    BX      lr                                  ; Return to caller
#else
    MOV     pc, lr                              ; Return to caller
#endif
;
;}
;
;
    END
