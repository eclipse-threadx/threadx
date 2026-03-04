/***************************************************************************
 * Copyright (c) 2026 Quintauris
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

    .section .data
    .global  __tx_free_memory_start
__tx_free_memory_start:


    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                           RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Francisco Merino, Quintauris                                        */
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
//    .global  _tx_initialize_low_level
    .weak    _tx_initialize_low_level
_tx_initialize_low_level:

    /* Save the system stack pointer.  */
    /* _tx_thread_system_stack_ptr = sp;  */

    la      t0, _tx_thread_system_stack_ptr         // Pickup address of system stack ptr
    sw      sp, 0(t0)                               // Save system stack pointer

    /* Pickup first free address.  */
    /* _tx_initialize_unused_memory(__tx_free_memory_start);  */

    la      t0, __tx_free_memory_start              // Pickup first free address
    la      t1, _tx_initialize_unused_memory        // Pickup address of unused memory
    sw      t0, 0(t1)                               // Save unused memory address

    /* Initialize floating point control/status register if floating point is enabled.  */
#ifdef __riscv_flen
    li      t0, 0
    csrw    fcsr, t0                                // Clear FP control/status register
#endif

    ret

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