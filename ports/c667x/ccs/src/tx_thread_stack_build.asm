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
FP          .set    A15
DP          .set    B14
SP          .set    B15
ADDRESS_MSK .set    0xFFFFFFF0
;
    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_thread_stack_build                              C667x/TI        */ 
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
_tx_thread_stack_build:
;
;       
;    /* Build a fake interrupt frame.  The form of the fake interrupt stack
;       on the C667x should look like the following after it is built:
;       
;       Stack Top:      N/A         Available for use
;                       1           Interrupt stack frame type          4
;                       CSR         Initial value for CSR               8
;                       IRP         Initial thread entry                12
;                       AMR         Initial thread addressing mode      16
;                       A0          Initial A0                          20
;                       A1          Initial A1                          24
;                       A2          Initial A2                          28
;                       A3          Initial A3                          32
;                       A4          Initial A4                          36
;                       A5          Initial A5                          40
;                       A6          Initial A6                          44
;                       A7          Initial A7                          48
;                       A8          Initial A8                          52
;                       A9          Initial A9                          56
;                       A10         Initial A10                         60
;                       A11         Initial A11                         64
;                       A12         Initial A12                         68
;                       A13         Initial A13                         72
;                       A14         Initial A14                         76
;                       A15 (FP)    Initial A15 (FP)                    80
;                       B0          Initial B0                          84
;                       B1          Initial B1                          88
;                       B2          Initial B2                          92
;                       B3          Initial B3                          96
;                       B4          Initial B4                          100
;                       B5          Initial B5                          104
;                       B6          Initial B6                          108
;                       B7          Initial B7                          112
;                       B8          Initial B8                          116
;                       B9          Initial B9                          120
;                       B10         Initial B10                         124
;                       B11         Initial B11                         128
;                       B12         Initial B12                         132
;                       B13         Initial B13                         136
;                       A16         Initial A16                         140
;                       A17         Initial A17                         144
;                       A18         Initial A18                         148
;                       A19         Initial A19                         152
;                       A20         Initial A20                         156
;                       A21         Initial A21                         160
;                       A22         Initial A22                         164
;                       A23         Initial A23                         168
;                       A24         Initial A24                         172
;                       A25         Initial A25                         176
;                       A26         Initial A26                         180
;                       A27         Initial A27                         184
;                       A28         Initial A28                         188
;                       A29         Initial A29                         192
;                       A30         Initial A30                         196
;                       A31         Initial A31                         200
;                       B16         Initial B16                         204
;                       B17         Initial B17                         208
;                       B18         Initial B18                         212
;                       B19         Initial B19                         216
;                       B20         Initial B20                         220
;                       B21         Initial B21                         224
;                       B22         Initial B22                         228
;                       B23         Initial B23                         232
;                       B24         Initial B24                         236
;                       B25         Initial B25                         240
;                       B26         Initial B26                         244
;                       B27         Initial B27                         248
;                       B28         Initial B28                         252
;                       B29         Initial B29                         256
;                       B30         Initial B30                         260
;                       B31         Initial B31                         264
;                       ILC         Initial ILC                         268
;                       RILC        Initial RILC                        272
;                       ITSR        Initial ITSR                        276


;
;    Stack Bottom: (higher memory address)  */
;
        LDW         *+A4(16),A0                         ; Pickup end of stack area
        MVKL        ADDRESS_MSK,A1                      ; Build address mask
        MVKH        ADDRESS_MSK,A1                      ; 
        MVC         CSR,B0                              ; Pickup current CSR
        AND         -2,B0,B0                            ; Clear GIE bit
        OR          2,B0,B0                             ; Set PGIE bit for interrupt return
        AND         A1,A0,A0                            ; Ensure alignment
        MVKL        288,A2                              ; Calculate stack size
        SUB         A0,A2,A0                            ; Allocate space on thread's stack
;
;    /* Actually build the stack frame.  */
;
        MVKL        1,A2                                ; Build stack type
        ZERO        A3                                  ; Clear value 
        STW         A2,*+A0(4)                          ; Interrupt stack type
        STW         B0,*+A0(8)                          ; Initial CSR
        STW         B4,*+A0(12)                         ; Thread shell entry point
        STW         A3,*+A0(16)                         ; Initial AMR
        STW         A3,*+A0(20)                         ; Initial A0
        STW         A3,*+A0(24)                         ; Initial A1
        STW         A3,*+A0(28)                         ; Initial A2
        STW         A3,*+A0(32)                         ; Initial A3
        STW         A3,*+A0(36)                         ; Initial A4
        STW         A3,*+A0(40)                         ; Initial A5
        STW         A3,*+A0(44)                         ; Initial A6
        STW         A3,*+A0(48)                         ; Initial A7
        STW         A3,*+A0(52)                         ; Initial A8
        STW         A3,*+A0(56)                         ; Initial A9
        STW         A3,*+A0(60)                         ; Initial A10
        STW         A3,*+A0(64)                         ; Initial A11
        STW         A3,*+A0(68)                         ; Initial A12
        STW         A3,*+A0(72)                         ; Initial A13
        STW         A3,*+A0(76)                         ; Initial A14
        STW         A3,*+A0(80)                         ; Initial A15
        STW         A3,*+A0(84)                         ; Initial B0
        STW         A3,*+A0(88)                         ; Initial B1
        STW         A3,*+A0(92)                         ; Initial B2
        STW         A3,*+A0(96)                         ; Initial B3
        STW         A3,*+A0(100)                        ; Initial B4
        STW         A3,*+A0(104)                        ; Initial B5
        STW         A3,*+A0(108)                        ; Initial B6
        STW         A3,*+A0(112)                        ; Initial B7
        STW         A3,*+A0(116)                        ; Initial B8
        STW         A3,*+A0(120)                        ; Initial B9
        STW         A3,*+A0(124)                        ; Initial B10
        MVKL        128,A2                              ; Stack adjustment value
        ADD         A2,A0,A2                            ; Adjust pointer into stack frame
        STW         A3,*+A2(0)                          ; Initial B11
        STW         A3,*+A2(4)                          ; Initial B12
        STW         A3,*+A2(8)                          ; Initial B13
        STW         A3,*+A2(12)                         ; Initial A16
        STW         A3,*+A2(16)                         ; Initial A17
        STW         A3,*+A2(20)                         ; Initial A18
        STW         A3,*+A2(24)                         ; Initial A19
        STW         A3,*+A2(28)                         ; Initial A20
        STW         A3,*+A2(32)                         ; Initial A21
        STW         A3,*+A2(36)                         ; Initial A22
        STW         A3,*+A2(40)                         ; Initial A23
        STW         A3,*+A2(44)                         ; Initial A24
        STW         A3,*+A2(48)                         ; Initial A25
        STW         A3,*+A2(52)                         ; Initial A26
        STW         A3,*+A2(56)                         ; Initial A27
        STW         A3,*+A2(60)                         ; Initial A28
        STW         A3,*+A2(64)                         ; Initial A29
        STW         A3,*+A2(68)                         ; Initial A30
        STW         A3,*+A2(72)                         ; Initial A31
        STW         A3,*+A2(76)                         ; Initial B16
        STW         A3,*+A2(80)                         ; Initial B17
        STW         A3,*+A2(84)                         ; Initial B18
        STW         A3,*+A2(88)                         ; Initial B19
        STW         A3,*+A2(92)                         ; Initial B20
        STW         A3,*+A2(96)                         ; Initial B21
        STW         A3,*+A2(100)                        ; Initial B22
        STW         A3,*+A2(104)                        ; Initial B23
        STW         A3,*+A2(108)                        ; Initial B24
        STW         A3,*+A2(112)                        ; Initial B25
        STW         A3,*+A2(116)                        ; Initial B26
        STW         A3,*+A2(120)                        ; Initial B27
        STW         A3,*+A2(124)                        ; Initial B28
        ADDK        128,A2                              ; Adjust stack pointer again
        STW         A3,*+A2(0)                          ; Initial B29
        STW         A3,*+A2(4)                          ; Initial B30
        STW         A3,*+A2(8)                          ; Initial B31
        B           B3                                  ; Return to caller
        STW         A3,*+A2(12)                         ; Initial ILC
        STW         A3,*+A2(16)                         ; Initial RILC
        MVKL        0x3,B0                              ; Build initial ITSR (set GIE and SGIE bits)
        STW         B0,*+A2(20)                         ; Store ITSR
;
;    /* Setup stack pointer.  */
;    thread_ptr -> tx_thread_stack_ptr =  A0;
;
        STW         A0,*+A4(8)                          ; Save stack pointer in thread's
                                                        ;   control block
;}

