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
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_interrupt_restore                        ARM11/IAR       */ 
;/*                                                           6.0.1        */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Express Logic, Inc.                               */ 
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
;/*  06-30-2020     William E. Lamie         Initial Version 6.0.1         */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;void   _tx_thread_interrupt_restore(UINT old_posture)
;{
    RSEG    .text:CODE:NOROOT(2)
    PUBLIC  _tx_thread_interrupt_restore
    CODE32
_tx_thread_interrupt_restore
;
;    /* Apply the new interrupt posture.  */
;
    MSR     CPSR_cxsf, r0                       ; Setup new CPSR
#ifdef TX_THUMB
    BX      lr                                  ; Return to caller
#else
    MOV     pc, lr                              ; Return to caller
#endif
;}
;
    END
