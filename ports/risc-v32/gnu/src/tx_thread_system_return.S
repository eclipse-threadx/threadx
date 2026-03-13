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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include "tx_port.h"

    .section .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_system_return                           RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is target processor specific.  It is used to transfer */
/*    control from a thread back to the system.  Only a minimal context   */
/*    is saved since the compiler assumes temp registers are going to get */
/*    slicked by a function call anyway.                                  */
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
/*    _tx_thread_schedule                   Thread scheduling loop        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX components                                                  */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_system_return(VOID)
{  */
    .global  _tx_thread_system_return
_tx_thread_system_return:

    /* Save minimal context on the stack.  */

#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, -TX_THREAD_FRAME_SIZE_FPU           // Allocate space on the stack - with floating point enabled
#else
    addi    sp, sp, -TX_THREAD_FRAME_SIZE_INT           // Allocate space on the stack - without floating point enabled
#endif

    /* Store floating point preserved registers.  */
#if defined(__riscv_flen) && (__riscv_flen == 32)
    fsw     f8,  TX_STACK_OFFSET_F8(sp)                 // Store fs0
    fsw     f9,  TX_STACK_OFFSET_F9(sp)                 // Store fs1
    fsw     f18, TX_STACK_OFFSET_F18(sp)                // Store fs2
    fsw     f19, TX_STACK_OFFSET_F19(sp)                // Store fs3
    fsw     f20, TX_STACK_OFFSET_F20(sp)                // Store fs4
    fsw     f21, TX_STACK_OFFSET_F21(sp)                // Store fs5
    fsw     f22, TX_STACK_OFFSET_F22(sp)                // Store fs6
    fsw     f23, TX_STACK_OFFSET_F23(sp)                // Store fs7
    fsw     f24, TX_STACK_OFFSET_F24(sp)                // Store fs8
    fsw     f25, TX_STACK_OFFSET_F25(sp)                // Store fs9
    fsw     f26, TX_STACK_OFFSET_F26(sp)                // Store fs10
    fsw     f27, TX_STACK_OFFSET_F27(sp)                // Store fs11
    csrr    t0, fcsr
    STORE   t0, TX_STACK_OFFSET_FCSR(sp)                // Store fcsr
#elif defined(__riscv_flen) && (__riscv_flen == 64)
    fsd     f8,  TX_STACK_OFFSET_F8(sp)                 // Store fs0
    fsd     f9,  TX_STACK_OFFSET_F9(sp)                 // Store fs1
    fsd     f18, TX_STACK_OFFSET_F18(sp)                // Store fs2
    fsd     f19, TX_STACK_OFFSET_F19(sp)                // Store fs3
    fsd     f20, TX_STACK_OFFSET_F20(sp)                // Store fs4
    fsd     f21, TX_STACK_OFFSET_F21(sp)                // Store fs5
    fsd     f22, TX_STACK_OFFSET_F22(sp)                // Store fs6
    fsd     f23, TX_STACK_OFFSET_F23(sp)                // Store fs7
    fsd     f24, TX_STACK_OFFSET_F24(sp)                // Store fs8
    fsd     f25, TX_STACK_OFFSET_F25(sp)                // Store fs9
    fsd     f26, TX_STACK_OFFSET_F26(sp)                // Store fs10
    fsd     f27, TX_STACK_OFFSET_F27(sp)                // Store fs11
    csrr    t0, fcsr
    STORE   t0, TX_STACK_OFFSET_FCSR(sp)                // Store fcsr
#endif

    STORE   x0,  0(sp)                                  // Solicited stack type
    STORE   x1,  TX_STACK_OFFSET_X1(sp)                 // Save RA
    STORE   x1,  TX_STACK_OFFSET_MEPC(sp)               // Save RA as MEPC
    STORE   x8,  TX_STACK_OFFSET_X8(sp)                 // Save s0
    STORE   x9,  TX_STACK_OFFSET_X9(sp)                 // Save s1
    STORE   x18, TX_STACK_OFFSET_X18(sp)                // Save s2
    STORE   x19, TX_STACK_OFFSET_X19(sp)                // Save s3
    STORE   x20, TX_STACK_OFFSET_X20(sp)                // Save s4
    STORE   x21, TX_STACK_OFFSET_X21(sp)                // Save s5
    STORE   x22, TX_STACK_OFFSET_X22(sp)                // Save s6
    STORE   x23, TX_STACK_OFFSET_X23(sp)                // Save s7
    STORE   x24, TX_STACK_OFFSET_X24(sp)                // Save s8
    STORE   x25, TX_STACK_OFFSET_X25(sp)                // Save s9
    STORE   x26, TX_STACK_OFFSET_X26(sp)                // Save s10
    STORE   x27, TX_STACK_OFFSET_X27(sp)                // Save s11
    csrr    t0, mstatus                                 // Pickup mstatus
    STORE   t0, TX_STACK_OFFSET_MSTATUS(sp)             // Save mstatus


   /* Lockout interrupts. - will be enabled in _tx_thread_schedule  */

    csrci   mstatus, 0xF

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_exit                   // Call the thread execution exit function
#endif

    la      t0, _tx_thread_current_ptr                  // Pickup address of pointer
    LOAD    t1, 0(t0)                                   // Pickup current thread pointer
    la      t2,_tx_thread_system_stack_ptr              // Pickup stack pointer address

    /* Save current stack and switch to system stack.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  SP;
    SP = _tx_thread_system_stack_ptr;  */

    STORE   sp, TX_THREAD_STACK_PTR(t1)                 // Save stack pointer
    LOAD    sp, 0(t2)                                   // Switch to system stack

    /* Determine if the time-slice is active.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t4, _tx_timer_time_slice                    // Pickup time slice variable addr
    LOAD    t3, 0(t4)                                   // Pickup time slice value
    la      t2, _tx_thread_schedule                     // Pickup address of scheduling loop
    beqz    t3, _tx_thread_dont_save_ts                 // If no time-slice, don't save it

        /* Save time-slice for the thread and clear the current time-slice.  */
        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
        _tx_timer_time_slice =  0;  */

    STORE   t3, TX_THREAD_TIME_SLICE(t1)                // Save current time-slice for thread
    STORE   x0, 0(t4)                                   // Clear time-slice variable

    /* }  */
_tx_thread_dont_save_ts:

    /* Clear the current thread pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    STORE   x0, 0(t0)                                   // Clear current thread pointer
    jr      t2                                          // Return to thread scheduler

/* }  */
