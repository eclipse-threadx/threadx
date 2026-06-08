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
/*    _tx_thread_context_restore                         RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function restores the interrupt context if it is processing a  */
/*    nested interrupt.  If not, it returns to the interrupt thread if no */
/*    preemption is necessary.  Otherwise, if preemption is necessary or  */
/*    if no thread was running, the function returns to the scheduler.    */
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
/*    _tx_thread_schedule                   Thread scheduling routine     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                  Interrupt Service Routines    */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_context_restore(VOID)
{  */
    .global  _tx_thread_context_restore
_tx_thread_context_restore:

    /* Lockout interrupts.  */

#ifdef TX_RISCV_SMODE
    csrci   sstatus, 0x02                               // Disable interrupts (SIE bit 1)
#else
    csrci   mstatus, 0x08                               // Disable interrupts (MIE bit 3)
#endif

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_exit                      // Call the ISR execution exit function
#endif

    /* Determine if interrupts are nested.  */
    /* if (--_tx_thread_system_state)
    {  */

    la      t0, _tx_thread_system_state                 // Pickup addr of nested interrupt count
    lw      t1, 0(t0)                                   // Pickup nested interrupt count
    addi    t1, t1, -1                                  // Decrement the nested interrupt counter
    sw      t1, 0(t0)                                   // Store new nested count
    beqz    t1, _tx_thread_not_nested_restore           // If 0, not nested restore

    /* Interrupts are nested.  */

    /* Just recover the saved registers and return to the point of
       interrupt.  */

    /* Recover floating point registers.  */
#if defined(__riscv_float_abi_single)
    flw     f0, 31*8(sp)                                // Recover ft0
    flw     f1, 32*8(sp)                                // Recover ft1
    flw     f2, 33*8(sp)                                // Recover ft2
    flw     f3, 34*8(sp)                                // Recover ft3
    flw     f4, 35*8(sp)                                // Recover ft4
    flw     f5, 36*8(sp)                                // Recover ft5
    flw     f6, 37*8(sp)                                // Recover ft6
    flw     f7, 38*8(sp)                                // Recover ft7
    flw     f10,41*8(sp)                                // Recover fa0
    flw     f11,42*8(sp)                                // Recover fa1
    flw     f12,43*8(sp)                                // Recover fa2
    flw     f13,44*8(sp)                                // Recover fa3
    flw     f14,45*8(sp)                                // Recover fa4
    flw     f15,46*8(sp)                                // Recover fa5
    flw     f16,47*8(sp)                                // Recover fa6
    flw     f17,48*8(sp)                                // Recover fa7
    flw     f28,59*8(sp)                                // Recover ft8
    flw     f29,60*8(sp)                                // Recover ft9
    flw     f30,61*8(sp)                                // Recover ft10
    flw     f31,62*8(sp)                                // Recover ft11
    ld      t0, 63*8(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#elif defined(__riscv_float_abi_double)
    fld     f0, 31*8(sp)                                // Recover ft0
    fld     f1, 32*8(sp)                                // Recover ft1
    fld     f2, 33*8(sp)                                // Recover ft2
    fld     f3, 34*8(sp)                                // Recover ft3
    fld     f4, 35*8(sp)                                // Recover ft4
    fld     f5, 36*8(sp)                                // Recover ft5
    fld     f6, 37*8(sp)                                // Recover ft6
    fld     f7, 38*8(sp)                                // Recover ft7
    fld     f10,41*8(sp)                                // Recover fa0
    fld     f11,42*8(sp)                                // Recover fa1
    fld     f12,43*8(sp)                                // Recover fa2
    fld     f13,44*8(sp)                                // Recover fa3
    fld     f14,45*8(sp)                                // Recover fa4
    fld     f15,46*8(sp)                                // Recover fa5
    fld     f16,47*8(sp)                                // Recover fa6
    fld     f17,48*8(sp)                                // Recover fa7
    fld     f28,59*8(sp)                                // Recover ft8
    fld     f29,60*8(sp)                                // Recover ft9
    fld     f30,61*8(sp)                                // Recover ft10
    fld     f31,62*8(sp)                                // Recover ft11
    ld      t0, 63*8(sp)                                // Recover fcsr
    csrw    fcsr, t0
#endif

#if defined(__riscv_vector)
    /* Recover vector registers v0-v31 */
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t1, sp, 64*8
#else
    addi    t1, sp, 31*8
#endif
    addi    t2, t1, 4*8
    vsetvli t3, zero, e8, m8, ta, ma
    vle8.v  v0, (t2)                                    // Recover v0 ~ v7
    add     t2, t2, t3
    vle8.v  v8, (t2)                                    // Recover v8 ~ v15
    add     t2, t2, t3
    vle8.v  v16, (t2)                                   // Recover v16 ~ v23
    add     t2, t2, t3
    vle8.v  v24, (t2)                                   // Recover v24 ~ v31
    add     t2, t2, t3

    /* Recover vector CSRs */
    ld      t2, 0*8(t1)
    ld      t3, 1*8(t1)
    ld      t4, 2*8(t1)
    vsetvl  zero, t4, t3
    csrw    vstart, t2
    ld      t4, 3*8(t1)
    csrw    vcsr, t4
#endif

    /* Recover standard registers.  */

    /* Restore registers,
       Skip global pointer because that does not change.
       Also skip the saved registers since they have been restored by any function we called,
       except s0 since we use it ourselves. */

    ld      t0, 30*8(sp)                                // Recover exception PC
#ifdef TX_RISCV_SMODE
    csrw    sepc, t0                                    // Setup sepc

    /* Compose sstatus via read/modify/write to avoid clobbering unrelated bits.
       Set SPIE and restore SPP to Supervisor, preserve other fields. */

    csrr    t1, sstatus
    li      t4, ~0x122                                  // Clear mask for SPP/SPIE/SIE
    and     t1, t1, t4
    li      t3, 0x120                                   // Set SPP=Supervisor(0x100) + SPIE(0x20) so sret re-enables SIE
    or      t1, t1, t3

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t0, 0x6000                                  // Set FS=Dirty (bits 14:13) for FP state
    or      t1, t1, t0
#endif
    csrw    sstatus, t1                                 // Update sstatus safely
#else
    csrw    mepc, t0                                    // Setup mepc

    /* Compose mstatus via read/modify/write to avoid clobbering unrelated bits.
       Set MPIE and restore MPP to Machine, preserve other fields. */

    csrr    t1, mstatus
    li      t2, 0x1880                                  // Set MPP(0x1800) | MPIE(0x80)

    or      t1, t1, t2                                  // MPP and MPIE are now set. All other bits are preserved
    andi    t1, t1, ~0x8                                // Clear MIE, Hardware will restore it from MPIE

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t0, 0x2000                                  // Set FS bits (bits 14:13 to 01) for FP state
    or      t1, t1, t0
#endif
#if defined(__riscv_vector)
    li      t0, 0x0200                                  // Set VS bits (bits 10:9 to 01) for vector state
    or      t1, t1, t0
#endif
    csrw    mstatus, t1                                 // Update mstatus safely
#endif

    ld      ra,  28*8(sp)                               // Recover return address
    ld      t0,  19*8(sp)                               // Recover t0
    ld      t1,  18*8(sp)                               // Recover t1
    ld      t2,  17*8(sp)                               // Recover t2
    ld      s0,  12*8(sp)                               // Recover s0
    ld      a0, 27*8(sp)                                // Recover a0
    ld      a1, 26*8(sp)                                // Recover a1
    ld      a2, 25*8(sp)                                // Recover a2
    ld      a3, 24*8(sp)                                // Recover a3
    ld      a4, 23*8(sp)                                // Recover a4
    ld      a5, 22*8(sp)                                // Recover a5
    ld      a6, 21*8(sp)                                // Recover a6
    ld      a7, 20*8(sp)                                // Recover a7
    ld      t3, 16*8(sp)                                // Recover t3
    ld      t4, 15*8(sp)                                // Recover t4
    ld      t5, 14*8(sp)                                // Recover t5
    ld      t6, 13*8(sp)                                // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*8                                // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*8                                // Recover stack frame - without floating point enabled
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
#ifdef TX_RISCV_SMODE
    sret                                                // Return to point of interrupt
#else
    mret                                                // Return to point of interrupt
#endif

    /* }  */
_tx_thread_not_nested_restore:
    /* Determine if a thread was interrupted and no preemption is required.  */
    /* else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr)
               || (_tx_thread_preempt_disable))
    {  */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    ld      t1, 0(t0)                                   // Pickup current thread pointer
    beqz    t1, _tx_thread_idle_system_restore          // If NULL, idle system restore

    la      t0, _tx_thread_preempt_disable              // Pickup preempt disable flag address
    lw      t2, 0(t0)                                   // Pickup preempt disable flag (UINT)
    bgtz    t2, _tx_thread_no_preempt_restore           // If set, restore interrupted thread

    la      t0, _tx_thread_execute_ptr                  // Pickup thread execute pointer address
    ld      t2, 0(t0)                                   // Pickup thread execute pointer
    bne     t1, t2, _tx_thread_preempt_restore          // If higher-priority thread is ready, preempt


_tx_thread_no_preempt_restore:
    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    /* sp =  _tx_thread_current_ptr -> tx_thread_stack_ptr;  */

    ld      sp, 8(t1)                                  // Switch back to thread's stack

    /* Recover floating point registers.  */
#if defined(__riscv_float_abi_single)
    flw     f0, 31*8(sp)                                // Recover ft0
    flw     f1, 32*8(sp)                                // Recover ft1
    flw     f2, 33*8(sp)                                // Recover ft2
    flw     f3, 34*8(sp)                                // Recover ft3
    flw     f4, 35*8(sp)                                // Recover ft4
    flw     f5, 36*8(sp)                                // Recover ft5
    flw     f6, 37*8(sp)                                // Recover ft6
    flw     f7, 38*8(sp)                                // Recover ft7
    flw     f10,41*8(sp)                                // Recover fa0
    flw     f11,42*8(sp)                                // Recover fa1
    flw     f12,43*8(sp)                                // Recover fa2
    flw     f13,44*8(sp)                                // Recover fa3
    flw     f14,45*8(sp)                                // Recover fa4
    flw     f15,46*8(sp)                                // Recover fa5
    flw     f16,47*8(sp)                                // Recover fa6
    flw     f17,48*8(sp)                                // Recover fa7
    flw     f28,59*8(sp)                                // Recover ft8
    flw     f29,60*8(sp)                                // Recover ft9
    flw     f30,61*8(sp)                                // Recover ft10
    flw     f31,62*8(sp)                                // Recover ft11
    ld      t0, 63*8(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#elif defined(__riscv_float_abi_double)
    fld     f0, 31*8(sp)                                // Recover ft0
    fld     f1, 32*8(sp)                                // Recover ft1
    fld     f2, 33*8(sp)                                // Recover ft2
    fld     f3, 34*8(sp)                                // Recover ft3
    fld     f4, 35*8(sp)                                // Recover ft4
    fld     f5, 36*8(sp)                                // Recover ft5
    fld     f6, 37*8(sp)                                // Recover ft6
    fld     f7, 38*8(sp)                                // Recover ft7
    fld     f10,41*8(sp)                                // Recover fa0
    fld     f11,42*8(sp)                                // Recover fa1
    fld     f12,43*8(sp)                                // Recover fa2
    fld     f13,44*8(sp)                                // Recover fa3
    fld     f14,45*8(sp)                                // Recover fa4
    fld     f15,46*8(sp)                                // Recover fa5
    fld     f16,47*8(sp)                                // Recover fa6
    fld     f17,48*8(sp)                                // Recover fa7
    fld     f28,59*8(sp)                                // Recover ft8
    fld     f29,60*8(sp)                                // Recover ft9
    fld     f30,61*8(sp)                                // Recover ft10
    fld     f31,62*8(sp)                                // Recover ft11
    ld      t0, 63*8(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#endif

#if defined(__riscv_vector)
    /* Recover vector registers v0-v31 */
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t1, sp, 64*8
#else
    addi    t1, sp, 31*8
#endif
    addi    t2, t1, 4*8
    vsetvli t3, zero, e8, m8, ta, ma
    vle8.v  v0, (t2)                                    // Recover v0 ~ v7
    add     t2, t2, t3
    vle8.v  v8, (t2)                                    // Recover v8 ~ v15
    add     t2, t2, t3
    vle8.v  v16, (t2)                                   // Recover v16 ~ v23
    add     t2, t2, t3
    vle8.v  v24, (t2)                                   // Recover v24 ~ v31
    add     t2, t2, t3

    /* Recover vector CSRs */
    ld      t2, 0*8(t1)
    ld      t3, 1*8(t1)
    ld      t4, 2*8(t1)
    vsetvl  zero, t4, t3
    csrw    vstart, t2
    ld      t4, 3*8(t1)
    csrw    vcsr, t4
#endif

    /* Recover the saved context and return to the point of interrupt.  */

    /* Recover standard registers.  */
    /* Restore registers,
       Skip global pointer because that does not change */

    ld      t0, 30*8(sp)                                // Recover exception PC
#ifdef TX_RISCV_SMODE
    csrw    sepc, t0                                    // Setup sepc

    /* Compose sstatus via read/modify/write to avoid clobbering unrelated bits. */

    csrr    t1, sstatus
    li      t4, ~0x122                                  // Clear mask for SPP/SPIE/SIE
    and     t1, t1, t4
    li      t3, 0x120                                   // Set SPP=Supervisor(0x100) + SPIE(0x20) so sret re-enables SIE
    or      t1, t1, t3

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t0, 0x6000                                  // Set FS=Dirty (bits 14:13) for FP state
    or      t1, t1, t0
#endif
    csrw    sstatus, t1                                 // Update sstatus safely
#else
    csrw    mepc, t0                                    // Setup mepc


    /* Compose mstatus via read/modify/write to avoid clobbering unrelated bits.
       Set MPIE and restore MPP to Machine, preserve other fields. */

    csrr    t1, mstatus
    li      t2, 0x1880                                  // Set MPP(0x1800) | MPIE(0x80)

    or      t1, t1, t2                                  // MPP and MPIE are now set. All other bits are preserved
    andi    t1, t1, ~0x8                                // Clear MIE, Hardware will restore it from MPIE

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t0, 0x2000                                  // Set FS bits for FP state
    or      t1, t1, t0
#endif
#if defined(__riscv_vector)
    li      t0, 0x0200                                  // Set VS bits (bits 10:9 to 01) for vector state
    or      t1, t1, t0
#endif
    csrw    mstatus, t1                                 // Update mstatus safely
#endif

    ld      ra,  28*8(sp)                               // Recover return address
    ld      t0,  19*8(sp)                               // Recover t0
    ld      t1,  18*8(sp)                               // Recover t1
    ld      t2,  17*8(sp)                               // Recover t2
    ld      s0,  12*8(sp)                               // Recover s0
    ld      a0, 27*8(sp)                                // Recover a0
    ld      a1, 26*8(sp)                                // Recover a1
    ld      a2, 25*8(sp)                                // Recover a2
    ld      a3, 24*8(sp)                                // Recover a3
    ld      a4, 23*8(sp)                                // Recover a4
    ld      a5, 22*8(sp)                                // Recover a5
    ld      a6, 21*8(sp)                                // Recover a6
    ld      a7, 20*8(sp)                                // Recover a7
    ld      t3, 16*8(sp)                                // Recover t3
    ld      t4, 15*8(sp)                                // Recover t4
    ld      t5, 14*8(sp)                                // Recover t5
    ld      t6, 13*8(sp)                                // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*8                                // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*8                                // Recover stack frame - without floating point enabled
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
#ifdef TX_RISCV_SMODE
    sret                                                // Return to point of interrupt
#else
    mret                                                // Return to point of interrupt
#endif

    /* }
    else
    {  */
_tx_thread_preempt_restore:
    /* Instead of directly activating the thread again, ensure we save the
       entire stack frame by saving the remaining registers. */

    ld      t0, 8(t1)                                  // Pickup thread's stack pointer
    ori     t3, zero, 1                                 // Build interrupt stack type
    sd      t3, 0(t0)                                   // Store stack type

    /* Store floating point preserved registers.  */
#ifdef __riscv_float_abi_single
    fsw     f8,  39*8(t0)                               // Store fs0
    fsw     f9,  40*8(t0)                               // Store fs1
    fsw     f18, 49*8(t0)                               // Store fs2
    fsw     f19, 50*8(t0)                               // Store fs3
    fsw     f20, 51*8(t0)                               // Store fs4
    fsw     f21, 52*8(t0)                               // Store fs5
    fsw     f22, 53*8(t0)                               // Store fs6
    fsw     f23, 54*8(t0)                               // Store fs7
    fsw     f24, 55*8(t0)                               // Store fs8
    fsw     f25, 56*8(t0)                               // Store fs9
    fsw     f26, 57*8(t0)                               // Store fs10
    fsw     f27, 58*8(t0)                               // Store fs11
#elif defined(__riscv_float_abi_double)
    fsd     f8,  39*8(t0)                               // Store fs0
    fsd     f9,  40*8(t0)                               // Store fs1
    fsd     f18, 49*8(t0)                               // Store fs2
    fsd     f19, 50*8(t0)                               // Store fs3
    fsd     f20, 51*8(t0)                               // Store fs4
    fsd     f21, 52*8(t0)                               // Store fs5
    fsd     f22, 53*8(t0)                               // Store fs6
    fsd     f23, 54*8(t0)                               // Store fs7
    fsd     f24, 55*8(t0)                               // Store fs8
    fsd     f25, 56*8(t0)                               // Store fs9
    fsd     f26, 57*8(t0)                               // Store fs10
    fsd     f27, 58*8(t0)                               // Store fs11
#endif

#if defined(__riscv_vector)
    /* Store vector registers and CSRs */
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    t1, t0, 64*8
#else
    addi    t1, t0, 31*8
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

    /* Store standard preserved registers.  */

    sd      x9,  11*8(t0)                               // Store s1
    sd      x18, 10*8(t0)                               // Store s2
    sd      x19,  9*8(t0)                               // Store s3
    sd      x20,  8*8(t0)                               // Store s4
    sd      x21,  7*8(t0)                               // Store s5
    sd      x22,  6*8(t0)                               // Store s6
    sd      x23,  5*8(t0)                               // Store s7
    sd      x24,  4*8(t0)                               // Store s8
    sd      x25,  3*8(t0)                               // Store s9
    sd      x26,  2*8(t0)                               // Store s10
    sd      x27,  1*8(t0)                               // Store s11
                                                        // Note: s0 is already stored!

    /* Save the remaining time-slice and disable it.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t0, _tx_timer_time_slice                    // Pickup time slice variable address
    lw      t2, 0(t0)                                   // Pickup time slice
    beqz    t2, _tx_thread_dont_save_ts                 // If 0, skip time slice processing

        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice
        _tx_timer_time_slice =  0;  */

    sw      t2, 36(t1)                                  // Save current time slice
    sw      x0, 0(t0)                                   // Clear global time slice


    /* }  */
_tx_thread_dont_save_ts:
    /* Clear the current task pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    /* Return to the scheduler.  */
    /* _tx_thread_schedule();  */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    sd      x0, 0(t0)                                   // Clear current thread pointer*/
    /* }  */

_tx_thread_idle_system_restore:
    /* Just return back to the scheduler!  */
    j       _tx_thread_schedule                         // Return to scheduler

/* }  */
