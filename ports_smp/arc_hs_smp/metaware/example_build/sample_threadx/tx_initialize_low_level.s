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
    .equ    IRQ_SELECT,     0x40B

;
;
;    /* Define section for placement after all linker allocated RAM memory. This
;       is used to calculate the first free address that is passed to 
;       tx_appication_define, soley for the ThreadX application's use.  */
;
    .section    ".free_memory","aw"
    .align  4
    .global     _tx_first_free_address
_tx_first_free_address:
    .space  4   
;
;
    .text
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_initialize_low_level                        SMP/ARC_HS/MetaWare */
;/*                                                            6.1         */
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
    .type   _tx_initialize_low_level, @function 
_tx_initialize_low_level:

;
;    /* Save the system stack pointer.  */
;    _tx_thread_system_stack_ptr[0] = (VOID_PTR) (sp);
;
    st      sp, [gp, _tx_thread_system_stack_ptr@sda]   ; Save system stack pointer
;
;
;    /* Pickup the first available memory address.  */
;
    mov     r0, _tx_first_free_address                  ; Pickup first free memory address
;
;    /* Save the first available memory address.  */
;    _tx_initialize_unused_memory =  (VOID_PTR) _end;
;
    st      r0, [gp, _tx_initialize_unused_memory@sda]
;
;
;    /* Done, return to caller.  */
;
    j_s.d   [blink]                                     ; Return to caller
    nop
;}
;
;
;    /* Define default vector table entries.   */
;
    .global _tx_memory_error        
_tx_memory_error:
    flag    1
    nop
    nop
    nop
    b       _tx_memory_error

    .global _tx_instruction_error   
_tx_instruction_error:
    flag    1
    nop
    nop
    nop
    b       _tx_instruction_error

    .global _tx_ev_machine_check    
_tx_ev_machine_check:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_machine_check

    .global _tx_ev_tblmiss_inst     
_tx_ev_tblmiss_inst:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_tblmiss_inst

    .global _tx_ev_tblmiss_data    
_tx_ev_tblmiss_data:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_tblmiss_data

    .global _tx_ev_protection_viol  
_tx_ev_protection_viol:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_protection_viol

    .global _tx_ev_privilege_viol   
_tx_ev_privilege_viol:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_privilege_viol

    .global _tx_ev_software_int     
_tx_ev_software_int:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_software_int

    .global _tx_ev_trap             
_tx_ev_trap:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_trap

    .global _tx_ev_extension        
_tx_ev_extension:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_extension

    .global _tx_ev_divide_by_zero   
_tx_ev_divide_by_zero:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_divide_by_zero

    .global _tx_ev_dc_error         
_tx_ev_dc_error:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_dc_error

    .global _tx_ev_maligned         
_tx_ev_maligned:
    flag    1
    nop
    nop
    nop
    b       _tx_ev_maligned

    .global _tx_unsued_0            
_tx_unsued_0:
    flag    1
    nop
    nop
    nop
    b       _tx_unsued_0

    .global _tx_unused_1            
_tx_unused_1:
    flag    1
    nop
    nop
    nop
    b       _tx_unused_1

    .global _tx_timer_0             
_tx_timer_0:
;
;    /* By default, setup Timer 0 as the ThreadX timer interrupt.  */
;
    sub     sp, sp, 160                     ; Allocate an interrupt stack frame
    st      blink, [sp, 16]                 ; Save blink (blink must be saved before _tx_thread_context_save)
    bl      _tx_thread_context_save         ; Save interrupt context
;
;    /* Call ThreadX timer interrupt processing.  */
;
    bl.d    _tx_timer_interrupt             ; Call ThreadX timer processing
    sub     sp, sp, 16                      ; Allocate stack space (delay slot)
    add     sp, sp, 16                      ; Recover stack space

;    /* Clear Timer 0 interrupt.  */
;
    lr      r0, [CONTROL0]					; Read control register
    and     r0, r0, 0xFFFFFFF7              ; Clear timer 0 interrupt bit
    sr      r0, [CONTROL0]                  ; Store control register
    ld      r0, [sp, 16]                    ; Recover r0
;
    b       _tx_thread_context_restore      ; Restore interrupt context
;    flag    1
;    nop
;    nop
;    nop
;    b       _tx_timer_0

    .global _tx_timer_1             
_tx_timer_1:
    flag    1
    nop
    nop
    nop
    b       _tx_timer_1

    .global _tx_undefined_0         
_tx_undefined_0:
    flag    1
    nop
    nop
    nop
    b       _tx_undefined_0

    .global _tx_smp_inter_core
_tx_smp_inter_core:
;
;    /* Define the inter-core interrupt.  */
;
    sub     sp, sp, 160                     ; Allocate an interrupt stack frame
    st      blink, [sp, 16]                 ; Save blink (blink must be saved before _tx_thread_context_save)
    bl      _tx_thread_context_save         ; Save interrupt context
;
;    /* Call ARC inter-core interrupt processing.  */
;
    bl.d    arc_ici_handler                 ; Call ARC inter-core processing
    sub     sp, sp, 16                      ; Allocate stack space (delay slot)
    add     sp, sp, 16                      ; Recover stack space
;
    b       _tx_thread_context_restore      ; Restore interrupt context
;    flag    1
;    nop
;    nop
;    nop
;    b       _tx_undefined_1

    .global _tx_undefined_2         
_tx_undefined_2:
    flag    1
    nop
    nop
    nop
    b       _tx_undefined_2

   .end
