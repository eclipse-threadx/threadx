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
;

    extern __tx_initialize_unused_memory

    section .text:CODE:ROOT

;
;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_initialize_low_level                             RXv2/IAR       */
;/*                                                           6.1.3        */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    William E. Lamie, Microsoft Corporation                             */
;/*                                                                        */
;/*  DESCRIPTION                                                           */
;/*                                                                        */
;/*    This function is responsible for any low-level processor            */
;/*    initialization, including setting up interrupt vectors, setting     */
;/*    up a periodic timer interrupt source, saving the system stack       */
;/*    pointer for use in ISR processing later, and finding the first      */
;/*    available RAM memory address for tx_application_define.             */
;/*                                                                        */
;/*  INPUT                                                                 */
;/*                                                                        */
;/*    None                                                                */
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
;/*    _tx_initialize_kernel_enter           ThreadX entry function        */
;/*                                                                        */
;/*  RELEASE HISTORY                                                       */
;/*                                                                        */
;/*    DATE              NAME                      DESCRIPTION             */
;/*                                                                        */
;/*  12-31-2020     William E. Lamie         Initial Version 6.1.3         */
;/*                                                                        */
;/**************************************************************************/
    public __tx_initialize_low_level

__tx_initialize_low_level:

;    /* Save the first available memory address.  */
;    _tx_initialize_unused_memory =  (VOID_PTR) &free_mem_start;
;
    MOV.L    #__tx_free_memory_start, R1        ; Pickup unused memory address
    MOV.L    #__tx_initialize_unused_memory,R2                
    MOV.L    R1,[R2]                            ; Save first free memory address

;   /* Set priority of SWINT to 1. */
    MOV.L    #0x87303, r1
    MOV.L    #1, r2
    MOV.B    r2, [r1]

;   /* Enable SWINT. */
    MOV.L    #0x87203, r1
    MOV.B    [r1], r2
    OR       #(1 << 3), r2
    MOV.B    r2, [r1]

    RTS

    section FREEMEM:DATA
    public __tx_free_memory_start
__tx_free_memory_start
    DS32    4

    END
