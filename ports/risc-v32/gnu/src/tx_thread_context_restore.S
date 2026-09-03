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

/* Trap frame layout (stack grows down, all totals mod 16 = 0):
   FP build (double ABI): integer slots 0-31 at i*4 (0-127; slot 31 is pad),
   f0-f31 as doubles at 128 + i*8 (128-383), fcsr at 384, pad 388-399.
   SOFT_FLOAT build: integer slots 0-31 at i*4 only.  */
#if defined(__riscv_float_abi_double)
    .equ    TX_TRAP_FRAME_SIZE, 400
#else
    .equ    TX_TRAP_FRAME_SIZE, 128
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_restore                         RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
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

    /* Recover floating point registers only if saved mstatus.FS was not Off.  */
#if defined(__riscv_float_abi_double)
    lw      t1, 29*4(sp)                                // Pickup saved mstatus
    srli    t1, t1, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_skip_fp_restore              // Skip if FS was Off
    fld     f0,  16*8(sp)                                // Recover ft0
    fld     f1,  17*8(sp)                                // Recover ft1
    fld     f2,  18*8(sp)                                // Recover ft2
    fld     f3,  19*8(sp)                                // Recover ft3
    fld     f4,  20*8(sp)                                // Recover ft4
    fld     f5,  21*8(sp)                                // Recover ft5
    fld     f6,  22*8(sp)                                // Recover ft6
    fld     f7,  23*8(sp)                                // Recover ft7
    fld     f10, 26*8(sp)                                // Recover fa0
    fld     f11, 27*8(sp)                                // Recover fa1
    fld     f12, 28*8(sp)                                // Recover fa2
    fld     f13, 29*8(sp)                                // Recover fa3
    fld     f14, 30*8(sp)                                // Recover fa4
    fld     f15, 31*8(sp)                                // Recover fa5
    fld     f16, 32*8(sp)                                // Recover fa6
    fld     f17, 33*8(sp)                                // Recover fa7
    fld     f28, 44*8(sp)                                // Recover ft8
    fld     f29, 45*8(sp)                                // Recover ft9
    fld     f30, 46*8(sp)                                // Recover ft10
    fld     f31, 47*8(sp)                                // Recover ft11
    lw      t0,  96*4(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#endif
_tx_thread_skip_fp_restore:

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
    li      t3, 0x1880                                  // Set MPP=Machine (0x1800) + MPIE=1 (0x80); mret sets MIE=MPIE

    /* Construct new mstatus in t1: clear mask bits, set MPP/MPIE and optionally FP bit,
       preserve everything except the bits we will modify.  */

    li      t4, ~0x1888                                 // Clear mask for MPP/MPIE/MIE
    and     t1, t1, t4
    or      t1, t1, t3

#if defined(__riscv_float_abi_double)
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

    addi    sp, sp, TX_TRAP_FRAME_SIZE                  // Recover stack frame
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

    bnez    t2, _tx_thread_no_preempt_restore           // If set, restore interrupted thread


    la      t0, _tx_thread_execute_ptr                  // Pickup thread execute pointer address
    lw      t2, 0(t0)                                   // Pickup thread execute pointer

    bne     t1, t2, _tx_thread_preempt_restore          // If higher-priority thread is ready, preempt


_tx_thread_no_preempt_restore:
    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    /* sp =  _tx_thread_current_ptr -> tx_thread_stack_ptr;  */

    lw      sp, 8(t1)                                   // Switch back to thread's stack

    /* Recover floating point registers only if saved mstatus.FS was not Off.  */
#if defined(__riscv_float_abi_double)
    lw      t3, 29*4(sp)                                // Pickup saved mstatus
    srli    t3, t3, 13
    andi    t3, t3, 0x3
    beqz    t3, _tx_thread_no_preempt_skip_fp_restore   // Skip if FS was Off
    fld     f0,  16*8(sp)                                // Recover ft0
    fld     f1,  17*8(sp)                                // Recover ft1
    fld     f2,  18*8(sp)                                // Recover ft2
    fld     f3,  19*8(sp)                                // Recover ft3
    fld     f4,  20*8(sp)                                // Recover ft4
    fld     f5,  21*8(sp)                                // Recover ft5
    fld     f6,  22*8(sp)                                // Recover ft6
    fld     f7,  23*8(sp)                                // Recover ft7
    fld     f10, 26*8(sp)                                // Recover fa0
    fld     f11, 27*8(sp)                                // Recover fa1
    fld     f12, 28*8(sp)                                // Recover fa2
    fld     f13, 29*8(sp)                                // Recover fa3
    fld     f14, 30*8(sp)                                // Recover fa4
    fld     f15, 31*8(sp)                                // Recover fa5
    fld     f16, 32*8(sp)                                // Recover fa6
    fld     f17, 33*8(sp)                                // Recover fa7
    fld     f28, 44*8(sp)                                // Recover ft8
    fld     f29, 45*8(sp)                                // Recover ft9
    fld     f30, 46*8(sp)                                // Recover ft10
    fld     f31, 47*8(sp)                                // Recover ft11
    lw      t0,  96*4(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#endif
_tx_thread_no_preempt_skip_fp_restore:

    /* Recover the saved context and return to the point of interrupt.  */

    /* Recover standard registers.  */
    /* Restore registers,
       Skip global pointer because that does not change */

    lw      t0, 30*4(sp)                                // Recover mepc
    csrw    mepc, t0                                    // Setup mepc

    /* Compose mstatus via read/modify/write to avoid clobbering unrelated bits. */

    csrr    t1, mstatus
    li      t2, 0x1888                                  // MPP(0x1800) | MPIE(0x80) | MIE(0x08)
    li      t3, 0x1880                                  // Set MPP=Machine (0x1800) + MPIE=1 (0x80); mret sets MIE=MPIE
    li      t4, ~0x1888                                 // Clear mask for MPP/MPIE/MIE
    and     t1, t1, t4
    or      t1, t1, t3

#if defined(__riscv_float_abi_double)
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

    addi    sp, sp, TX_TRAP_FRAME_SIZE                  // Recover stack frame
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

    /* Store floating point preserved registers only if saved mstatus.FS was not Off.  */
#if defined(__riscv_float_abi_double)
    lw      t3, 29*4(t0)                                // Pickup saved mstatus
    srli    t3, t3, 13
    andi    t3, t3, 0x3
    beqz    t3, _tx_thread_preempt_skip_fp_restore      // Skip if FS was Off
    fsd     f8,  192(t0)                                // Store fs0
    fsd     f9,  200(t0)                                // Store fs1
    fsd     f18, 272(t0)                                // Store fs2
    fsd     f19, 280(t0)                                // Store fs3
    fsd     f20, 288(t0)                                // Store fs4
    fsd     f21, 296(t0)                                // Store fs5
    fsd     f22, 304(t0)                                // Store fs6
    fsd     f23, 312(t0)                                // Store fs7
    fsd     f24, 320(t0)                                // Store fs8
    fsd     f25, 328(t0)                                // Store fs9
    fsd     f26, 336(t0)                                // Store fs10
    fsd     f27, 344(t0)                                // Store fs11
#endif
_tx_thread_preempt_skip_fp_restore:

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
