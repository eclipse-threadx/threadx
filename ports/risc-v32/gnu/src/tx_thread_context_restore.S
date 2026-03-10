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
/*    _tx_thread_context_restore                         RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  23-12-2025        Akif Ejaz            Initial Version 6.4.x          */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_context_restore(VOID)
{  */
    .global  _tx_thread_context_restore
_tx_thread_context_restore:

    /* Lockout interrupts.  */

    csrci   mstatus, 0x08                               // Disable interrupts

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    call    _tx_execution_isr_exit                      // Call the ISR execution exit function
#endif

    /* Determine if interrupts are nested.  */
    /* if (--_tx_thread_system_state)
    {  */

    la      t0, _tx_thread_system_state                 // Pickup addr of nested interrupt count
    LOAD    t1, 0(t0)                                   // Pickup nested interrupt count
    addi    t1, t1, -1                                  // Decrement the nested interrupt counter
    STORE   t1, 0(t0)                                   // Store new nested count
    beqz    t1, _tx_thread_not_nested_restore           // If 0, not nested restore

    /* Interrupts are nested.  */

    /* Just recover the saved registers and return to the point of
       interrupt.  */

    /* Recover floating point registers.  */
#if defined(__riscv_flen)
    LOAD    t1, TX_STACK_OFFSET_MSTATUS(t0)             // Pickup thread's floating point state */
    /* Check if floating point is enabled */
    srli    t1, t1, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_skip_fp_restore              // Skip floating point restore FS is Off
#if __riscv_flen == 32
    flw     f0, TX_STACK_OFFSET_F0(sp)                  // Recover ft0
    flw     f1, TX_STACK_OFFSET_F1(sp)                  // Recover ft1
    flw     f2, TX_STACK_OFFSET_F2(sp)                  // Recover ft2
    flw     f3, TX_STACK_OFFSET_F3(sp)                  // Recover ft3
    flw     f4, TX_STACK_OFFSET_F4(sp)                  // Recover ft4
    flw     f5, TX_STACK_OFFSET_F5(sp)                  // Recover ft5
    flw     f6, TX_STACK_OFFSET_F6(sp)                  // Recover ft6
    flw     f7, TX_STACK_OFFSET_F7(sp)                  // Recover ft7
    flw     f10,TX_STACK_OFFSET_F10(sp)                 // Recover fa0
    flw     f11,TX_STACK_OFFSET_F11(sp)                 // Recover fa1
    flw     f12,TX_STACK_OFFSET_F12(sp)                 // Recover fa2
    flw     f13,TX_STACK_OFFSET_F13(sp)                 // Recover fa3
    flw     f14,TX_STACK_OFFSET_F14(sp)                 // Recover fa4
    flw     f15,TX_STACK_OFFSET_F15(sp)                 // Recover fa5
    flw     f16,TX_STACK_OFFSET_F16(sp)                 // Recover fa6
    flw     f17,TX_STACK_OFFSET_F17(sp)                 // Recover fa7
    flw     f28,TX_STACK_OFFSET_F28(sp)                 // Recover ft8
    flw     f29,TX_STACK_OFFSET_F29(sp)                 // Recover ft9
    flw     f30,TX_STACK_OFFSET_F30(sp)                 // Recover ft10
    flw     f31,TX_STACK_OFFSET_F31(sp)                 // Recover ft11
    lw      t0, TX_STACK_OFFSET_FCSR(sp)                // Recover fcsr
    csrw    fcsr, t0                                    //
#elif __riscv_flen == 64
    fld     f0, TX_STACK_OFFSET_F0(sp)                  // Recover ft0
    fld     f1, TX_STACK_OFFSET_F1(sp)                  // Recover ft1
    fld     f2, TX_STACK_OFFSET_F2(sp)                  // Recover ft2
    fld     f3, TX_STACK_OFFSET_F3(sp)                  // Recover ft3
    fld     f4, TX_STACK_OFFSET_F4(sp)                  // Recover ft4
    fld     f5, TX_STACK_OFFSET_F5(sp)                  // Recover ft5
    fld     f6, TX_STACK_OFFSET_F6(sp)                  // Recover ft6
    fld     f7, TX_STACK_OFFSET_F7(sp)                  // Recover ft7
    fld     f10,TX_STACK_OFFSET_F10(sp)                 // Recover fa0
    fld     f11,TX_STACK_OFFSET_F11(sp)                 // Recover fa1
    fld     f12,TX_STACK_OFFSET_F12(sp)                 // Recover fa2
    fld     f13,TX_STACK_OFFSET_F13(sp)                 // Recover fa3
    fld     f14,TX_STACK_OFFSET_F14(sp)                 // Recover fa4
    fld     f15,TX_STACK_OFFSET_F15(sp)                 // Recover fa5
    fld     f16,TX_STACK_OFFSET_F16(sp)                 // Recover fa6
    fld     f17,TX_STACK_OFFSET_F17(sp)                 // Recover fa7
    fld     f28,TX_STACK_OFFSET_F28(sp)                 // Recover ft8
    fld     f29,TX_STACK_OFFSET_F29(sp)                 // Recover ft9
    fld     f30,TX_STACK_OFFSET_F30(sp)                 // Recover ft10
    fld     f31,TX_STACK_OFFSET_F31(sp)                 // Recover ft11
    LOAD    t0, TX_STACK_OFFSET_FCSR(sp)                // Recover fcsr
    csrw    fcsr, t0                                    //
#endif
_tx_thread_skip_fp_restore:
#endif

    /* Recover standard registers.  */

    /* Restore registers,
       Skip global pointer because that does not change.
       Also skip the saved registers since they have been restored by any function we called,
       except s0 since we use it ourselves. */

    LOAD    t0, TX_STACK_OFFSET_MEPC(sp)                // Recover mepc
    csrw    mepc, t0                                    // Setup mepc
    li      t0, 0x1880                                  // Prepare MPIP
#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    li      t1, 1<<13
    or      t0, t1, t0
#endif
    csrw    mstatus, t0                                 // Enable MPIP

    LOAD    x1,  TX_STACK_OFFSET_X1(sp)                 // Recover RA
    LOAD    x5,  TX_STACK_OFFSET_X5(sp)                 // Recover t0
    LOAD    x6,  TX_STACK_OFFSET_X6(sp)                 // Recover t1
    LOAD    x7,  TX_STACK_OFFSET_X7(sp)                 // Recover t2
    LOAD    x8,  TX_STACK_OFFSET_X8(sp)                 // Recover s0
    LOAD    x10, TX_STACK_OFFSET_X10(sp)                // Recover a0
    LOAD    x11, TX_STACK_OFFSET_X11(sp)                // Recover a1
    LOAD    x12, TX_STACK_OFFSET_X12(sp)                // Recover a2
    LOAD    x13, TX_STACK_OFFSET_X13(sp)                // Recover a3
    LOAD    x14, TX_STACK_OFFSET_X14(sp)                // Recover a4
    LOAD    x15, TX_STACK_OFFSET_X15(sp)                // Recover a5
    LOAD    x16, TX_STACK_OFFSET_X16(sp)                // Recover a6
    LOAD    x17, TX_STACK_OFFSET_X17(sp)                // Recover a7
    LOAD    x28, TX_STACK_OFFSET_X28(sp)                // Recover t3
    LOAD    x29, TX_STACK_OFFSET_X29(sp)                // Recover t4
    LOAD    x30, TX_STACK_OFFSET_X30(sp)                // Recover t5
    LOAD    x31, TX_STACK_OFFSET_X31(sp)                // Recover t6

#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, TX_THREAD_FRAME_SIZE_FPU            // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, TX_THREAD_FRAME_SIZE_INT            // Recover stack frame - without floating point enabled
#endif
    mret                                                // Return to point of interrupt

    /* }  */
_tx_thread_not_nested_restore:
    /* Determine if a thread was interrupted and no preemption is required.  */
    /* else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr)
               || (_tx_thread_preempt_disable))
    {  */

    LOAD    t1, _tx_thread_current_ptr                  // Pickup current thread pointer
    beqz    t1, _tx_thread_idle_system_restore          // If NULL, idle system restore

    LOAD    t2, _tx_thread_preempt_disable              // Pickup preempt disable flag
    bgtz    t2, _tx_thread_no_preempt_restore           // If set, restore interrupted thread

    LOAD    t2, _tx_thread_execute_ptr                  // Pickup thread execute pointer
    bne     t1, t2, _tx_thread_preempt_restore          // If higher-priority thread is ready, preempt


_tx_thread_no_preempt_restore:
    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    /* SP =  _tx_thread_current_ptr -> tx_thread_stack_ptr;  */

    LOAD    sp, TX_THREAD_STACK_PTR(t1)                 // Switch back to thread's stack

    /* Recover floating point registers.  */
#if defined(__riscv_flen)
    LOAD    t1, TX_STACK_OFFSET_MSTATUS(t0)             // Pickup thread's floating point state */
    /* Check if floating point is enabled */
    srli    t1, t1, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_no_preempt_skip_fp_restore   // Skip floating point restore FS is Off
#if __riscv_flen == 32
    flw     f0, TX_STACK_OFFSET_F0(sp)                  // Recover ft0
    flw     f1, TX_STACK_OFFSET_F1(sp)                  // Recover ft1
    flw     f2, TX_STACK_OFFSET_F2(sp)                  // Recover ft2
    flw     f3, TX_STACK_OFFSET_F3(sp)                  // Recover ft3
    flw     f4, TX_STACK_OFFSET_F4(sp)                  // Recover ft4
    flw     f5, TX_STACK_OFFSET_F5(sp)                  // Recover ft5
    flw     f6, TX_STACK_OFFSET_F6(sp)                  // Recover ft6
    flw     f7, TX_STACK_OFFSET_F7(sp)                  // Recover ft7
    flw     f10,TX_STACK_OFFSET_F10(sp)                 // Recover fa0
    flw     f11,TX_STACK_OFFSET_F11(sp)                 // Recover fa1
    flw     f12,TX_STACK_OFFSET_F12(sp)                 // Recover fa2
    flw     f13,TX_STACK_OFFSET_F13(sp)                 // Recover fa3
    flw     f14,TX_STACK_OFFSET_F14(sp)                 // Recover fa4
    flw     f15,TX_STACK_OFFSET_F15(sp)                 // Recover fa5
    flw     f16,TX_STACK_OFFSET_F16(sp)                 // Recover fa6
    flw     f17,TX_STACK_OFFSET_F17(sp)                 // Recover fa7
    flw     f28,TX_STACK_OFFSET_F28(sp)                 // Recover ft8
    flw     f29,TX_STACK_OFFSET_F29(sp)                 // Recover ft9
    flw     f30,TX_STACK_OFFSET_F30(sp)                 // Recover ft10
    flw     f31,TX_STACK_OFFSET_F31(sp)                 // Recover ft11
    lw      t0, TX_STACK_OFFSET_FCSR(sp)                // Recover fcsr
    csrw    fcsr, t0                                    //
#elif __riscv_flen == 64
    fld     f0, TX_STACK_OFFSET_F0(sp)                  // Recover ft0
    fld     f1, TX_STACK_OFFSET_F1(sp)                  // Recover ft1
    fld     f2, TX_STACK_OFFSET_F2(sp)                  // Recover ft2
    fld     f3, TX_STACK_OFFSET_F3(sp)                  // Recover ft3
    fld     f4, TX_STACK_OFFSET_F4(sp)                  // Recover ft4
    fld     f5, TX_STACK_OFFSET_F5(sp)                  // Recover ft5
    fld     f6, TX_STACK_OFFSET_F6(sp)                  // Recover ft6
    fld     f7, TX_STACK_OFFSET_F7(sp)                  // Recover ft7
    fld     f10,TX_STACK_OFFSET_F10(sp)                 // Recover fa0
    fld     f11,TX_STACK_OFFSET_F11(sp)                 // Recover fa1
    fld     f12,TX_STACK_OFFSET_F12(sp)                 // Recover fa2
    fld     f13,TX_STACK_OFFSET_F13(sp)                 // Recover fa3
    fld     f14,TX_STACK_OFFSET_F14(sp)                 // Recover fa4
    fld     f15,TX_STACK_OFFSET_F15(sp)                 // Recover fa5
    fld     f16,TX_STACK_OFFSET_F16(sp)                 // Recover fa6
    fld     f17,TX_STACK_OFFSET_F17(sp)                 // Recover fa7
    fld     f28,TX_STACK_OFFSET_F28(sp)                 // Recover ft8
    fld     f29,TX_STACK_OFFSET_F29(sp)                 // Recover ft9
    fld     f30,TX_STACK_OFFSET_F30(sp)                 // Recover ft10
    fld     f31,TX_STACK_OFFSET_F31(sp)                 // Recover ft11
    LOAD    t0, TX_STACK_OFFSET_FCSR(sp)                // Recover fcsr
    csrw    fcsr, t0                                    //
#endif
_tx_thread_no_preempt_skip_fp_restore:
#endif

    /* Recover the saved context and return to the point of interrupt.  */

    /* Recover standard registers.  */
    /* Restore registers,
       Skip global pointer because that does not change */

    LOAD    t0, TX_STACK_OFFSET_MEPC(sp)                // Recover mepc
    csrw    mepc, t0                                    // Setup mepc
    li      t0, 0x1880                                  // Prepare MPIP
#if defined(__riscv_flen)
    li      t1, 1<<13
    or      t0, t1, t0
#endif
    csrw    mstatus, t0                                 // Enable MPIP

    LOAD    x1,  TX_STACK_OFFSET_X1(sp)                 // Recover RA
    LOAD    x5,  TX_STACK_OFFSET_X5(sp)                 // Recover t0
    LOAD    x6,  TX_STACK_OFFSET_X6(sp)                 // Recover t1
    LOAD    x7,  TX_STACK_OFFSET_X7(sp)                 // Recover t2
    LOAD    x8,  TX_STACK_OFFSET_X8(sp)                 // Recover s0
    LOAD    x10, TX_STACK_OFFSET_X10(sp)                // Recover a0
    LOAD    x11, TX_STACK_OFFSET_X11(sp)                // Recover a1
    LOAD    x12, TX_STACK_OFFSET_X12(sp)                // Recover a2
    LOAD    x13, TX_STACK_OFFSET_X13(sp)                // Recover a3
    LOAD    x14, TX_STACK_OFFSET_X14(sp)                // Recover a4
    LOAD    x15, TX_STACK_OFFSET_X15(sp)                // Recover a5
    LOAD    x16, TX_STACK_OFFSET_X16(sp)                // Recover a6
    LOAD    x17, TX_STACK_OFFSET_X17(sp)                // Recover a7
    LOAD    x28, TX_STACK_OFFSET_X28(sp)                // Recover t3
    LOAD    x29, TX_STACK_OFFSET_X29(sp)                // Recover t4
    LOAD    x30, TX_STACK_OFFSET_X30(sp)                // Recover t5
    LOAD    x31, TX_STACK_OFFSET_X31(sp)                // Recover t6

#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, TX_THREAD_FRAME_SIZE_FPU            // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, TX_THREAD_FRAME_SIZE_INT            // Recover stack frame - without floating point enabled
#endif
    mret                                                // Return to point of interrupt

    /* }
    else
    {  */
_tx_thread_preempt_restore:
    /* Instead of directly activating the thread again, ensure we save the
       entire stack frame by saving the remaining registers. */

    LOAD    t0, TX_THREAD_STACK_PTR(t1)                 // Pickup thread's stack pointer
    ori     t3, x0, 1                                   // Build interrupt stack type
    STORE   t3, 0(t0)                                   // Store stack type

    /* Store floating point preserved registers.  */
#if defined(__riscv_flen)
    LOAD    t2, TX_STACK_OFFSET_MSTATUS(t0)             // Pickup thread's floating point state */
    /* Check if floating point is enabled */
    srli    t2, t2, 13
    andi    t2, t2, 0x3
    beqz    t2, _tx_thread_preempt_skip_fp_restore      // Skip floating point restore FS is Off
#if __riscv_flen == 32
    fsw     f8,  TX_STACK_OFFSET_F8(t0)                 // Store fs0
    fsw     f9,  TX_STACK_OFFSET_F9(t0)                 // Store fs1
    fsw     f18, TX_STACK_OFFSET_F18(t0)                // Store fs2
    fsw     f19, TX_STACK_OFFSET_F19(t0)                // Store fs3
    fsw     f20, TX_STACK_OFFSET_F20(t0)                // Store fs4
    fsw     f21, TX_STACK_OFFSET_F21(t0)                // Store fs5
    fsw     f22, TX_STACK_OFFSET_F22(t0)                // Store fs6
    fsw     f23, TX_STACK_OFFSET_F23(t0)                // Store fs7
    fsw     f24, TX_STACK_OFFSET_F24(t0)                // Store fs8
    fsw     f25, TX_STACK_OFFSET_F25(t0)                // Store fs9
    fsw     f26, TX_STACK_OFFSET_F26(t0)                // Store fs10
    fsw     f27, TX_STACK_OFFSET_F27(t0)                // Store fs11
#elif __riscv_flen == 64
    fsd     f8,  TX_STACK_OFFSET_F8(t0)                 // Store fs0
    fsd     f9,  TX_STACK_OFFSET_F9(t0)                 // Store fs1
    fsd     f18, TX_STACK_OFFSET_F18(t0)                // Store fs2
    fsd     f19, TX_STACK_OFFSET_F19(t0)                // Store fs3
    fsd     f20, TX_STACK_OFFSET_F20(t0)                // Store fs4
    fsd     f21, TX_STACK_OFFSET_F21(t0)                // Store fs5
    fsd     f22, TX_STACK_OFFSET_F22(t0)                // Store fs6
    fsd     f23, TX_STACK_OFFSET_F23(t0)                // Store fs7
    fsd     f24, TX_STACK_OFFSET_F24(t0)                // Store fs8
    fsd     f25, TX_STACK_OFFSET_F25(t0)                // Store fs9
    fsd     f26, TX_STACK_OFFSET_F26(t0)                // Store fs10
    fsd     f27, TX_STACK_OFFSET_F27(t0)                // Store fs11
#endif
_tx_thread_preempt_skip_fp_restore:
#endif

    /* Store standard preserved registers.  */

    STORE   x9,  TX_STACK_OFFSET_X9(t0)                 // Store s1
    STORE   x18, TX_STACK_OFFSET_X18(t0)                // Store s2
    STORE   x19, TX_STACK_OFFSET_X19(t0)                // Store s3
    STORE   x20, TX_STACK_OFFSET_X20(t0)                // Store s4
    STORE   x21, TX_STACK_OFFSET_X21(t0)                // Store s5
    STORE   x22, TX_STACK_OFFSET_X22(t0)                // Store s6
    STORE   x23, TX_STACK_OFFSET_X23(t0)                // Store s7
    STORE   x24, TX_STACK_OFFSET_X24(t0)                // Store s8
    STORE   x25, TX_STACK_OFFSET_X25(t0)                // Store s9
    STORE   x26, TX_STACK_OFFSET_X26(t0)                // Store s10
    STORE   x27, TX_STACK_OFFSET_X27(t0)                // Store s11
                                                        // Note: s0 is already stored!

    /* Save the remaining time-slice and disable it.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t0, _tx_timer_time_slice                    // Pickup time slice variable address
    LOAD    t2, 0(t0)                                   // Pickup time slice
    beqz    t2, _tx_thread_dont_save_ts                 // If 0, skip time slice processing

        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice
        _tx_timer_time_slice =  0;  */

    STORE   t2, TX_THREAD_TIME_SLICE(t1)                // Save current time slice
    STORE   x0, 0(t0)                                   // Clear global time slice


    /* }  */
_tx_thread_dont_save_ts:
    /* Clear the current task pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    /* Return to the scheduler.  */
    /* _tx_thread_schedule();  */

    STORE   x0, _tx_thread_current_ptr, t0              // Clear current thread pointer*/
    /* }  */

_tx_thread_idle_system_restore:
    /* Just return back to the scheduler!  */
    j       _tx_thread_schedule                         // Return to scheduler

/* }  */
