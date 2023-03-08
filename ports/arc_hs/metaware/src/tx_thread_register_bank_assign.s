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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_register_bank_assign                   ARC_HS/MetaWare   */
;/*                                                           6.2.1        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function builds a stack frame on the supplied thread's stack.  */
;/*    The stack frame results in a fake interrupt return to the supplied  */
;/*    function pointer.                                                   */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    thread_ptr                            Pointer to thread control blk */
;/*    register_bank                         Register bank number          */
;/*                                            (1 through max-1)           */ 
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
;/*    Application                                                         */
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*  03-08-2023     Cindy Deng               Modified comment(s), added    */
;/*                                            #include tx_user.h,         */
;/*                                            resulting in version 6.2.1  */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_register_bank_assign(VOID *thread_ptr, UINT register_bank)
;{
    .global _tx_thread_register_bank_assign
    .type   _tx_thread_register_bank_assign, @function
_tx_thread_register_bank_assign:
;
;  /* Assume this routine is being called from initialization, with interrupts
;     disabled and from register bank 0. Also assume that the thread pointer and
;     register bank input is valid, i.e., there is no error checking on the validity of
;     the thread pointer or the register_bank.  
;
;     It is worth noting that if fast interrupts are being used, register bank 1 
;     is reserved for the fast interrupt processing, so thread register bank assignments
;     should begin at bank 2.  */
;
    mov     ilink, r0                                   ; Move the thread control block into ilink
    asl     r2, r1, 16                                  ; Move the register bank bits over to proper location
    lr      r3, [status32]                              ; Pickup status32 register
    or      r3, r3, r2                                  ; Build new status32 register
    ld      r4, [r0, 8]                                 ; Pickup stack pointer for the thread
    ld      r5, [r4, 164]                               ; Pickup initial status32 from stack area
    or      r5, r5, r2                                  ; Modify initial status32 with register bank number
    st      r5, [r4, 164]                               ; Store initial status32 in stack area
    kflag   r3                                          ; Move to the hardware register bank
    mov     r0, ilink                                   ; Place thread control block in r0
    ld      sp, [r0, 8]                                 ; Setup stack pointer for this hardware register bank
    ld      fp, [sp, 24]                                ; Setup fp
    ld      gp, [sp, 28]                                ; Setup gp
    ld      blink, [sp, 16]                             ; Setup blink
    ld      ilink, [sp, 20]                             ; Setup ilink
    lr      r3, [status32]                              ; Pickup status32 register
    bclr    r3, r3, 16                                  ; Build register bank 0 value
    bclr    r3, r3, 17                                  ;
    bclr    r3, r3, 18                                  ;
    kflag   r3                                          ; Move back to register bank 0
    mov     r5, 3                                       ; Build type for hardware interrupt context 
    j_s.d   [blink]                                     ; Return to caller
    st      r5, [r4, 0]                                 ; Set stack frame type
;}
    .end
