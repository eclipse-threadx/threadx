/***************************************************************************
 * Copyright (c) 2025 10xEngineers
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_initialize.h"
    #include "tx_thread.h"
    #include "tx_timer.h"  */
    
    .extern      _tx_thread_system_stack_ptr
    .extern      _tx_initialize_unused_memory
    .extern      _tx_thread_context_save
    .extern      _tx_thread_context_restore
    .extern      _tx_timer_interrupt

    .section .bss
    .balign    4
    .globl __tx_free_memory_start
__tx_free_memory_start:
    .space    4

    .section .text
    .balign    4
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                           RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
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
/**************************************************************************/

/* VOID   _tx_initialize_low_level(VOID)
{  */
    .globl _tx_initialize_low_level
_tx_initialize_low_level:
    la      t0, _tx_thread_system_stack_ptr 
    sw      sp, 0(t0)                               // Save system stack pointer

    la      t0, __tx_free_memory_start              // Pickup first free address
    la      t1, _tx_initialize_unused_memory
    sw      t0, 0(t1)                               // Save unused memory address

    ret


    /* Define the actual timer interrupt/exception handler.  */
    .globl _tx_timer_interrupt_handler
    .globl __minterrupt_000007
_tx_timer_interrupt_handler:
__minterrupt_000007:

    /* Before calling _tx_thread_context_save, we have to allocate an interrupt
       stack frame and save the current value of x1 (ra). */
#if defined(__riscv_32e) || defined(__riscv_32rve)
    addi    sp, sp, -260                            // Allocate space for all registers - with floating point enabled
#else
    addi    sp, sp, -128                            // Allocate space for all registers - without floating point enabled
#endif
    sw      x1, 0x70(sp)                            // Store RA
    call    _tx_thread_context_save                 // Call ThreadX context save

    /* Call the ThreadX timer routine.  */
    call    _tx_timer_interrupt                     // Call timer interrupt handler

    /* Timer interrupt processing is done, jump to ThreadX context restore.  */
    tail    _tx_thread_context_restore              // Jump to ThreadX context restore function. Note: this does not return!

/* Timer Interrupt Handler Note:
   Platform-specific implementations must provide their own timer ISR.
   The timer interrupt handler should follow this execution flow:

   1. Disable interrupts (if not done by hardware exception entry)
   2. Allocate interrupt stack frame (65*4 bytes with FP, 32*4 bytes without)
   3. Save RA (x1) on the stack at offset 28*4
   4. Call _tx_thread_context_save to save thread context
   5. Call _tx_timer_interrupt to process the timer tick
   6. Call _tx_thread_context_restore to resume execution (does not return)

   Example (for CLINT timer):

       _tx_timer_interrupt_handler:
           addi    sp, sp, -32*4
           sw      ra, 28*4(sp)
           call    _tx_thread_context_save
           call    _tx_timer_interrupt
           j       _tx_thread_context_restore

   The port assumes Machine mode (M-mode) execution.
   For Supervisor mode (S-mode), use sstatus and SIE/SPIE instead of mstatus.
   See the RISC-V Privileged Specification for more details. */
