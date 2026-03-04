/***************************************************************************
 * Copyright (c) 2025 Quintauris
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
/*    _tx_thread_context_restore                         RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Francisco Merino, Quintauris                                        */
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

    csrci   mstatus, 0x08                               // Disable interrupts (MIE bit 3)

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
    flw     f0,  31*4(sp)                               // Recover ft0
    flw     f1,  32*4(sp)                               // Recover ft1
    flw     f2,  33*4(sp)                               // Recover ft2
    flw     f3,  34*4(sp)                               // Recover ft3
    flw     f4,  35*4(sp)                               // Recover ft4
    flw     f5,  36*4(sp)                               // Recover ft5
    flw     f6,  37*4(sp)                               // Recover ft6
    flw     f7,  38*4(sp)                               // Recover ft7
    flw     f10, 41*4(sp)                               // Recover fa0
    flw     f11, 42*4(sp)                               // Recover fa1
    flw     f12, 43*4(sp)                               // Recover fa2
    flw     f13, 44*4(sp)                               // Recover fa3
    flw     f14, 45*4(sp)                               // Recover fa4
    flw     f15, 46*4(sp)                               // Recover fa5
    flw     f16, 47*4(sp)                               // Recover fa6
    flw     f17, 48*4(sp)                               // Recover fa7
    flw     f28, 59*4(sp)                               // Recover ft8
    flw     f29, 60*4(sp)                               // Recover ft9
    flw     f30, 61*4(sp)                               // Recover ft10
    flw     f31, 62*4(sp)                               // Recover ft11
    lw      t0,  63*4(sp)                               // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#elif defined(__riscv_float_abi_double)
    fld     f0,  31*4(sp)                               // Recover ft0
    fld     f1,  32*4(sp)                               // Recover ft1
    fld     f2,  33*4(sp)                               // Recover ft2
    fld     f3,  34*4(sp)                               // Recover ft3
    fld     f4,  35*4(sp)                               // Recover ft4
    fld     f5,  36*4(sp)                               // Recover ft5
    fld     f6,  37*4(sp)                               // Recover ft6
    fld     f7,  38*4(sp)                               // Recover ft7
    fld     f10, 41*4(sp)                               // Recover fa0
    fld     f11, 42*4(sp)                               // Recover fa1
    fld     f12, 43*4(sp)                               // Recover fa2
    fld     f13, 44*4(sp)                               // Recover fa3
    fld     f14, 45*4(sp)                               // Recover fa4
    fld     f15, 46*4(sp)                               // Recover fa5
    fld     f16, 47*4(sp)                               // Recover fa6
    fld     f17, 48*4(sp)                               // Recover fa7
    fld     f28, 59*4(sp)                               // Recover ft8
    fld     f29, 60*4(sp)                               // Recover ft9
    fld     f30, 61*4(sp)                               // Recover ft10
    fld     f31, 62*4(sp)                               // Recover ft11
    lw      t0,  63*4(sp)                               // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#endif

    /* Recover standard registers.  */

    /* Restore registers,
       Skip global pointer because that does not change.
       Also skip the saved registers since they have been restored by any function we called,
       except s0 since we use it ourselves. */

    lw      t0, 30*4(sp)                                // Recover mepc
    csrw    mepc, t0                                    // Setup mepc

    /* Compose mstatus via read/modify/write to avoid clobbering unrelated bits.
       Set MPIE and restore MPP to Machine, preserve other fields. */

    csrr    t1, mstatus

    /* Clear MPP/MPIE/MIE bits in t1 then set desired values. */

    li      t2, 0x1888                                  // MPP(0x1800) | MPIE(0x80) | MIE(0x08)
    li      t3, 0x1800                                  // Set MPP to Machine mode (bits 12:11)

    /* Construct new mstatus in t1: clear mask bits, set MPP/MPIE and optionally FP bit,
       preserve everything except the bits we will modify.  */

    li      t4, ~0x1888                                 // Clear mask for MPP/MPIE/MIE
    and     t1, t1, t4
    or      t1, t1, t3

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t0, 0x2000                                  // Set FS bits (bits 14:13 to 01) for FP state
    or      t1, t1, t0
#endif
    csrw    mstatus, t1                                 // Update mstatus safely

    lw      ra,  28*4(sp)                               // Recover return address
    lw      t0,  19*4(sp)                               // Recover t0
    lw      t1,  18*4(sp)                               // Recover t1
    lw      t2,  17*4(sp)                               // Recover t2
    lw      s0,  12*4(sp)                               // Recover s0
    lw      a0,  27*4(sp)                               // Recover a0
    lw      a1,  26*4(sp)                               // Recover a1
    lw      a2,  25*4(sp)                               // Recover a2
    lw      a3,  24*4(sp)                               // Recover a3
    lw      a4,  23*4(sp)                               // Recover a4
    lw      a5,  22*4(sp)                               // Recover a5
    lw      a6,  21*4(sp)                               // Recover a6
    lw      a7,  20*4(sp)                               // Recover a7
    lw      t3,  16*4(sp)                               // Recover t3
    lw      t4,  15*4(sp)                               // Recover t4
    lw      t5,  14*4(sp)                               // Recover t5
    lw      t6,  13*4(sp)                               // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*4                                // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*4                                // Recover stack frame - without floating point enabled
#endif
    mret                                                // Return to point of interrupt

    /* }  */
_tx_thread_not_nested_restore:
    /* Determine if a thread was interrupted and no preemption is required.  */
    /* else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr)
               || (_tx_thread_preempt_disable))
    {  */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    lw      t1, 0(t0)                                   // Pickup current thread pointer

    beqz    t1, _tx_thread_idle_system_restore          // If NULL, idle system restore


    la      t0, _tx_thread_preempt_disable              // Pickup preempt disable flag address
    lw      t2, 0(t0)                                   // Pickup preempt disable flag (UINT)

    bgtz    t2, _tx_thread_no_preempt_restore           // If set, restore interrupted thread


    la      t0, _tx_thread_execute_ptr                  // Pickup thread execute pointer address
    lw      t2, 0(t0)                                   // Pickup thread execute pointer

    bne     t1, t2, _tx_thread_preempt_restore          // If higher-priority thread is ready, preempt


_tx_thread_no_preempt_restore:
    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    /* sp =  _tx_thread_current_ptr -> tx_thread_stack_ptr;  */

    lw      sp, 8(t1)                                   // Switch back to thread's stack

    /* Recover floating point registers.  */
#if defined(__riscv_float_abi_single)
    flw     f0,  31*4(sp)                               // Recover ft0
    flw     f1,  32*4(sp)                               // Recover ft1
    flw     f2,  33*4(sp)                               // Recover ft2
    flw     f3,  34*4(sp)                               // Recover ft3
    flw     f4,  35*4(sp)                               // Recover ft4
    flw     f5,  36*4(sp)                               // Recover ft5
    flw     f6,  37*4(sp)                               // Recover ft6
    flw     f7,  38*4(sp)                               // Recover ft7
    flw     f10, 41*4(sp)                               // Recover fa0
    flw     f11, 42*4(sp)                               // Recover fa1
    flw     f12, 43*4(sp)                               // Recover fa2
    flw     f13, 44*4(sp)                               // Recover fa3
    flw     f14, 45*4(sp)                               // Recover fa4
    flw     f15, 46*4(sp)                               // Recover fa5
    flw     f16, 47*4(sp)                               // Recover fa6
    flw     f17, 48*4(sp)                               // Recover fa7
    flw     f28, 59*4(sp)                               // Recover ft8
    flw     f29, 60*4(sp)                               // Recover ft9
    flw     f30, 61*4(sp)                               // Recover ft10
    flw     f31, 62*4(sp)                               // Recover ft11
    lw      t0,  63*4(sp)                               // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#elif defined(__riscv_float_abi_double)
    fld     f0,  31*4(sp)                               // Recover ft0
    fld     f1,  32*4(sp)                               // Recover ft1
    fld     f2,  33*4(sp)                               // Recover ft2
    fld     f3,  34*4(sp)                               // Recover ft3
    fld     f4,  35*4(sp)                               // Recover ft4
    fld     f5,  36*4(sp)                               // Recover ft5
    fld     f6,  37*4(sp)                               // Recover ft6
    fld     f7,  38*4(sp)                               // Recover ft7
    fld     f10, 41*4(sp)                               // Recover fa0
    fld     f11, 42*4(sp)                               // Recover fa1
    fld     f12, 43*4(sp)                               // Recover fa2
    fld     f13, 44*4(sp)                               // Recover fa3
    fld     f14, 45*4(sp)                               // Recover fa4
    fld     f15, 46*4(sp)                               // Recover fa5
    fld     f16, 47*4(sp)                               // Recover fa6
    fld     f17, 48*4(sp)                               // Recover fa7
    fld     f28, 59*4(sp)                               // Recover ft8
    fld     f29, 60*4(sp)                               // Recover ft9
    fld     f30, 61*4(sp)                               // Recover ft10
    fld     f31, 62*4(sp)                               // Recover ft11
    lw      t0,  63*4(sp)                               // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#endif

    /* Recover the saved context and return to the point of interrupt.  */

    /* Recover standard registers.  */
    /* Restore registers,
       Skip global pointer because that does not change */

    lw      t0, 30*4(sp)                                // Recover mepc
    csrw    mepc, t0                                    // Setup mepc

    /* Compose mstatus via read/modify/write to avoid clobbering unrelated bits. */

    csrr    t1, mstatus
    li      t2, 0x1888                                  // MPP(0x1800) | MPIE(0x80) | MIE(0x08)
    li      t3, 0x1800                                  // Set MPP to Machine mode
    li      t4, ~0x1888                                 // Clear mask for MPP/MPIE/MIE
    and     t1, t1, t4
    or      t1, t1, t3

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t0, 0x2000                                  // Set FS bits for FP state
    or      t1, t1, t0
#endif
    csrw    mstatus, t1                                 // Update mstatus safely

    lw      ra,  28*4(sp)                               // Recover return address
    lw      t0,  19*4(sp)                               // Recover t0
    lw      t1,  18*4(sp)                               // Recover t1
    lw      t2,  17*4(sp)                               // Recover t2
    lw      s0,  12*4(sp)                               // Recover s0
    lw      a0,  27*4(sp)                               // Recover a0
    lw      a1,  26*4(sp)                               // Recover a1
    lw      a2,  25*4(sp)                               // Recover a2
    lw      a3,  24*4(sp)                               // Recover a3
    lw      a4,  23*4(sp)                               // Recover a4
    lw      a5,  22*4(sp)                               // Recover a5
    lw      a6,  21*4(sp)                               // Recover a6
    lw      a7,  20*4(sp)                               // Recover a7
    lw      t3,  16*4(sp)                               // Recover t3
    lw      t4,  15*4(sp)                               // Recover t4
    lw      t5,  14*4(sp)                               // Recover t5
    lw      t6,  13*4(sp)                               // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*4                                // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*4                                // Recover stack frame - without floating point enabled
#endif
    mret                                                // Return to point of interrupt

    /* }
    else
    {  */
_tx_thread_preempt_restore:
    /* Instead of directly activating the thread again, ensure we save the
       entire stack frame by saving the remaining registers. */

    lw      t0, 8(t1)                                   // Pickup thread's stack pointer
    ori     t3, zero, 1                                 // Build interrupt stack type
    sw      t3, 0(t0)                                   // Store stack type

    /* Store floating point preserved registers.  */
#ifdef __riscv_float_abi_single
    fsw     f8,  39*4(t0)                               // Store fs0
    fsw     f9,  40*4(t0)                               // Store fs1
    fsw     f18, 49*4(t0)                               // Store fs2
    fsw     f19, 50*4(t0)                               // Store fs3
    fsw     f20, 51*4(t0)                               // Store fs4
    fsw     f21, 52*4(t0)                               // Store fs5
    fsw     f22, 53*4(t0)                               // Store fs6
    fsw     f23, 54*4(t0)                               // Store fs7
    fsw     f24, 55*4(t0)                               // Store fs8
    fsw     f25, 56*4(t0)                               // Store fs9
    fsw     f26, 57*4(t0)                               // Store fs10
    fsw     f27, 58*4(t0)                               // Store fs11
#elif defined(__riscv_float_abi_double)
    fsd     f8,  39*4(t0)                               // Store fs0
    fsd     f9,  40*4(t0)                               // Store fs1
    fsd     f18, 49*4(t0)                               // Store fs2
    fsd     f19, 50*4(t0)                               // Store fs3
    fsd     f20, 51*4(t0)                               // Store fs4
    fsd     f21, 52*4(t0)                               // Store fs5
    fsd     f22, 53*4(t0)                               // Store fs6
    fsd     f23, 54*4(t0)                               // Store fs7
    fsd     f24, 55*4(t0)                               // Store fs8
    fsd     f25, 56*4(t0)                               // Store fs9
    fsd     f26, 57*4(t0)                               // Store fs10
    fsd     f27, 58*4(t0)                               // Store fs11
#endif

    /* Store standard preserved registers.  */

    sw      x9,  11*4(t0)                               // Store s1
    sw      x18, 10*4(t0)                               // Store s2
    sw      x19,  9*4(t0)                               // Store s3
    sw      x20,  8*4(t0)                               // Store s4
    sw      x21,  7*4(t0)                               // Store s5
    sw      x22,  6*4(t0)                               // Store s6
    sw      x23,  5*4(t0)                               // Store s7
    sw      x24,  4*4(t0)                               // Store s8
    sw      x25,  3*4(t0)                               // Store s9
    sw      x26,  2*4(t0)                               // Store s10
    sw      x27,  1*4(t0)                               // Store s11
                                                        // Note: s0 is already stored!

    /* Save the remaining time-slice and disable it.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t0, _tx_timer_time_slice                    // Pickup time slice variable address
    lw      t2, 0(t0)                                   // Pickup time slice
    beqz    t2, _tx_thread_dont_save_ts                 // If 0, skip time slice processing

        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice
        _tx_timer_time_slice =  0;  */

    sw      t2, 24(t1)                                  // Save current time slice
    sw      x0, 0(t0)                                   // Clear global time slice


    /* }  */
_tx_thread_dont_save_ts:
    /* Clear the current task pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    /* Return to the scheduler.  */
    /* _tx_thread_schedule();  */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    sw      x0, 0(t0)                                   // Clear current thread pointer

    /* }  */

_tx_thread_idle_system_restore:
    /* Just return back to the scheduler!  */
    j       _tx_thread_schedule                         // Return to scheduler

/* }  */
