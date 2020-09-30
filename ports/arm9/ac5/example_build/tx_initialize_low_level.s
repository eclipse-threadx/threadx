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
;
SVC_MODE        EQU     0xD3                    ; Disable IRQ/FIQ SVC mode
IRQ_MODE        EQU     0xD2                    ; Disable IRQ/FIQ IRQ mode
FIQ_MODE        EQU     0xD1                    ; Disable IRQ/FIQ FIQ mode
SYS_MODE        EQU     0xDF                    ; Disable IRQ/FIQ SYS mode
HEAP_SIZE       EQU     4096                    ; Heap size
FIQ_STACK_SIZE  EQU     512                     ; FIQ stack size
SYS_STACK_SIZE  EQU     1024                    ; SYS stack size (used for nested interrupts)
IRQ_STACK_SIZE  EQU     1024                    ; IRQ stack size
;
;
;/* ARM9 ARMulator Timer and Interrupt controller information.  This depends on 
;   the ARMulator's Interrupt Controller and Timer being enabled in the default.ami.
;   In addition, the addresses must match those specified in the peripherals.ami file.
;   Please refer to section 2.10 and 4.16 of the Debug Target Guide, version 1.2.  */
;
IRQStatus       EQU     0x0a000000              ; IRQ Status Register
IRQRawStatus    EQU     0x0a000004              ; IRQ Raw Status Register
IRQEnable       EQU     0x0a000008              ; IRQ Enable Set Register
IRQEnableClear  EQU     0x0a00000C              ; IRQ Enable Clear Register
IRQSoft         EQU     0x0a000010              ; IRQ Soft 
FIQStatus       EQU     0x0a000100              ; FIQ Status Register
FIQRawStatus    EQU     0x0a000104              ; FIQ Raw Status Register
FIQEnable       EQU     0x0a000108              ; FIQ Enable Set Register
FIQEnableClear  EQU     0x0a00010C              ; FIQ Enable Clear Register

TIMER1_BIT      EQU     0x00000010              ; IRQ/FIQ Timer1 bit
TIMER2_BIT      EQU     0x00000020              ; IRQ/FIQ Timer2 bit

Timer1Load      EQU     0x0a800000              ; Timer1 Load Register
Timer1Value     EQU     0x0a800004              ; Timer1 Value Register
Timer1Control   EQU     0x0a800008              ; Timer1 Control Register
Timer1Clear     EQU     0x0a80000C              ; Timer1 Clear Register

Timer1Mode      EQU     0x000000C0              ; Timer1 Control Value, Timer enable, periodic, no prescaler
Timer1Period    EQU     0x0000FFFF              ; Timer1 count-down period, maximum value

Timer2Load      EQU     0x0a800020              ; Timer2 Load Register
Timer2Value     EQU     0x0a800024              ; Timer2 Value Register
Timer2Control   EQU     0x0a800028              ; Timer2 Control Register
Timer2Clear     EQU     0x0a80002C              ; Timer2 Clear Register
;
;
    IMPORT      _tx_thread_system_stack_ptr
    IMPORT      _tx_initialize_unused_memory
    IMPORT      _tx_thread_context_save
    IMPORT      _tx_thread_context_restore
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
    IMPORT      _tx_thread_fiq_context_save
    IMPORT      _tx_thread_fiq_context_restore
    ENDIF
    IF  :DEF:TX_ENABLE_IRQ_NESTING
    IMPORT      _tx_thread_irq_nesting_start
    IMPORT      _tx_thread_irq_nesting_end
    ENDIF
    IF  :DEF:TX_ENABLE_FIQ_NESTING
    IMPORT      _tx_thread_fiq_nesting_start
    IMPORT      _tx_thread_fiq_nesting_end
    ENDIF
    IMPORT      _tx_timer_interrupt
    IMPORT      __main
    IMPORT      _tx_version_id
    IMPORT      _tx_build_options
    IMPORT      |Image$$ZI$$Limit|
;
;
        AREA  Init, CODE, READONLY
;
;/* Define the ARM9 vector area.  This should be located or copied to 0.  */
;
    EXPORT  __vectors
__vectors
    LDR     pc,=__main                          ; Reset goes to startup function
    LDR     pc,=__tx_undefined                  ; Undefined handler
    LDR     pc,=__tx_swi_interrupt              ; Software interrupt handler
    LDR     pc,=__tx_prefetch_handler           ; Prefetch exception handler
    LDR     pc,=__tx_abort_handler              ; Abort exception handler
    LDR     pc,=__tx_reserved_handler           ; Reserved exception handler
    LDR     pc,=__tx_irq_handler                ; IRQ interrupt handler
    LDR     pc,=__tx_fiq_handler                ; FIQ interrupt handler
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_initialize_low_level                             ARM9/AC5       */ 
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
    EXPORT  _tx_initialize_low_level
_tx_initialize_low_level
;
;
;    /****** NOTE ****** We must be in SVC MODE at this point.  Some monitors
;       enter this routine in USER mode and require a software interrupt to
;       change into SVC mode.  */
;
    LDR     r1, =|Image$$ZI$$Limit|             ; Get end of non-initialized RAM area
    LDR     r2, =HEAP_SIZE                      ; Pickup the heap size
    ADD     r1, r2, r1                          ; Setup heap limit
    ADD     r1, r1, #4                          ; Setup stack limit
;
    IF :DEF:TX_ENABLE_IRQ_NESTING
;    /* Setup the system mode stack for nested interrupt support  */
    LDR     r2, =SYS_STACK_SIZE                 ; Pickup stack size
    MOV     r3, #SYS_MODE                       ; Build SYS mode CPSR
    MSR     CPSR_cxsf, r3                       ; Enter SYS mode
    ADD     r1, r1, r2                          ; Calculate start of SYS stack
    BIC     r1, r1, #7                          ; Ensure 8-byte alignment
    MOV     sp, r1                              ; Setup SYS stack pointer
    ENDIF
;
    LDR     r2, =FIQ_STACK_SIZE                 ; Pickup stack size
    MOV     r0, #FIQ_MODE                       ; Build FIQ mode CPSR
    MSR     CPSR_c, r0                          ; Enter FIQ mode
    ADD     r1, r1, r2                          ; Calculate start of FIQ stack
    BIC     r1, r1, #7                          ; Ensure 8-byte alignment
    MOV     sp, r1                              ; Setup FIQ stack pointer
    MOV     sl, #0                              ; Clear sl
    MOV     fp, #0                              ; Clear fp
    LDR     r2, =IRQ_STACK_SIZE                 ; Pickup IRQ (system stack size)
    MOV     r0, #IRQ_MODE                       ; Build IRQ mode CPSR
    MSR     CPSR_c, r0                          ; Enter IRQ mode
    ADD     r1, r1, r2                          ; Calculate start of IRQ stack
    BIC     r1, r1, #7                          ; Ensure 8-byte alignment
    MOV     sp, r1                              ; Setup IRQ stack pointer
    MOV     r0, #SVC_MODE                       ; Build SVC mode CPSR
    MSR     CPSR_c, r0                          ; Enter SVC mode
    LDR     r3, =_tx_thread_system_stack_ptr    ; Pickup stack pointer
    STR     r1, [r3, #0]                        ; Save the system stack
;
;    /* Save the system stack pointer.  */
;    _tx_thread_system_stack_ptr = (VOID_PTR) (sp);
;
    LDR     r1, =_tx_thread_system_stack_ptr    ; Pickup address of system stack ptr
    LDR     r0, [r1, #0]                        ; Pickup system stack 
    ADD     r0, r0, #4                          ; Increment to next free word
;
;    /* Save the first available memory address.  */
;    _tx_initialize_unused_memory =  (VOID_PTR) |Image$$ZI$$Limit| + HEAP + [SYS_STACK] + FIQ_STACK + IRQ_STACK;
;
    LDR     r2, =_tx_initialize_unused_memory   ; Pickup unused memory ptr address
    STR     r0, [r2, #0]                        ; Save first free memory address
;
;    /* Setup Timer for periodic interrupts.  */
;
;    /* Setup ARMulator Timer1 for periodic interrupts.  */
;
    LDR     r0,=IRQEnable                       ; Build address of IRQ enable register
    LDR     r1,=TIMER1_BIT                      ; Build value of Timer1 IRQ enable
    STR     r1,[r0]                             ; Enable IRQ interrupts for Timer1

    LDR     r0,=Timer1Load                      ; Build address of Timer1 load register
    LDR     r1,=Timer1Period                    ; Build Timer1 periodic value
    STR     r1,[r0]                             ; Set Timer1 load value

    LDR     r0,=Timer1Control                   ; Build address of Timer1 control register
    LDR     r1,=Timer1Mode                      ; Build Timer1 control value
    STR     r1,[r0]                             ; Enable Timer1
;
;    /* Done, return to caller.  */
;
    IF  {INTER} = {TRUE}
    BX      lr                                  ; Return to caller
    ELSE
    MOV     pc, lr                              ; Return to caller
    ENDIF
;}
;
;
;/* Define initial heap/stack routine for the ARM RealView (and ADS) startup code.  This
;   routine will set the initial stack to use the ThreadX IRQ & FIQ & 
;   (optionally SYS) stack areas.  */
;
    EXPORT  __user_initial_stackheap
__user_initial_stackheap
    LDR     r0, =|Image$$ZI$$Limit|             ; Get end of non-initialized RAM area
    LDR     r2, =HEAP_SIZE                      ; Pickup the heap size
    ADD     r2, r2, r0                          ; Setup heap limit
    ADD     r3, r2, #4                          ; Setup stack limit
    MOV     r1, r3                              ; Setup start of stack
    IF :DEF:TX_ENABLE_IRQ_NESTING
    LDR     r12, =SYS_STACK_SIZE                ; Pickup IRQ system stack
    ADD     r1, r1, r12                         ; Setup the return system stack
    BIC     r1, r1, #7                          ; Ensure 8-byte alignment
    ENDIF
    LDR     r12, =FIQ_STACK_SIZE                ; Pickup FIQ stack size
    ADD     r1, r1, r12                         ; Setup the return system stack
    BIC     r1, r1, #7                          ; Ensure 8-byte alignment
    LDR     r12, =IRQ_STACK_SIZE                ; Pickup IRQ system stack
    ADD     r1, r1, r12                         ; Setup the return system stack
    BIC     r1, r1, #7                          ; Ensure 8-byte alignment
    IF  {INTER} = {TRUE}
    BX      lr                                  ; Return to caller
    ELSE
    MOV     pc, lr                              ; Return to caller
    ENDIF
;
;
;/* Define shells for each of the interrupt vectors.  */
;
    EXPORT  __tx_undefined
__tx_undefined
    B       __tx_undefined                      ; Undefined handler
;
    EXPORT  __tx_swi_interrupt
__tx_swi_interrupt
    B       __tx_swi_interrupt                  ; Software interrupt handler
;
    EXPORT  __tx_prefetch_handler
__tx_prefetch_handler
    B       __tx_prefetch_handler               ; Prefetch exception handler
;
    EXPORT  __tx_abort_handler
__tx_abort_handler
    B       __tx_abort_handler                  ; Abort exception handler
;
    EXPORT  __tx_reserved_handler
__tx_reserved_handler
    B       __tx_reserved_handler               ; Reserved exception handler
;
;
    EXPORT  __tx_irq_handler
    EXPORT  __tx_irq_processing_return      
__tx_irq_handler
;
;    /* Jump to context save to save system context.  */
    B       _tx_thread_context_save
__tx_irq_processing_return
;
;    /* At this point execution is still in the IRQ mode.  The CPSR, point of
;       interrupt, and all C scratch registers are available for use.  In 
;       addition, IRQ interrupts may be re-enabled - with certain restrictions -
;       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
;       small code sequences where lr is saved before enabling interrupts and 
;       restored after interrupts are again disabled.  */
;
;
;    /* Check for Timer1 interrupts on the ARMulator.  */

    LDR     r1,=IRQStatus                       ; Pickup address of IRQStatus register
    LDR     r2, [r1]                            ; Read IRQStatus 
    LDR     r0,=TIMER1_BIT                      ; Pickup Timer1 interrupt present bit
    AND     r2, r2, r0                          ; Is this a timer interrupt?
    CMP     r2, r0                              ; 
    BNE     _tx_not_timer_interrupt             ; If 0, not a timer interrupt

    LDR     r1,=Timer1Clear                     ; Build address of Timer1 clear register
    MOV     r0,#0                               ; 
    STR     r0, [r1]                            ; Clear timer 0 interrupt

    BL      _tx_timer_interrupt                 ; Timer interrupt handler
_tx_not_timer_interrupt
;
;    /* Interrupt nesting is allowed after calling _tx_thread_irq_nesting_start 
;       from IRQ mode with interrupts disabled.  This routine switches to the
;       system mode and returns with IRQ interrupts enabled.  
;       
;       NOTE:  It is very important to ensure all IRQ interrupts are cleared 
;       prior to enabling nested IRQ interrupts.  */
    IF :DEF:TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_start
    ENDIF
;
;
;    /* Application IRQ handlers can be called here!  */
;
;    /* If interrupt nesting was started earlier, the end of interrupt nesting
;       service must be called before returning to _tx_thread_context_restore.  
;       This routine returns in processing in IRQ mode with interrupts disabled.  */
    IF :DEF:TX_ENABLE_IRQ_NESTING
    BL      _tx_thread_irq_nesting_end
    ENDIF
;
;    /* Jump to context restore to restore system context.  */
    B       _tx_thread_context_restore
;
;
;    /* This is an example of a vectored IRQ handler.  */
;
    EXPORT  __tx_example_vectored_irq_handler
__tx_example_vectored_irq_handler
;
;
;    /* Save initial context and call context save to prepare for 
;       vectored ISR execution.  */
;
;    STMDB   sp!, {r0-r3}                        ; Save some scratch registers
;    MRS     r0, SPSR                            ; Pickup saved SPSR
;    SUB     lr, lr, #4                          ; Adjust point of interrupt 
;    STMDB   sp!, {r0, r10, r12, lr}             ; Store other scratch registers
;    BL      _tx_thread_vectored_context_save    ; Vectored context save
;
;    /* At this point execution is still in the IRQ mode.  The CPSR, point of
;       interrupt, and all C scratch registers are available for use.  In 
;       addition, IRQ interrupts may be re-enabled - with certain restrictions -
;       if nested IRQ interrupts are desired.  Interrupts may be re-enabled over
;       small code sequences where lr is saved before enabling interrupts and 
;       restored after interrupts are again disabled.  */
;
;
;    /* Interrupt nesting is allowed after calling _tx_thread_irq_nesting_start 
;       from IRQ mode with interrupts disabled.  This routine switches to the
;       system mode and returns with IRQ interrupts enabled.  
;       
;       NOTE:  It is very important to ensure all IRQ interrupts are cleared 
;       prior to enabling nested IRQ interrupts.  */
;    IF :DEF:TX_ENABLE_IRQ_NESTING
;    BL      _tx_thread_irq_nesting_start
;    ENDIF
;
;    /* Application IRQ handlers can be called here!  */
;
;    /* If interrupt nesting was started earlier, the end of interrupt nesting
;       service must be called before returning to _tx_thread_context_restore.  
;       This routine returns in processing in IRQ mode with interrupts disabled.  */
;    IF :DEF:TX_ENABLE_IRQ_NESTING
;    BL      _tx_thread_irq_nesting_end
;    ENDIF
;
;    /* Jump to context restore to restore system context.  */
;    B       _tx_thread_context_restore
;
;
    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
    EXPORT  __tx_fiq_handler
    EXPORT  __tx_fiq_processing_return
__tx_fiq_handler
;
;    /* Jump to fiq context save to save system context.  */
    B       _tx_thread_fiq_context_save
__tx_fiq_processing_return
;
;    /* At this point execution is still in the FIQ mode.  The CPSR, point of
;       interrupt, and all C scratch registers are available for use.  */
;
;    /* Interrupt nesting is allowed after calling _tx_thread_fiq_nesting_start 
;       from FIQ mode with interrupts disabled.  This routine switches to the
;       system mode and returns with FIQ interrupts enabled. 
;
;       NOTE:  It is very important to ensure all FIQ interrupts are cleared 
;       prior to enabling nested FIQ interrupts.  */
    IF  :DEF:TX_ENABLE_FIQ_NESTING
    BL      _tx_thread_fiq_nesting_start
    ENDIF
;
;    /* Application FIQ handlers can be called here!  */
;
;    /* If interrupt nesting was started earlier, the end of interrupt nesting
;       service must be called before returning to _tx_thread_fiq_context_restore.  */
    IF  :DEF:TX_ENABLE_FIQ_NESTING
    BL      _tx_thread_fiq_nesting_end
    ENDIF
;
;    /* Jump to fiq context restore to restore system context.  */
    B       _tx_thread_fiq_context_restore
;
;
    ELSE
    EXPORT  __tx_fiq_handler
__tx_fiq_handler
    B       __tx_fiq_handler                    ; FIQ interrupt handler
    ENDIF
;
;    /* Reference build options and version ID to ensure they come in.  */
;
    LDR     r2, =_tx_build_options              ; Pickup build options variable address
    LDR     r0, [r2, #0]                        ; Pickup build options content
    LDR     r2, =_tx_version_id                 ; Pickup version ID variable address
    LDR     r0, [r2, #0]                        ; Pickup version ID content
;
;
    END

