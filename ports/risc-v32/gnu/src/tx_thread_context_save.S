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

    .section .text

#if defined(__riscv_float_abi_double)
    .equ    TX_TRAP_FRAME_SIZE, 400
#else
    .equ    TX_TRAP_FRAME_SIZE, 128
#endif

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
    .global  _tx_thread_context_save
_tx_thread_context_save:

    /* Upon entry to this routine, RA/x1 has been saved on the stack
       and the stack has been already allocated for the entire context:
       addi sp, sp, -TX_TRAP_FRAME_SIZE   (400 with FP, 128 without)
       sw   ra, 28*4(sp)
    */

    sw      t0, 19*4(sp)                         // Store t0
    sw      t1, 18*4(sp)                         // Store t1

    /* Check for a nested interrupt.  */
    /* if (_tx_thread_system_state++)
    {  */

    la      t0, _tx_thread_system_state          // Pickup addr of system state var
    lw      t1, 0(t0)                            // Pickup system state
    addi    t1, t1, 1                            // Increment system state
    sw      t1, 0(t0)                            // Store system state
    li      t0, 1
    bgt     t1, t0, _tx_thread_nested_save       // If it's more than 1, nested interrupt

    /* First level interrupt, save the rest of the scratch registers and
       check for a thread to preempt.  */

    sw      t2, 17*4(sp)                         // Store t2
    sw      s0, 12*4(sp)                         // Store s0
    sw      a0, 27*4(sp)                         // Store a0
    sw      a1, 26*4(sp)                         // Store a1
    sw      a2, 25*4(sp)                         // Store a2
    sw      a3, 24*4(sp)                         // Store a3
    sw      a4, 23*4(sp)                         // Store a4
    sw      a5, 22*4(sp)                         // Store a5
    sw      a6, 21*4(sp)                         // Store a6
    sw      a7, 20*4(sp)                         // Store a7
    sw      t3, 16*4(sp)                         // Store t3
    sw      t4, 15*4(sp)                         // Store t4
    sw      t5, 14*4(sp)                         // Store t5
    sw      t6, 13*4(sp)                         // Store t6

    /* Save mstatus and skip FP state if FS is Off.  */
    csrr    t0, mstatus
    sw      t0, 29*4(sp)
#if defined(__riscv_float_abi_double)
    srli    t1, t0, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_skip_fpu_save

    /* Save floating point registers (doubles at 128 + i*8).  */
    fsd     f0,  16*8(sp)                         // Store ft0
    fsd     f1,  17*8(sp)                         // Store ft1
    fsd     f2,  18*8(sp)                         // Store ft2
    fsd     f3,  19*8(sp)                         // Store ft3
    fsd     f4,  20*8(sp)                         // Store ft4
    fsd     f5,  21*8(sp)                         // Store ft5
    fsd     f6,  22*8(sp)                         // Store ft6
    fsd     f7,  23*8(sp)                         // Store ft7
    fsd     f10, 26*8(sp)                         // Store fa0
    fsd     f11, 27*8(sp)                         // Store fa1
    fsd     f12, 28*8(sp)                         // Store fa2
    fsd     f13, 29*8(sp)                         // Store fa3
    fsd     f14, 30*8(sp)                         // Store fa4
    fsd     f15, 31*8(sp)                         // Store fa5
    fsd     f16, 32*8(sp)                         // Store fa6
    fsd     f17, 33*8(sp)                         // Store fa7
    fsd     f28, 44*8(sp)                         // Store ft8
    fsd     f29, 45*8(sp)                         // Store ft9
    fsd     f30, 46*8(sp)                         // Store ft10
    fsd     f31, 47*8(sp)                         // Store ft11
    csrr    t0, fcsr
    sw      t0, 96*4(sp)                          // Store fcsr
#endif
_tx_thread_skip_fpu_save:

    csrr    t0, mepc
    sw      t0, 30*4(sp)                         // Save it on the stack

    la      t1, _tx_thread_current_ptr           // Pickup address of current thread ptr
    lw      t2, 0(t1)                            // Pickup current thread pointer
    beqz    t2, _tx_thread_idle_system_save      // If NULL, idle system was interrupted

    /* Save the current thread's stack pointer and switch to the system stack.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;
    sp = _tx_thread_system_stack_ptr;  */

    sw      sp, 8(t2)                            // Save stack pointer
    la      t0, _tx_thread_system_stack_ptr
    lw      sp, 0(t0)                            // Switch to system stack

    /* Call the ISR execution exit function if enabled.  */
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter              // Call the ISR execution enter function
#endif

    ret                                          // Return to ISR

_tx_thread_nested_save:

    /* Nested interrupt!  Just save the scratch registers and return to the ISR.  */

    sw      t2, 17*4(sp)                         // Store t2
    sw      s0, 12*4(sp)                         // Store s0
    sw      a0, 27*4(sp)                         // Store a0
    sw      a1, 26*4(sp)                         // Store a1
    sw      a2, 25*4(sp)                         // Store a2
    sw      a3, 24*4(sp)                         // Store a3
    sw      a4, 23*4(sp)                         // Store a4
    sw      a5, 22*4(sp)                         // Store a5
    sw      a6, 21*4(sp)                         // Store a6
    sw      a7, 20*4(sp)                         // Store a7
    sw      t3, 16*4(sp)                         // Store t3
    sw      t4, 15*4(sp)                         // Store t4
    sw      t5, 14*4(sp)                         // Store t5
    sw      t6, 13*4(sp)                         // Store t6

    /* Save mstatus and skip FP state if FS is Off.  */
    csrr    t0, mstatus
    sw      t0, 29*4(sp)
#if defined(__riscv_float_abi_double)
    srli    t1, t0, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_skip_nested_fpu_save

    /* Save floating point registers (doubles at 128 + i*8).  */
    fsd     f0,  16*8(sp)                         // Store ft0
    fsd     f1,  17*8(sp)                         // Store ft1
    fsd     f2,  18*8(sp)                         // Store ft2
    fsd     f3,  19*8(sp)                         // Store ft3
    fsd     f4,  20*8(sp)                         // Store ft4
    fsd     f5,  21*8(sp)                         // Store ft5
    fsd     f6,  22*8(sp)                         // Store ft6
    fsd     f7,  23*8(sp)                         // Store ft7
    fsd     f10, 26*8(sp)                         // Store fa0
    fsd     f11, 27*8(sp)                         // Store fa1
    fsd     f12, 28*8(sp)                         // Store fa2
    fsd     f13, 29*8(sp)                         // Store fa3
    fsd     f14, 30*8(sp)                         // Store fa4
    fsd     f15, 31*8(sp)                         // Store fa5
    fsd     f16, 32*8(sp)                         // Store fa6
    fsd     f17, 33*8(sp)                         // Store fa7
    fsd     f28, 44*8(sp)                         // Store ft8
    fsd     f29, 45*8(sp)                         // Store ft9
    fsd     f30, 46*8(sp)                         // Store ft10
    fsd     f31, 47*8(sp)                         // Store ft11
    csrr    t0, fcsr
    sw      t0, 96*4(sp)                          // Store fcsr
#endif
_tx_thread_skip_nested_fpu_save:

    csrr    t0, mepc
    sw      t0, 30*4(sp)                         // Save it on stack

    /* Call the ISR execution exit function if enabled.  */
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter              // Call the ISR execution enter function
#endif

    ret                                          // Return to ISR

_tx_thread_idle_system_save:


#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif

    /* Interrupt occurred in the scheduling loop.  */

    /* }
}  */
    addi    sp, sp, TX_TRAP_FRAME_SIZE                 // Recover the reserved stack space
    ret                                                // Return to calling ISR
