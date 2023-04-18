/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

    .global     _tx_thread_system_stack_ptr
    .global     _tx_initialize_unused_memory
    .global     __RAM_segment_used_end__
    .global     _tx_timer_interrupt
    .global     __main
    .global     _vectors
    .global     __tx_NMIHandler                     // NMI
    .global     __tx_BadHandler                     // HardFault
    .global     __tx_SVCallHandler                  // SVCall
    .global     __tx_DBGHandler                     // Monitor
    .global     __tx_PendSVHandler                  // PendSV
    .global     __tx_SysTickHandler                 // SysTick
    .global     __tx_IntHandler                     // Int 0

SYSTEM_CLOCK      =   6000000
SYSTICK_CYCLES    =   ((SYSTEM_CLOCK / 100) -1)

    .text 32
    .align 4
    .syntax unified
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                          Cortex-Mx/GNU     */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021      Scott Larson            Initial Version 6.1.7         */
/*  01-31-2022      Scott Larson            Modified comment(s), added    */
/*                                            TX_NO_TIMER support,        */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_initialize_low_level(VOID)
// {
    .global  _tx_initialize_low_level
    .thumb_func
_tx_initialize_low_level:

    /* Ensure that interrupts are disabled.  */
    CPSID   i

    /* Set base of available memory to end of non-initialised RAM area.  */
    LDR     r0, =_tx_initialize_unused_memory       // Build address of unused memory pointer
    LDR     r1, =__RAM_segment_used_end__           // Build first free address
    ADD     r1, r1, #4                              //
    STR     r1, [r0]                                // Setup first unused memory pointer

    /* Setup Vector Table Offset Register.  */
    MOV     r0, #0xE000E000                         // Build address of NVIC registers
    LDR     r1, =_vectors                           // Pickup address of vector table
    STR     r1, [r0, #0xD08]                        // Set vector table address

    /* Set system stack pointer from vector value.  */
    LDR     r0, =_tx_thread_system_stack_ptr        // Build address of system stack pointer
    LDR     r1, =_vectors                           // Pickup address of vector table
    LDR     r1, [r1]                                // Pickup reset stack pointer
    STR     r1, [r0]                                // Save system stack pointer

#ifndef TX_NO_TIMER
    /* Configure SysTick.  */
    MOV     r0, #0xE000E000                         // Build address of NVIC registers
    LDR     r1, =SYSTICK_CYCLES
    STR     r1, [r0, #0x14]                         // Setup SysTick Reload Value
    MOV     r1, #0x7                                // Build SysTick Control Enable Value
    STR     r1, [r0, #0x10]                         // Setup SysTick Control
#endif

    /* Configure handler priorities.  */
    LDR     r1, =0x00000000                         // Rsrv, UsgF, BusF, MemM
    STR     r1, [r0, #0xD18]                        // Setup System Handlers 4-7 Priority Registers
    LDR     r1, =0xFF000000                         // SVCl, Rsrv, Rsrv, Rsrv
    STR     r1, [r0, #0xD1C]                        // Setup System Handlers 8-11 Priority Registers
                                                    // Note: SVC must be lowest priority, which is 0xFF
    LDR     r1, =0x40FF0000                         // SysT, PnSV, Rsrv, DbgM
    STR     r1, [r0, #0xD20]                        // Setup System Handlers 12-15 Priority Registers
                                                    // Note: PnSV must be lowest priority, which is 0xFF

    /* Return to caller.  */
    BX      lr
// }

/* Define shells for each of the unused vectors.  */
    .global  __tx_BadHandler
    .thumb_func
__tx_BadHandler:
    B       __tx_BadHandler

/* added to catch the hardfault */
    .global  __tx_HardfaultHandler
    .thumb_func
__tx_HardfaultHandler:
    B       __tx_HardfaultHandler

/* added to catch the SVC */
    .global  __tx_SVCallHandler
    .thumb_func
__tx_SVCallHandler:
    B       __tx_SVCallHandler

/* Generic interrupt handler template */
    .global  __tx_IntHandler
    .thumb_func
__tx_IntHandler:
// VOID InterruptHandler (VOID)
// {
    PUSH    {r0, lr}
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    BL      _tx_execution_isr_enter             // Call the ISR enter function
#endif
    /* Do interrupt handler work here */
    /* BL <your C Function>.... */
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    BL      _tx_execution_isr_exit              // Call the ISR exit function
#endif
    POP     {r0, lr}
    BX      lr
// }

#ifndef TX_NO_TIMER
/* System Tick timer interrupt handler */
    .global  __tx_SysTickHandler
    .global  SysTick_Handler
    .thumb_func
__tx_SysTickHandler:
    .thumb_func
SysTick_Handler:
// VOID SysTick_Handler (VOID)
// {
    PUSH    {r0, lr}
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    BL      _tx_execution_isr_enter             // Call the ISR enter function
#endif
    BL      _tx_timer_interrupt
#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    BL      _tx_execution_isr_exit              // Call the ISR exit function
#endif
    POP     {r0, lr}
    BX      lr
// }
#endif

/* NMI, DBG handlers */
    .global  __tx_NMIHandler
    .thumb_func
__tx_NMIHandler:
    B       __tx_NMIHandler

    .global  __tx_DBGHandler
    .thumb_func
__tx_DBGHandler:
    B       __tx_DBGHandler
