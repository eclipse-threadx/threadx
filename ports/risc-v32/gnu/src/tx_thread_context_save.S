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
/*    _tx_thread_context_save                            RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function saves the context of an executing thread in the       */
/*    beginning of interrupt processing.  The function also ensures that  */
/*    the system stack is used upon return to the calling ISR.            */
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
/*    ISRs                                                                */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_context_save(VOID)
{  */
    .global      _tx_thread_context_save
_tx_thread_context_save:

    /* Upon entry to this routine, it is assumed that interrupts are locked
       out and the interrupt stack fame has been allocated and x1 (ra) has
       been saved on the stack. */

    STORE   x5, TX_STACK_OFFSET_X5(sp)                  // First store t0 and t1
    STORE   x6, TX_STACK_OFFSET_X6(sp)

    la      x5, _tx_thread_system_state                 // Pickup address of system state
    LOAD    x6, 0(x5)                                   // Pickup system state

    /* Check for a nested interrupt condition.  */
    /* if (_tx_thread_system_state++)
    {  */
    beqz    x6, _tx_thread_not_nested_save              // If 0, first interrupt condition
    addi    x6, x6, 1                                   // Increment the interrupt counter
    STORE   x6, 0(x5)                                   // Store the interrupt counter

    /* Nested interrupt condition.
       Save the reset of the scratch registers on the stack and return to the
       calling ISR.  */

    STORE   x7,  TX_STACK_OFFSET_X7(sp)                 // Store t2
    STORE   x8,  TX_STACK_OFFSET_X8(sp)                 // Store s0
    STORE   x10, TX_STACK_OFFSET_X10(sp)                // Store a0
    STORE   x11, TX_STACK_OFFSET_X11(sp)                // Store a1
    STORE   x12, TX_STACK_OFFSET_X12(sp)                // Store a2
    STORE   x13, TX_STACK_OFFSET_X13(sp)                // Store a3
    STORE   x14, TX_STACK_OFFSET_X14(sp)                // Store a4
    STORE   x15, TX_STACK_OFFSET_X15(sp)                // Store a5
    STORE   x16, TX_STACK_OFFSET_X16(sp)                // Store a6
    STORE   x17, TX_STACK_OFFSET_X17(sp)                // Store a7
    STORE   x28, TX_STACK_OFFSET_X28(sp)                // Store t3
    STORE   x29, TX_STACK_OFFSET_X29(sp)                // Store t4
    STORE   x30, TX_STACK_OFFSET_X30(sp)                // Store t5
    STORE   x31, TX_STACK_OFFSET_X31(sp)                // Store t6
    csrr    t0, mepc                                    // Load exception program counter
    STORE   t0, TX_STACK_OFFSET_MEPC(sp)                // Save it on the stack

    /* Save floating point scratch registers.  */
#if defined(__riscv_flen)
    csrr  t0, mstatus                                   // Pickup thread's floating point state
    STORE t0, TX_STACK_OFFSET_MSTATUS(sp)
    /* Check the floating point status for lazy FPU.
       Invariant: If FS is Off (0), the FP registers are Disabled/Unused
       so we can skip saving them. 
       Note: We strictly save Initial (1), Clean (2), and Dirty (3) states. */
    srli  t1, t0, 13
    andi  t1, t1, 0x3
    beqz  t1, _tx_skip_nested_fpu_save                  // Skip floating point save if FS is Off
#if (__riscv_flen == 32)
    fsw     f0, TX_STACK_OFFSET_F0(sp)                  // Store ft0
    fsw     f1, TX_STACK_OFFSET_F1(sp)                  // Store ft1
    fsw     f2, TX_STACK_OFFSET_F2(sp)                  // Store ft2
    fsw     f3, TX_STACK_OFFSET_F3(sp)                  // Store ft3
    fsw     f4, TX_STACK_OFFSET_F4(sp)                  // Store ft4
    fsw     f5, TX_STACK_OFFSET_F5(sp)                  // Store ft5
    fsw     f6, TX_STACK_OFFSET_F6(sp)                  // Store ft6
    fsw     f7, TX_STACK_OFFSET_F7(sp)                  // Store ft7
    fsw     f10,TX_STACK_OFFSET_F10(sp)                 // Store fa0
    fsw     f11,TX_STACK_OFFSET_F11(sp)                 // Store fa1
    fsw     f12,TX_STACK_OFFSET_F12(sp)                 // Store fa2
    fsw     f13,TX_STACK_OFFSET_F13(sp)                 // Store fa3
    fsw     f14,TX_STACK_OFFSET_F14(sp)                 // Store fa4
    fsw     f15,TX_STACK_OFFSET_F15(sp)                 // Store fa5
    fsw     f16,TX_STACK_OFFSET_F16(sp)                 // Store fa6
    fsw     f17,TX_STACK_OFFSET_F17(sp)                 // Store fa7
    fsw     f28,TX_STACK_OFFSET_F28(sp)                 // Store ft8
    fsw     f29,TX_STACK_OFFSET_F29(sp)                 // Store ft9
    fsw     f30,TX_STACK_OFFSET_F30(sp)                 // Store ft10
    fsw     f31,TX_STACK_OFFSET_F31(sp)                 // Store ft11
    csrr    t0, fcsr
    STORE   t0, TX_STACK_OFFSET_FCSR(sp)                // Store fcsr
#elif (__riscv_flen == 64)
    fsd     f0, TX_STACK_OFFSET_F0(sp)                  // Store ft0
    fsd     f1, TX_STACK_OFFSET_F1(sp)                  // Store ft1
    fsd     f2, TX_STACK_OFFSET_F2(sp)                  // Store ft2
    fsd     f3, TX_STACK_OFFSET_F3(sp)                  // Store ft3
    fsd     f4, TX_STACK_OFFSET_F4(sp)                  // Store ft4
    fsd     f5, TX_STACK_OFFSET_F5(sp)                  // Store ft5
    fsd     f6, TX_STACK_OFFSET_F6(sp)                  // Store ft6
    fsd     f7, TX_STACK_OFFSET_F7(sp)                  // Store ft7
    fsd     f10,TX_STACK_OFFSET_F10(sp)                 // Store fa0
    fsd     f11,TX_STACK_OFFSET_F11(sp)                 // Store fa1
    fsd     f12,TX_STACK_OFFSET_F12(sp)                 // Store fa2
    fsd     f13,TX_STACK_OFFSET_F13(sp)                 // Store fa3
    fsd     f14,TX_STACK_OFFSET_F14(sp)                 // Store fa4
    fsd     f15,TX_STACK_OFFSET_F15(sp)                 // Store fa5
    fsd     f16,TX_STACK_OFFSET_F16(sp)                 // Store fa6
    fsd     f17,TX_STACK_OFFSET_F17(sp)                 // Store fa7
    fsd     f28,TX_STACK_OFFSET_F28(sp)                 // Store ft8
    fsd     f29,TX_STACK_OFFSET_F29(sp)                 // Store ft9
    fsd     f30,TX_STACK_OFFSET_F30(sp)                 // Store ft10
    fsd     f31,TX_STACK_OFFSET_F31(sp)                 // Store ft11
    csrr    t0, fcsr
    STORE   t0, TX_STACK_OFFSET_FCSR(sp)                // Store fcsr
#endif
_tx_skip_nested_fpu_save:
#endif

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif

    ret                                                 // Return to calling ISR

_tx_thread_not_nested_save:
    /* }  */

    /* Otherwise, not nested, check to see if a thread was running.  */
    /* else if (_tx_thread_current_ptr)
    {  */
    addi    x6, x6, 1                                   // Increment the interrupt counter
    STORE   x6, 0(x5)                                   // Store the interrupt counter

    /* Not nested: Find the user thread that was running and load our SP */

    LOAD    x5, _tx_thread_current_ptr                  // Pickup current thread pointer
    beqz    x5, _tx_thread_idle_system_save             // If NULL, idle system was interrupted

    /* Save the standard scratch registers.  */

    STORE   x7,  TX_STACK_OFFSET_X7(sp)                 // Store t2
    STORE   x8,  TX_STACK_OFFSET_X8(sp)                 // Store s0
    STORE   x10, TX_STACK_OFFSET_X10(sp)                // Store a0
    STORE   x11, TX_STACK_OFFSET_X11(sp)                // Store a1
    STORE   x12, TX_STACK_OFFSET_X12(sp)                // Store a2
    STORE   x13, TX_STACK_OFFSET_X13(sp)                // Store a3
    STORE   x14, TX_STACK_OFFSET_X14(sp)                // Store a4
    STORE   x15, TX_STACK_OFFSET_X15(sp)                // Store a5
    STORE   x16, TX_STACK_OFFSET_X16(sp)                // Store a6
    STORE   x17, TX_STACK_OFFSET_X17(sp)                // Store a7
    STORE   x28, TX_STACK_OFFSET_X28(sp)                // Store t3
    STORE   x29, TX_STACK_OFFSET_X29(sp)                // Store t4
    STORE   x30, TX_STACK_OFFSET_X30(sp)                // Store t5
    STORE   x31, TX_STACK_OFFSET_X31(sp)                // Store t6

    csrr    t0, mepc                                    // Load exception program counter
    STORE   t0, TX_STACK_OFFSET_MEPC(sp)                // Save it on the stack

    /* Save floating point scratch registers.  */
#if defined(__riscv_flen)
    csrr  t0, mstatus                                   // Pickup thread's floating point state
    STORE t0, TX_STACK_OFFSET_MSTATUS(sp)
    /* Check the floating point status for lazy FPU*/
    srli  t1, t0, 13
    andi  t1, t1, 0x3
    beqz  t1, _tx_skip_not_nested_fpu_save              // Skip floating point save FS is Off
#if (__riscv_flen == 32)
    fsw     f0, TX_STACK_OFFSET_F0(sp)                  // Store ft0
    fsw     f1, TX_STACK_OFFSET_F1(sp)                  // Store ft1
    fsw     f2, TX_STACK_OFFSET_F2(sp)                  // Store ft2
    fsw     f3, TX_STACK_OFFSET_F3(sp)                  // Store ft3
    fsw     f4, TX_STACK_OFFSET_F4(sp)                  // Store ft4
    fsw     f5, TX_STACK_OFFSET_F5(sp)                  // Store ft5
    fsw     f6, TX_STACK_OFFSET_F6(sp)                  // Store ft6
    fsw     f7, TX_STACK_OFFSET_F7(sp)                  // Store ft7
    fsw     f10,TX_STACK_OFFSET_F10(sp)                 // Store fa0
    fsw     f11,TX_STACK_OFFSET_F11(sp)                 // Store fa1
    fsw     f12,TX_STACK_OFFSET_F12(sp)                 // Store fa2
    fsw     f13,TX_STACK_OFFSET_F13(sp)                 // Store fa3
    fsw     f14,TX_STACK_OFFSET_F14(sp)                 // Store fa4
    fsw     f15,TX_STACK_OFFSET_F15(sp)                 // Store fa5
    fsw     f16,TX_STACK_OFFSET_F16(sp)                 // Store fa6
    fsw     f17,TX_STACK_OFFSET_F17(sp)                 // Store fa7
    fsw     f28,TX_STACK_OFFSET_F28(sp)                 // Store ft8
    fsw     f29,TX_STACK_OFFSET_F29(sp)                 // Store ft9
    fsw     f30,TX_STACK_OFFSET_F30(sp)                 // Store ft10
    fsw     f31,TX_STACK_OFFSET_F31(sp)                 // Store ft11
    csrr    t0, fcsr
    STORE   t0, TX_STACK_OFFSET_FCSR(sp)                // Store fcsr
#elif (__riscv_flen == 64)
    fsd     f0, TX_STACK_OFFSET_F0(sp)                  // Store ft0
    fsd     f1, TX_STACK_OFFSET_F1(sp)                  // Store ft1
    fsd     f2, TX_STACK_OFFSET_F2(sp)                  // Store ft2
    fsd     f3, TX_STACK_OFFSET_F3(sp)                  // Store ft3
    fsd     f4, TX_STACK_OFFSET_F4(sp)                  // Store ft4
    fsd     f5, TX_STACK_OFFSET_F5(sp)                  // Store ft5
    fsd     f6, TX_STACK_OFFSET_F6(sp)                  // Store ft6
    fsd     f7, TX_STACK_OFFSET_F7(sp)                  // Store ft7
    fsd     f10,TX_STACK_OFFSET_F10(sp)                 // Store fa0
    fsd     f11,TX_STACK_OFFSET_F11(sp)                 // Store fa1
    fsd     f12,TX_STACK_OFFSET_F12(sp)                 // Store fa2
    fsd     f13,TX_STACK_OFFSET_F13(sp)                 // Store fa3
    fsd     f14,TX_STACK_OFFSET_F14(sp)                 // Store fa4
    fsd     f15,TX_STACK_OFFSET_F15(sp)                 // Store fa5
    fsd     f16,TX_STACK_OFFSET_F16(sp)                 // Store fa6
    fsd     f17,TX_STACK_OFFSET_F17(sp)                 // Store fa7
    fsd     f28,TX_STACK_OFFSET_F28(sp)                 // Store ft8
    fsd     f29,TX_STACK_OFFSET_F29(sp)                 // Store ft9
    fsd     f30,TX_STACK_OFFSET_F30(sp)                 // Store ft10
    fsd     f31,TX_STACK_OFFSET_F31(sp)                 // Store ft11
    csrr    t0, fcsr
    STORE   t0, TX_STACK_OFFSET_FCSR(sp)                // Store fcsr
#endif
_tx_skip_not_nested_fpu_save:
#endif

    /* Save the current stack pointer in the thread's control block.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;  */

    /* Switch to the system stack.  */
    /* sp =  _tx_thread_system_stack_ptr;  */

    LOAD    t1, _tx_thread_current_ptr                  // Pickup current thread pointer
    STORE   sp, TX_THREAD_STACK_PTR(t1)                 // Save stack pointer

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* _tx_execution_isr_enter is called with thread stack pointer */
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif


    LOAD    sp, _tx_thread_system_stack_ptr             // Switch to system stack
    ret                                                 // Return to calling ISR

    /* }
    else
    {  */

_tx_thread_idle_system_save:


#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif

    /* Interrupt occurred in the scheduling loop.  */

    /* }
}  */
#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, TX_THREAD_FRAME_SIZE_FPU            // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, TX_THREAD_FRAME_SIZE_INT            // Recover the reserved stack space
#endif
    ret                                                 // Return to calling ISR
