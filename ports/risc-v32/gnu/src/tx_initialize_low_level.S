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
#include "tx_port.h"

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
/*    Akif Ejaz, 10xEngineers                                             */
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
/*  23-12-2025        Akif Ejaz            Initial Version 6.4.x          */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{  */
    .global  _tx_initialize_low_level
    .weak    _tx_initialize_low_level
_tx_initialize_low_level:
    andi    sp, sp, -16                             // Align stack pointer to 16 bytes
    la      t0, _tx_thread_system_stack_ptr         // Pickup address of system stack ptr
    sw      sp, 0(t0)                               // Save system stack pointer

    la      t0, __tx_free_memory_start              // Pickup first free address
    la      t1, _tx_initialize_unused_memory        // Pickup address of unused memory
    sw      t0, 0(t1)                               // Save unused memory address

#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    li t0, 0x00006000                               // Set FS field of mstatus to 3 (Dirty)
    csrs mstatus, t0
    fscsr x0                                        // Clear the fcsr
#endif

    la      t0, _tx_trap_handler                    // Pickup trap handler address
    csrw    mtvec, t0                               // Store trap handler address
    ret

    /* Define the actual timer interrupt/exception handler.  */

    .global  _tx_trap_handler
_tx_trap_handler:
    /* Before calling _tx_thread_context_save, we have to allocate an interrupt
       stack frame and save the current value of x1 (ra). */

//#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
//    addi    sp, sp, -520                            // Allocate space for all registers - with floating point enabled
//#else
//    addi    sp, sp, -256                            // Allocate space for all registers - without floating point enabled
//#endif
//    sd      x1, 224(sp)                             // Store RA

#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, -TX_THREAD_FRAME_SIZE_FPU         // Allocate space for all registers - with floating point enabled
#else
    addi    sp, sp, -TX_THREAD_FRAME_SIZE_INT         // Allocate space for all registers - without floating point enabled
#endif

    STORE x1, TX_STACK_OFFSET_X1(sp)
    call    _tx_thread_context_save                   // Call ThreadX context save
    csrr    t0, mcause                                // Pickup mcause
    li      t1, 0x80000007
    beq     t0, t1, _tx_timer_handler_entry           // If mcause is Timer Interrupt, call timer interrupt handler
    
    j _tx_trap_exit
    /* Call the ThreadX timer routine.  */
    _tx_timer_handler_entry:
    call    _tx_timer_interrupt                       // Call timer interrupt handler

    /* Timer interrupt processing is done, jump to ThreadX context restore.  */
_tx_trap_exit:
    j       _tx_thread_context_restore                // Jump to ThreadX context restore function. Note: this does not return!
