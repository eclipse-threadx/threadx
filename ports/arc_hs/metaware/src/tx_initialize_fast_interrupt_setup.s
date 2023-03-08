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
;/**   Initialize                                                          */
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
;/*    _tx_initialize_fast_interrupt_setup               ARC_HS/MetaWare   */
;/*                                                           6.2.1        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */ 
;/*    This function initializes register bank 1 for fast interrupt use.   */ 
;/*    The initialization includes setting the stack pointer to the value  */ 
;/*    supplied by the caller.                                             */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    stack_ptr                             Pointer to stack for bank 1   */
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
;VOID   _tx_initialize_fast_interrupt_setup(VOID *stack_ptr)
;{
    .global _tx_initialize_fast_interrupt_setup
    .type   _tx_initialize_fast_interrupt_setup, @function
_tx_initialize_fast_interrupt_setup:
;
;  /* Assume this routine is being called from initialization, with interrupts
;     disabled and from register bank 0. Also assume that the stack pointer 
;     input is valid, i.e., there is no error checking on the validity of
;     register_bank.  */
;
    sub     sp, sp, 8                                   ; Build a small stack frame to hold the setup information
    st      gp, [sp, 0]                                 ; Save gp in the frame
    st      r0, [sp, 4]                                 ; Save sp in the frame
    mov     ilink, sp                                   ; Move the stack frame into ilink
    mov     r1, 1                                       ; Select register bank 1
    asl     r2, r1, 16                                  ; Move the register bank bits over to proper location
    lr      r3, [status32]                              ; Pickup status32 register
    or      r3, r3, r2                                  ; Build new status32 register
    kflag   r3                                          ; Move to the hardware register bank
    mov     r0, ilink                                   ; Place stack pointer in r0
    ld      sp, [r0, 4]                                 ; Setup stack pointer for this hardware register bank
    mov     fp, 0                                       ; Setup fp
    ld      gp, [r0, 0]                                 ; Setup gp
    mov     blink, 0                                    ; Setup blink
    mov     r0, 0                                       ; Clear r0
    sub     sp, sp, 8                                   ; Reserve space for saving ilink and status32.p0 on thread preemption
    lr      r3, [status32]                              ; Pickup status32 register
    bclr    r3, r3, 16                                  ; Build register bank 0 value
    bclr    r3, r3, 17                                  ;
    bclr    r3, r3, 18                                  ;
    kflag   r3                                          ; Move back to register bank 0
    j_s.d   [blink]                                     ; Return to caller
    add     sp, sp, 8                                   ;
;}
    .end
