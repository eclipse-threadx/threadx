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
;
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
    .equ    LONG_ALIGN_MASK, 0xFFFFFFFC     
    .equ    INT_ENABLE_BITS, 0x8000001E
;
;
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_stack_build                            ARC_HS/MetaWare   */
;/*                                                           6.1          */
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
;/*    function_ptr                          Pointer to return function    */
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
;/*    _tx_thread_create                     Create thread service         */
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID))
;{
    .global _tx_thread_stack_build
    .type   _tx_thread_stack_build, @function
_tx_thread_stack_build:
;
;       
;    /* Build a fake interrupt frame.  The form of the fake interrupt stack
;       on the ARC HS should look like the following after it is built.
;       Note that the extension registers are always assigned space here.
;       
;       Stack Top:      1           Interrupt stack frame type
;                       LP_START    Initial loop start
;                       LP_END      Initial loop end
;                       LP_COUNT    Initial loop count
;                       blink       Initial blink value
;                       ilink       Initial ilink (point of interrupt)
;                       fp (r27)    Initial fp (0)
;                       gp          Initial gp
;                       r25         Initial r25
;                       r24         Initial r24
;                       r23         Initial r23
;                       r22         Initial r22
;                       r21         Initial r21
;                       r20         Initial r20
;                       r19         Initial r19
;                       r18         Initial r18
;                       r17         Initial r17
;                       r16         Initial r16
;                       r15         Initial r15
;                       r14         Initial r14
;                       r13         Initial r13
;                       r12         Initial r12
;                       r11         Initial r11
;                       r10         Initial r10
;                       r9          Initial r9
;                       r8          Initial r8
;                       r7          Initial r7
;                       r6          Initial r6
;                       r5          Initial r5
;                       r4          Initial r4
;                       r3          Initial r3
;                       r2          Initial r2
;                       r1          Initial r1
;                       r0          Initial r0
;                       r30         Initial r30 
;                       r58         Initial r58
;                       r59         Initial r59
;                       0           Reserved
;                       0           Reserved
;                       0           Initial BTA
;                       0           Point of Interrupt (thread entry point)
;                       0           Initial STATUS32
;                       0           Backtrace 
;                       0           Backtrace 
;                       0           Backtrace 
;                       0           Backtrace 
;
; *: these registers will only be saved and restored if flag -Xxmac_d16 is passed to hcac
;
;    Stack Bottom: (higher memory address)  */
;
    ld      r3, [r0, 16]                                ; Pickup end of stack area
    and     r3, r3, LONG_ALIGN_MASK                     ; Ensure long-word alignment
    sub     r3, r3, 196                                 ; Allocate an interrupt stack frame (ARC HS)
;
;    /* Actually build the stack frame.  */
;
    st      1, [r3, 0]                                  ; Store interrupt stack type on the
                                                        ;   top of the stack
    mov     r5, 0                                       ; Build initial clear value
    st      r5, [r3, 4]                                 ; Store initial LP_START
    st      r5, [r3, 8]                                 ; Store initial LP_END
    st      r5, [r3, 12]                                ; Store initial LP_COUNT
    st      r5, [r3, 16]                                ; Store initial blink
    st      r1, [r3, 20]                                ; Store initial ilink
    st      r5, [r3, 24]                                ; Store initial fp (0 for backtrace)
    st      gp, [r3, 28]                                ; Store current gp
    st      r5, [r3, 32]                                ; Store initial r25
    st      r5, [r3, 36]                                ; Store initial r24
    st      r5, [r3, 40]                                ; Store initial r23
    st      r5, [r3, 44]                                ; Store initial r22
    st      r5, [r3, 48]                                ; Store initial r21
    st      r5, [r3, 52]                                ; Store initial r20
    st      r5, [r3, 56]                                ; Store initial r19
    st      r5, [r3, 60]                                ; Store initial r18
    st      r5, [r3, 64]                                ; Store initial r17
    st      r5, [r3, 68]                                ; Store initial r16
    st      r5, [r3, 72]                                ; Store initial r15
    st      r5, [r3, 76]                                ; Store initial r14
    st      r5, [r3, 80]                                ; Store initial r13
    st      r5, [r3, 84]                                ; Store initial r12
    st      r5, [r3, 88]                                ; Store initial r11
    st      r5, [r3, 92]                                ; Store initial r10
    st      r5, [r3, 96]                                ; Store initial r9
    st      r5, [r3, 100]                               ; Store initial r8
    st      r5, [r3, 104]                               ; Store initial r7
    st      r5, [r3, 108]                               ; Store initial r6
    st      r5, [r3, 112]                               ; Store initial r5
    st      r5, [r3, 116]                               ; Store initial r4
    st      r5, [r3, 120]                               ; Store initial r3
    st      r5, [r3, 124]                               ; Store initial r2
    st      r5, [r3, 128]                               ; Store initial r1
    st      r5, [r3, 132]                               ; Store initial r0
    st      r5, [r3, 136]                               ; Store initial r30
    st      r5, [r3, 140]                               ; Store initial r58 
    st      r5, [r3, 144]                               ; Store initial r59
    st      r5, [r3, 148]                               ; Reserved
    st      r5, [r3, 152]                               ; Reserved
    st      r5, [r3, 156]                               ; Store initial BTA
    st      r1, [r3, 160]                               ; Store initial point of entry 
    lr      r6, [status32]                              ; Pickup STATUS32
    or      r6, r6, INT_ENABLE_BITS                     ; Make sure interrupts are enabled
    st      r6, [r3, 164]                               ; Store initial STATUS32
    st      r5, [r3, 168]                               ; Backtrace 0 
    st      r5, [r3, 172]                               ; Backtrace 0 
    st      r5, [r3, 176]                               ; Backtrace 0 
    st      r5, [r3, 180]                               ; Backtrace 0 
;
;    /* Setup stack pointer.  */
;    thread_ptr -> tx_thread_stack_ptr =  r3;
;
    j_s.d   [blink]                                     ; Return to caller
    st      r3, [r0, 8]                                 ; Save stack pointer in thread's
                                                        ;   control block
;}
    .end


