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
    EXTERN  _tx_thread_system_stack_ptr
    EXTERN  _tx_initialize_unused_memory
    EXTERN  _tx_timer_interrupt
    EXTERN  __main
    EXTERN  __vector_table
    EXTERN  _tx_thread_current_ptr
    EXTERN  _tx_thread_stack_error_handler
;
;
SYSTEM_CLOCK        EQU     96000000
SYSTICK_CYCLES      EQU     ((SYSTEM_CLOCK / 100) -1)
;
;
    RSEG    FREE_MEM:DATA
    PUBLIC  __tx_free_memory_start
__tx_free_memory_start
    DS32    4
;
;
    SECTION `.text`:CODE:NOROOT(2)
    THUMB

;/**************************************************************************/
;/*                                                                        */
;/*  FUNCTION                                               RELEASE        */
;/*                                                                        */
;/*    _tx_initialize_low_level                          Cortex-M33/IAR    */
;/*                                                           6.1          */
;/*  AUTHOR                                                                */
;/*                                                                        */
;/*    Scott Larson, Microsoft Corporation                                 */
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
;/*  09-30-2020      Scott Larson            Initial Version 6.1           */
;/*                                                                        */
;/**************************************************************************/
;VOID   _tx_initialize_low_level(VOID)
;{
    PUBLIC  _tx_initialize_low_level
_tx_initialize_low_level:
;
;    /* Disable interrupts during ThreadX initialization.  */
;
    CPSID   i
;
;    /* Set base of available memory to end of non-initialised RAM area.  */
;
    LDR     r0, =_tx_initialize_unused_memory       ; Build address of unused memory pointer
    LDR     r1, =__tx_free_memory_start             ; Build first free address
    STR     r1, [r0]                                ; Setup first unused memory pointer
;
;    /* Setup Vector Table Offset Register.  */
;
    MOV     r0, #0xE000E000                         ; Build address of NVIC registers
    LDR     r1, =__vector_table                     ; Pickup address of vector table
    STR     r1, [r0, #0xD08]                        ; Set vector table address
;
;    /* Enable the cycle count register.  */
;
;    LDR     r0, =0xE0001000                         ; Build address of DWT register
;    LDR     r1, [r0]                                ; Pickup the current value
;    ORR     r1, r1, #1                              ; Set the CYCCNTENA bit
;    STR     r1, [r0]                                ; Enable the cycle count register
;
;    /* Set system stack pointer from vector value.  */
;
    LDR     r0, =_tx_thread_system_stack_ptr        ; Build address of system stack pointer
    LDR     r1, =__vector_table                     ; Pickup address of vector table
    LDR     r1, [r1]                                ; Pickup reset stack pointer
    STR     r1, [r0]                                ; Save system stack pointer
;
;    /* Configure SysTick.  */
;
    MOV     r0, #0xE000E000                         ; Build address of NVIC registers
    LDR     r1, =SYSTICK_CYCLES
    STR     r1, [r0, #0x14]                         ; Setup SysTick Reload Value
    MOV     r1, #0x7                                ; Build SysTick Control Enable Value
    STR     r1, [r0, #0x10]                         ; Setup SysTick Control
;
;    /* Configure handler priorities.  */
;
    LDR     r1, =0x00000000                         ; Rsrv, UsgF, BusF, MemM
    STR     r1, [r0, #0xD18]                        ; Setup System Handlers 4-7 Priority Registers

    LDR     r1, =0xFF000000                         ; SVCl, Rsrv, Rsrv, Rsrv
    STR     r1, [r0, #0xD1C]                        ; Setup System Handlers 8-11 Priority Registers
                                                    ; Note: SVC must be lowest priority, which is 0xFF

    LDR     r1, =0x40FF0000                         ; SysT, PnSV, Rsrv, DbgM
    STR     r1, [r0, #0xD20]                        ; Setup System Handlers 12-15 Priority Registers
                                                    ; Note: PnSV must be lowest priority, which is 0xFF
;
;    /* Return to caller.  */
;
    BX      lr
;}
;
;
;/* Define shells for each of the unused vectors.  */
;
    PUBLIC  __tx_BadHandler
__tx_BadHandler:
    B       __tx_BadHandler


    PUBLIC  __tx_IntHandler
__tx_IntHandler:
; VOID InterruptHandler (VOID)
; {
    PUSH    {r0,lr}     ; Save LR (and dummy r0 to maintain stack alignment)

;    /* Do interrupt handler work here */
;    /* .... */

    POP     {r0,lr}
    BX      LR
; }


    PUBLIC  __tx_SysTickHandler
    PUBLIC SysTick_Handler
SysTick_Handler:
__tx_SysTickHandler:
; VOID TimerInterruptHandler (VOID)
; {
;
    PUSH    {r0,lr}     ; Save LR (and dummy r0 to maintain stack alignment)
    BL      _tx_timer_interrupt
    POP     {r0,lr}
    BX      LR
; }

    PUBLIC HardFault_Handler
HardFault_Handler:
    B       HardFault_Handler


    PUBLIC UsageFault_Handler
UsageFault_Handler:
    CPSID   i                                       ; Disable interrupts
    ; Check for stack limit fault
    LDR     r0, =0xE000ED28                         ; CFSR address
    LDR     r1,[r0]                                 ; Pick up CFSR
    TST     r1, #0x00100000                         ; Check for Stack Overflow
_unhandled_usage_loop
    BEQ     _unhandled_usage_loop                   ; If not stack overflow then loop

    ; Handle stack overflow
    STR     r1, [r0]                                ; Clear CFSR flag(s)

#ifdef __ARMVFP__
    LDR     r0, =0xE000EF34                         ; Cleanup FPU context: Load FPCCR address
    LDR     r1, [r0]                                ; Load FPCCR
    BIC     r1, r1, #1                              ; Clear the lazy preservation active bit
    STR     r1, [r0]                                ; Store the value
#endif

    MOV32   r0, _tx_thread_current_ptr              ; Build current thread pointer address
    LDR     r0,[r0]                                 ; Pick up current thread pointer
    PUSH    {r0,lr}                                 ; Save LR (and r0 to maintain stack alignment)
    BL      _tx_thread_stack_error_handler          ; Call ThreadX/user handler
    POP     {r0,lr}                                 ; Restore LR and dummy reg

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    ; Call the thread exit function to indicate the thread is no longer executing.
    PUSH    {r0, lr}                                ; Save LR (and r0 just for alignment)
    BL      _tx_execution_thread_exit               ; Call the thread exit function
    POP     {r0, lr}                                ; Recover LR
#endif

    MOV     r1, #0                                  ; Build NULL value
    LDR     r0, =_tx_thread_current_ptr             ; Pickup address of current thread pointer
    STR     r1, [r0]                                ; Clear current thread pointer

    ; Return from UsageFault_Handler exception
    LDR     r0, =0xE000ED04                         ; Load ICSR
    LDR     r1, =0x10000000                         ; Set PENDSVSET bit
    STR     r1, [r0]                                ; Store ICSR
    DSB                                             ; Wait for memory access to complete
    CPSIE   i                                       ; Enable interrupts
    BX      lr                                      ; Return from exception


    PUBLIC  __tx_NMIHandler
__tx_NMIHandler:
    B       __tx_NMIHandler


    PUBLIC  __tx_DBGHandler
__tx_DBGHandler:
    B       __tx_DBGHandler

    END
