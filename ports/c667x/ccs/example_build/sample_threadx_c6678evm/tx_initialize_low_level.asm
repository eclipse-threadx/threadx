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
;
;#define TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_initialize.h"
;#include "tx_thread.h"
;#include "tx_timer.h"
;

SP                      .set    B15
ADDRESS_MSK             .set    0xFFFFFFF0          ; Ensure 16-byte alignment

; Use Timer 8 as it's available for all DSP cores at C6678
; Timer 8 interrupt high event (TINT8H) ID is 67
; Assume DSP core clock 1000 MHz
; Timer is clocked at DSP core clock / 6 = 167 MHz
; Timer frequency will set to 100 Hz
TMR8_INTCTLSTAT_ADDR    .set    0x02280044          ; Timer 8 Interrupt Control and Status Register
INTCTLSTAT_VAL          .set    0x00010000          ; Enable TIMHI interrupt

EVTCLR2_ADDR            .set    0x01800048          ; Event Clear register 2 address
EVTCLR2_TMR8_VAL        .set    0x00000008          ; Clear event 67 - TINT8H

EVTMASK2_ADDR           .set    0x01800088          ; Event Mask register 2 address
EVTMASK2_TMR8_VAL       .set    0x00000008          ; Mask event 67 - TINT8H

INTMUX1_ADDR            .set    0x01800104          ; Interrupt Mux Register 1 address
INTMUX1_TMR8_VAL        .set    0x43                ; Tie in Event 67 (TINT8H) to INT4
;
;
    .global     _tx_thread_system_stack_ptr
    .global     _tx_initialize_unused_memory
    .global     _tx_thread_context_save
    .global     _tx_thread_context_restore
    .global     _tx_timer_interrupt


; External interrupt handlers - should be defined by user
    .global     tx_nmi_handler
    .global     tx_int5_handler
    .global     tx_int6_handler
    .global     tx_int7_handler
    .global     tx_int8_handler
    .global     tx_int9_handler
    .global     tx_int10_handler
    .global     tx_int11_handler
    .global     tx_int12_handler
    .global     tx_int13_handler
    .global     tx_int14_handler
    .global     tx_int15_handler

;
;
;/* Define the first available address in memory, which is typically just the last
;   RAM section loaded.  */
    .sect   ".zend"
    .space  20
    .global _tx_first_free_memory
    .align  16
_tx_first_free_memory:
    .space  4        

; Useful macro definitions
; Load 32-bit integer into register
MVK_LH               .macro  val,reg
        MVKL        val,reg
        MVKH        val,reg
                     .endm
; Interrupt entry - allocate stack space, save A0-A4 and B3 registers to stack,
; build return address in B3 register for context save function and
; call context save function
TX_INTERRUPT_ENTRY    .macro
        ADDK.S2     -288,SP
        STW         B3,*+SP(96)
        STW         A0,*+SP(20)
        STW         A1,*+SP(24)
        STW         A2,*+SP(28)
        STW         A3,*+SP(32)
        STW         A4,*+SP(36)
        B           _tx_thread_context_save
                     .endm
; Interrupt exit - jump to context restore function
TX_INTERRUPT_EXIT    .macro
        B           _tx_thread_context_restore
        NOP         5
                     .endm


    .sect   ".text"
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_initialize_low_level                            C667x/TI        */ 
;/*                                                           6.1          */
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
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_initialize_low_level(VOID)
;{
    .global _tx_initialize_low_level
_tx_initialize_low_level:
;
;    /* Save the system stack pointer.  */
;    _tx_thread_system_stack_ptr = (VOID_PTR) (SP);
;
        MVK_LH      _tx_thread_system_stack_ptr,A0     ; Build address of system stack
        STW         SP,*A0                              ; Save system stack address
;
;    /* Pickup the first available memory address.  */
;
        MVK_LH      ADDRESS_MSK,A0                      ; Build address mask
        MVK_LH      _tx_first_free_memory,A1           ; Build address of free memory
        AND         A0,A1,A1                            ; Ensure alignment
;
;    /* Save the first available memory address.  */
;    _tx_initialize_unused_memory =  (VOID_PTR) end;
;
        MVK_LH      _tx_initialize_unused_memory,A0    ; Build address of variable
        STW         A1,*A0                              ; Save free memory address
;
;    /* Setup Timer 8 HIGH interrupt event */
;
        MVK_LH      INTMUX1_ADDR,A0                     ; Select event 67 to send through INT4
        MVK_LH      INTMUX1_TMR8_VAL,A1
        STW         A1,*A0

        MVK_LH      EVTMASK2_ADDR,A0                    ; Build address of Event Mask 2 Register
        MVK_LH      EVTMASK2_TMR8_VAL,A1                ; Build value of Event Mask 2 Register
        STW         A1,*A0
;
;    /* Done, return to caller.  */
;
        B           B3                                  ; Return to caller
        NOP         5                                   ; Delay slots
;}
;


    .global _tx_nmi_vector
    .global _tx_nmi_vector_processing
_tx_nmi_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_nmi_vector_processing,B3
        MVKH        _tx_nmi_vector_processing,B3
        NOP         3
_tx_nmi_vector_processing:
        CALLP       tx_nmi_handler,B3               ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int4_vector
_tx_int4_vector:
        ADDK.S2     -288,SP                         ; Allocate stack space
        STW         B3,*+SP(96)                     ; Save B3
        STW         A0,*+SP(20)                     ; Save A0
        STW         A1,*+SP(24)                     ; Save A1
        STW         A2,*+SP(28)                     ; Save A2
        STW         A3,*+SP(32)                     ; Save A3
        STW         A4,*+SP(36)                     ; Save A4
        NOP

;_tx_timer_interrupt_preamble:    

        MVK_LH      TMR8_INTCTLSTAT_ADDR,A0             ; Build address of Timer Interrupt Control Register
        MVK_LH      INTCTLSTAT_VAL,A1                   ; Build value of Timer Interrupt Control Register
        STW         A1,*A0                              ; Clear Timer Interrupts

; Clear DSP Event flag - DSP events are not self-cleared
        MVK_LH      EVTCLR2_ADDR,A0                     ; Build address of Event Clear 2 Register
        MVK_LH      EVTCLR2_TMR8_VAL,A1                 ; Build value of Event Clear 2 Register
        STW         A1,*A0                              ; Clear Timer Event

        MVK_LH      _tx_timer_interrupt,A0
        B           A0                                  ; Branch ThreadX timer ISR routine
        NOP         5                                   ; Delay slots
        NOP
        


    .global _tx_int5_vector
    .global _tx_int5_vector_processing
_tx_int5_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int5_vector_processing,B3
        MVKH        _tx_int5_vector_processing,B3
        NOP         3
_tx_int5_vector_processing:
        CALLP       tx_int5_handler,B3              ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int6_vector
    .global _tx_int6_vector_processing
_tx_int6_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int6_vector_processing,B3
        MVKH        _tx_int6_vector_processing,B3
        NOP         3
_tx_int6_vector_processing:
        CALLP       tx_int6_handler,B3              ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int7_vector
    .global _tx_int7_vector_processing
_tx_int7_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int7_vector_processing,B3
        MVKH        _tx_int7_vector_processing,B3
        NOP         3
_tx_int7_vector_processing:
        CALLP       tx_int7_handler,B3              ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int8_vector
    .global _tx_int8_vector_processing
_tx_int8_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int8_vector_processing,B3
        MVKH        _tx_int8_vector_processing,B3
        NOP         3
_tx_int8_vector_processing:
        CALLP       tx_int8_handler,B3              ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int9_vector
    .global _tx_int9_vector_processing
_tx_int9_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int9_vector_processing,B3
        MVKH        _tx_int9_vector_processing,B3
        NOP         3
_tx_int9_vector_processing:
        CALLP       tx_int9_handler,B3              ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int10_vector
    .global _tx_int10_vector_processing
_tx_int10_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int10_vector_processing,B3
        MVKH        _tx_int10_vector_processing,B3
        NOP         3
_tx_int10_vector_processing:
        CALLP       tx_int10_handler,B3             ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int11_vector
    .global _tx_int11_vector_processing
_tx_int11_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int11_vector_processing,B3
        MVKH        _tx_int11_vector_processing,B3
        NOP         3
_tx_int11_vector_processing:
        CALLP       tx_int11_handler,B3             ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int12_vector
    .global _tx_int12_vector_processing
_tx_int12_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int12_vector_processing,B3
        MVKH        _tx_int12_vector_processing,B3
        NOP         3
_tx_int12_vector_processing:
        CALLP       tx_int12_handler,B3             ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int13_vector
    .global _tx_int13_vector_processing
_tx_int13_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int13_vector_processing,B3
        MVKH        _tx_int13_vector_processing,B3
        NOP         3
_tx_int13_vector_processing:
        CALLP       tx_int13_handler,B3             ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int14_vector
    .global _tx_int14_vector_processing
_tx_int14_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int14_vector_processing,B3
        MVKH        _tx_int14_vector_processing,B3
        NOP         3
_tx_int14_vector_processing:
        CALLP       tx_int14_handler,B3             ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


    .global _tx_int15_vector
    .global _tx_int15_vector_processing
_tx_int15_vector:
        TX_INTERRUPT_ENTRY
;
;       /* Application specific processing goes here!  */
;
        MVKL        _tx_int15_vector_processing,B3
        MVKH        _tx_int15_vector_processing,B3
        NOP         3
_tx_int15_vector_processing:
        CALLP       tx_int15_handler,B3             ; CALLP instruction should be used here to call handler and save return address to B3
        TX_INTERRUPT_EXIT


