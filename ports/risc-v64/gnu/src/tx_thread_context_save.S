/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
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
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                            RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
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
       out and the interrupt stack frame has been allocated and ra has
       been saved on the stack. */

    sd      t0, 19*8(sp)                                // First store t0 and t1
    sd      t1, 18*8(sp)

    la      t0, _tx_thread_system_state                 // Pickup address of system state
    ld      t1, 0(t0)                                   // Pickup system state

    /* Check for a nested interrupt condition.  */
    /* if (_tx_thread_system_state++)
    {  */
    beqz    t1, _tx_thread_not_nested_save              // If 0, first interrupt condition
    addi    t1, t1, 1                                   // Increment the interrupt counter
    sd      t1, 0(t0)                                   // Store the interrupt counter

    /* Nested interrupt condition.
       Save the rest of the scratch registers on the stack and return to the
       calling ISR.  */

    sd      t2,  17*8(sp)                               // Store t2
    sd      s0,  12*8(sp)                               // Store s0
    sd      a0, 27*8(sp)                                // Store a0
    sd      a1, 26*8(sp)                                // Store a1
    sd      a2, 25*8(sp)                                // Store a2
    sd      a3, 24*8(sp)                                // Store a3
    sd      a4, 23*8(sp)                                // Store a4
    sd      a5, 22*8(sp)                                // Store a5
    sd      a6, 21*8(sp)                                // Store a6
    sd      a7, 20*8(sp)                                // Store a7
    sd      t3, 16*8(sp)                                // Store t3
    sd      t4, 15*8(sp)                                // Store t4
    sd      t5, 14*8(sp)                                // Store t5
    sd      t6, 13*8(sp)                                // Store t6
#ifdef TX_RISCV_SMODE
    csrr    t0, sepc                                    // Load exception program counter
#else
    csrr    t0, mepc                                    // Load exception program counter
#endif
    sd      t0, 30*8(sp)                                // Save it on the stack

    /* Save floating point scratch registers if floating point is enabled.  */
#ifdef __riscv_float_abi_single
    fsw     f0, 31*8(sp)                                // Store ft0
    fsw     f1, 32*8(sp)                                // Store ft1
    fsw     f2, 33*8(sp)                                // Store ft2
    fsw     f3, 34*8(sp)                                // Store ft3
    fsw     f4, 35*8(sp)                                // Store ft4
    fsw     f5, 36*8(sp)                                // Store ft5
    fsw     f6, 37*8(sp)                                // Store ft6
    fsw     f7, 38*8(sp)                                // Store ft7
    fsw     f10,41*8(sp)                                // Store fa0
    fsw     f11,42*8(sp)                                // Store fa1
    fsw     f12,43*8(sp)                                // Store fa2
    fsw     f13,44*8(sp)                                // Store fa3
    fsw     f14,45*8(sp)                                // Store fa4
    fsw     f15,46*8(sp)                                // Store fa5
    fsw     f16,47*8(sp)                                // Store fa6
    fsw     f17,48*8(sp)                                // Store fa7
    fsw     f28,59*8(sp)                                // Store ft8
    fsw     f29,60*8(sp)                                // Store ft9
    fsw     f30,61*8(sp)                                // Store ft10
    fsw     f31,62*8(sp)                                // Store ft11
    csrr    t0, fcsr
    sd      t0, 63*8(sp)                                // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f0, 31*8(sp)                                // Store ft0
    fsd     f1, 32*8(sp)                                // Store ft1
    fsd     f2, 33*8(sp)                                // Store ft2
    fsd     f3, 34*8(sp)                                // Store ft3
    fsd     f4, 35*8(sp)                                // Store ft4
    fsd     f5, 36*8(sp)                                // Store ft5
    fsd     f6, 37*8(sp)                                // Store ft6
    fsd     f7, 38*8(sp)                                // Store ft7
    fsd     f10,41*8(sp)                                // Store fa0
    fsd     f11,42*8(sp)                                // Store fa1
    fsd     f12,43*8(sp)                                // Store fa2
    fsd     f13,44*8(sp)                                // Store fa3
    fsd     f14,45*8(sp)                                // Store fa4
    fsd     f15,46*8(sp)                                // Store fa5
    fsd     f16,47*8(sp)                                // Store fa6
    fsd     f17,48*8(sp)                                // Store fa7
    fsd     f28,59*8(sp)                                // Store ft8
    fsd     f29,60*8(sp)                                // Store ft9
    fsd     f30,61*8(sp)                                // Store ft10
    fsd     f31,62*8(sp)                                // Store ft11
    csrr    t0, fcsr
    sd      t0, 63*8(sp)                                // Store fcsr
#endif

#if defined(__riscv_vector)
    /* Store vector registers and CSRs */
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t1, sp, 64*8
#else
    addi    t1, sp, 31*8
#endif
    /* Store vector CSRs */
    csrr    t2, vstart                                  // Store vstart
    sd      t2, 0*8(t1)
    csrr    t2, vtype                                   // Store vtype
    sd      t2, 1*8(t1)
    csrr    t2, vl                                      // Store vl
    sd      t2, 2*8(t1)
    csrr    t2, vcsr                                    // Store vcsr
    sd      t2, 3*8(t1)

    /* Store vector registers v0-v31 */
    addi    t2, t1, 4*8
    vsetvli t3, zero, e8, m8, ta, ma
    vse8.v  v0, 0(t2)                                   // Store v0 ~ v7
    add     t2, t2, t3
    vse8.v  v8, 0(t2)                                   // Store v8 ~ v15
    add     t2, t2, t3
    vse8.v  v16, 0(t2)                                  // Store v16 ~ v23
    add     t2, t2, t3
    vse8.v  v24, 0(t2)                                  // Store v24 ~ v31
    add     t2, t2, t3
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
    addi    t1, t1, 1                                   // Increment the interrupt counter
    sd      t1, 0(t0)                                   // Store the interrupt counter

    /* Not nested: Find the user thread that was running and load our SP */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    ld      t0, 0(t0)                                   // Pickup current thread pointer
    beqz    t0, _tx_thread_idle_system_save             // If NULL, idle system was interrupted

    /* Save the standard scratch registers.  */

    sd      t2,  17*8(sp)                               // Store t2
    sd      s0,  12*8(sp)                               // Store s0
    sd      a0, 27*8(sp)                                // Store a0
    sd      a1, 26*8(sp)                                // Store a1
    sd      a2, 25*8(sp)                                // Store a2
    sd      a3, 24*8(sp)                                // Store a3
    sd      a4, 23*8(sp)                                // Store a4
    sd      a5, 22*8(sp)                                // Store a5
    sd      a6, 21*8(sp)                                // Store a6
    sd      a7, 20*8(sp)                                // Store a7
    sd      t3, 16*8(sp)                                // Store t3
    sd      t4, 15*8(sp)                                // Store t4
    sd      t5, 14*8(sp)                                // Store t5
    sd      t6, 13*8(sp)                                // Store t6

#ifdef TX_RISCV_SMODE
    csrr    t1, sepc                                    // Load exception program counter
#else
    csrr    t1, mepc                                    // Load exception program counter
#endif
    sd      t1, 30*8(sp)                                // Save it on the stack

    /* Save floating point scratch registers if floating point is enabled.  */
#ifdef __riscv_float_abi_single
    fsw     f0, 31*8(sp)                                // Store ft0
    fsw     f1, 32*8(sp)                                // Store ft1
    fsw     f2, 33*8(sp)                                // Store ft2
    fsw     f3, 34*8(sp)                                // Store ft3
    fsw     f4, 35*8(sp)                                // Store ft4
    fsw     f5, 36*8(sp)                                // Store ft5
    fsw     f6, 37*8(sp)                                // Store ft6
    fsw     f7, 38*8(sp)                                // Store ft7
    fsw     f10,41*8(sp)                                // Store fa0
    fsw     f11,42*8(sp)                                // Store fa1
    fsw     f12,43*8(sp)                                // Store fa2
    fsw     f13,44*8(sp)                                // Store fa3
    fsw     f14,45*8(sp)                                // Store fa4
    fsw     f15,46*8(sp)                                // Store fa5
    fsw     f16,47*8(sp)                                // Store fa6
    fsw     f17,48*8(sp)                                // Store fa7
    fsw     f28,59*8(sp)                                // Store ft8
    fsw     f29,60*8(sp)                                // Store ft9
    fsw     f30,61*8(sp)                                // Store ft10
    fsw     f31,62*8(sp)                                // Store ft11
    csrr    t0, fcsr
    sd      t0, 63*8(sp)                                // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f0, 31*8(sp)                                // Store ft0
    fsd     f1, 32*8(sp)                                // Store ft1
    fsd     f2, 33*8(sp)                                // Store ft2
    fsd     f3, 34*8(sp)                                // Store ft3
    fsd     f4, 35*8(sp)                                // Store ft4
    fsd     f5, 36*8(sp)                                // Store ft5
    fsd     f6, 37*8(sp)                                // Store ft6
    fsd     f7, 38*8(sp)                                // Store ft7
    fsd     f10,41*8(sp)                                // Store fa0
    fsd     f11,42*8(sp)                                // Store fa1
    fsd     f12,43*8(sp)                                // Store fa2
    fsd     f13,44*8(sp)                                // Store fa3
    fsd     f14,45*8(sp)                                // Store fa4
    fsd     f15,46*8(sp)                                // Store fa5
    fsd     f16,47*8(sp)                                // Store fa6
    fsd     f17,48*8(sp)                                // Store fa7
    fsd     f28,59*8(sp)                                // Store ft8
    fsd     f29,60*8(sp)                                // Store ft9
    fsd     f30,61*8(sp)                                // Store ft10
    fsd     f31,62*8(sp)                                // Store ft11
    csrr    t0, fcsr
    sd      t0, 63*8(sp)                                // Store fcsr
#endif

#if defined(__riscv_vector)
    /* Store vector registers and CSRs */
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t1, sp, 64*8
#else
    addi    t1, sp, 31*8
#endif
    /* Store vector CSRs */
    csrr    t2, vstart                                  // Store vstart
    sd      t2, 0*8(t1)
    csrr    t2, vtype                                   // Store vtype
    sd      t2, 1*8(t1)
    csrr    t2, vl                                      // Store vl
    sd      t2, 2*8(t1)
    csrr    t2, vcsr                                    // Store vcsr
    sd      t2, 3*8(t1)

    /* Store vector registers v0-v31 */
    addi    t2, t1, 4*8
    vsetvli t3, zero, e8, m8, ta, ma
    vse8.v  v0, 0(t2)                                   // Store v0 ~ v7
    add     t2, t2, t3
    vse8.v  v8, 0(t2)                                   // Store v8 ~ v15
    add     t2, t2, t3
    vse8.v  v16, 0(t2)                                  // Store v16 ~ v23
    add     t2, t2, t3
    vse8.v  v24, 0(t2)                                  // Store v24 ~ v31
    add     t2, t2, t3
#endif

    /* Save the current stack pointer in the thread's control block.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;  */

    /* Switch to the system stack.  */
    /* sp =  _tx_thread_system_stack_ptr;  */

    la      t1, _tx_thread_current_ptr                  // Pickup current thread pointer address
    ld      t1, 0(t1)                                   // Pickup current thread pointer
    sd      sp, 8(t1)                                   // Save stack pointer

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    /* _tx_execution_isr_enter is called with thread stack pointer */
    call    _tx_execution_isr_enter                     // Call the ISR execution enter function
#endif

    la      t0, _tx_thread_system_stack_ptr             // Pickup system stack pointer address
    ld      sp, 0(t0)                                   // Switch to system stack
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
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*8                                // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*8                                // Recover the reserved stack space
#endif

#if defined(__riscv_vector)
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t0, sp, -65*8
#else
    addi    t0, sp, -32*8
#endif
    csrr    t1, vlenb                                   // Get vector register byte length
    slli    t1, t1, 5                                   // Multiply by 32 (number of vector registers)
    addi    t1, t1, 4*8                                 // Add vector CSR space: vstart, vtype, vl, vcsr
    add     sp, sp, t1                                  // Recover vector stack frame

    ld      t1, 18*8(t0)                                // Recover t1
    ld      t0, 19*8(t0)                                // Recover t0
#endif
    ret                                                 // Return to calling ISR
