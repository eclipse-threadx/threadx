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
/*    _tx_thread_schedule                                RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
/*    Wei-Chen Lai, National Cheng Kung University                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function waits for a thread control block pointer to appear in */
/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */
/*    in the variable, the corresponding thread is resumed.               */
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
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*    _tx_thread_system_return             Return to system from thread   */
/*    _tx_thread_context_restore           Restore thread's context       */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_schedule(VOID)
{  */
    .global  _tx_thread_schedule
_tx_thread_schedule:

    /* Enable interrupts.  */
    csrsi   mstatus, 0x08                               // Enable interrupts

    /* Wait for a thread to execute.  */
    /* do
    {  */

    la      t0, _tx_thread_execute_ptr                  // Pickup address of execute ptr
_tx_thread_schedule_loop:
    LOAD    t1, 0(t0)                                   // Pickup next thread to execute
    bnez    t1, _tx_thread_ready                        // If not NULL, break loop
    wfi                                                 // Wait for interrupt
    j       _tx_thread_schedule_loop                    // Wait for interrupt

_tx_thread_ready:

    /* }
    while(_tx_thread_execute_ptr == TX_NULL);  */

    /* Yes! We have a thread to execute.  Lockout interrupts and
       transfer control to it.  */
    csrci   mstatus, 0x08                               // Lockout interrupts

    /* Setup the current thread pointer.  */
    /* _tx_thread_current_ptr =  _tx_thread_execute_ptr;  */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    STORE   t1, 0(t0)                                   // Set current thread pointer

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr -> tx_thread_run_count++;  */

    LOAD    t2, TX_THREAD_RUN_COUNT(t1)                 // Pickup run count
    LOAD    t3, TX_THREAD_TIME_SLICE(t1)                // Pickup time slice value
    addi    t2, t2, 1                                   // Increment run count
    STORE   t2, TX_THREAD_RUN_COUNT(t1)                 // Store new run count

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;  */

    la      t2, _tx_timer_time_slice                    // Pickup time-slice variable address

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;  */

    LOAD    sp, TX_THREAD_STACK_PTR(t1)                 // Switch to thread's stack
    STORE   t3, 0(t2)                                   // Store new time-slice*/

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_enter                  // Call the thread execution enter function
#endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    LOAD    t2, 0(sp)                                   // Pickup stack type
    beqz    t2, _tx_thread_synch_return                 // If 0, solicited thread return

    /* Determine if floating point registers need to be recovered.  */

#if defined(__riscv_flen)
    LOAD    t1, TX_STACK_OFFSET_MSTATUS(t0)             // Pickup thread's floating point state */
    /* Check if floating point is enabled */
    srli    t1, t1, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_schedule_skip_fp_restore   // Skip floating point restore FS is Off
#if __riscv_flen == 32
    flw     f0, TX_STACK_OFFSET_F0(sp)                  // Recover ft0
    flw     f1, TX_STACK_OFFSET_F1(sp)                  // Recover ft1
    flw     f2, TX_STACK_OFFSET_F2(sp)                  // Recover ft2
    flw     f3, TX_STACK_OFFSET_F3(sp)                  // Recover ft3
    flw     f4, TX_STACK_OFFSET_F4(sp)                  // Recover ft4
    flw     f5, TX_STACK_OFFSET_F5(sp)                  // Recover ft5
    flw     f6, TX_STACK_OFFSET_F6(sp)                  // Recover ft6
    flw     f7, TX_STACK_OFFSET_F7(sp)                  // Recover ft7
    flw     f8, TX_STACK_OFFSET_F8(sp)                  // Recover fs0
    flw     f9, TX_STACK_OFFSET_F9(sp)                  // Recover fs1
    flw     f10,TX_STACK_OFFSET_F10(sp)                 // Recover fa0
    flw     f11,TX_STACK_OFFSET_F11(sp)                 // Recover fa1
    flw     f12,TX_STACK_OFFSET_F12(sp)                 // Recover fa2
    flw     f13,TX_STACK_OFFSET_F13(sp)                 // Recover fa3
    flw     f14,TX_STACK_OFFSET_F14(sp)                 // Recover fa4
    flw     f15,TX_STACK_OFFSET_F15(sp)                 // Recover fa5
    flw     f16,TX_STACK_OFFSET_F16(sp)                 // Recover fa6
    flw     f17,TX_STACK_OFFSET_F17(sp)                 // Recover fa7
    flw     f18,TX_STACK_OFFSET_F18(sp)                 // Recover fs2
    flw     f19,TX_STACK_OFFSET_F19(sp)                 // Recover fs3
    flw     f20,TX_STACK_OFFSET_F20(sp)                 // Recover fs4
    flw     f21,TX_STACK_OFFSET_F21(sp)                 // Recover fs5
    flw     f22,TX_STACK_OFFSET_F22(sp)                 // Recover fs6
    flw     f23,TX_STACK_OFFSET_F23(sp)                 // Recover fs7
    flw     f24,TX_STACK_OFFSET_F24(sp)                 // Recover fs8
    flw     f25,TX_STACK_OFFSET_F25(sp)                 // Recover fs9
    flw     f26,TX_STACK_OFFSET_F26(sp)                 // Recover fs10
    flw     f27,TX_STACK_OFFSET_F27(sp)                 // Recover fs11
    flw     f28,TX_STACK_OFFSET_F28(sp)                 // Recover ft8
    flw     f29,TX_STACK_OFFSET_F29(sp)                 // Recover ft9
    flw     f30,TX_STACK_OFFSET_F30(sp)                 // Recover ft10
    flw     f31,TX_STACK_OFFSET_F31(sp)                 // Recover ft11
    LOAD    t0, TX_STACK_OFFSET_FCSR(sp)                // Recover fcsr
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
    fld     f8, TX_STACK_OFFSET_F8(sp)                  // Recover fs0
    fld     f9, TX_STACK_OFFSET_F9(sp)                  // Recover fs1
    fld     f10,TX_STACK_OFFSET_F10(sp)                 // Recover fa0
    fld     f11,TX_STACK_OFFSET_F11(sp)                 // Recover fa1
    fld     f12,TX_STACK_OFFSET_F12(sp)                 // Recover fa2
    fld     f13,TX_STACK_OFFSET_F13(sp)                 // Recover fa3
    fld     f14,TX_STACK_OFFSET_F14(sp)                 // Recover fa4
    fld     f15,TX_STACK_OFFSET_F15(sp)                 // Recover fa5
    fld     f16,TX_STACK_OFFSET_F16(sp)                 // Recover fa6
    fld     f17,TX_STACK_OFFSET_F17(sp)                 // Recover fa7
    fld     f18,TX_STACK_OFFSET_F18(sp)                 // Recover fs2
    fld     f19,TX_STACK_OFFSET_F19(sp)                 // Recover fs3
    fld     f20,TX_STACK_OFFSET_F20(sp)                 // Recover fs4
    fld     f21,TX_STACK_OFFSET_F21(sp)                 // Recover fs5
    fld     f22,TX_STACK_OFFSET_F22(sp)                 // Recover fs6
    fld     f23,TX_STACK_OFFSET_F23(sp)                 // Recover fs7
    fld     f24,TX_STACK_OFFSET_F24(sp)                 // Recover fs8
    fld     f25,TX_STACK_OFFSET_F25(sp)                 // Recover fs9
    fld     f26,TX_STACK_OFFSET_F26(sp)                 // Recover fs10
    fld     f27,TX_STACK_OFFSET_F27(sp)                 // Recover fs11
    fld     f28,TX_STACK_OFFSET_F28(sp)                 // Recover ft8
    fld     f29,TX_STACK_OFFSET_F29(sp)                 // Recover ft9
    fld     f30,TX_STACK_OFFSET_F30(sp)                 // Recover ft10
    fld     f31,TX_STACK_OFFSET_F31(sp)                 // Recover ft11
    LOAD    t0, TX_STACK_OFFSET_FCSR(sp)                // Recover fcsr
    csrw    fcsr, t0                                    //
#endif
_tx_thread_schedule_skip_fp_restore:
#endif

    /* Recover standard registers.  */

    LOAD    t0, TX_STACK_OFFSET_MEPC(sp)                // Recover mepc
    csrw    mepc, t0                                    // Store mepc
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
    LOAD    x9,  TX_STACK_OFFSET_X9(sp)                 // Recover s1
    LOAD    x10, TX_STACK_OFFSET_X10(sp)                // Recover a0
    LOAD    x11, TX_STACK_OFFSET_X11(sp)                // Recover a1
    LOAD    x12, TX_STACK_OFFSET_X12(sp)                // Recover a2
    LOAD    x13, TX_STACK_OFFSET_X13(sp)                // Recover a3
    LOAD    x14, TX_STACK_OFFSET_X14(sp)                // Recover a4
    LOAD    x15, TX_STACK_OFFSET_X15(sp)                // Recover a5
    LOAD    x16, TX_STACK_OFFSET_X16(sp)                // Recover a6
    LOAD    x17, TX_STACK_OFFSET_X17(sp)                // Recover a7
    LOAD    x18, TX_STACK_OFFSET_X18(sp)                // Recover s2
    LOAD    x19, TX_STACK_OFFSET_X19(sp)                // Recover s3
    LOAD    x20, TX_STACK_OFFSET_X20(sp)                // Recover s4
    LOAD    x21, TX_STACK_OFFSET_X21(sp)                // Recover s5
    LOAD    x22, TX_STACK_OFFSET_X22(sp)                // Recover s6
    LOAD    x23, TX_STACK_OFFSET_X23(sp)                // Recover s7
    LOAD    x24, TX_STACK_OFFSET_X24(sp)                // Recover s8
    LOAD    x25, TX_STACK_OFFSET_X25(sp)                // Recover s9
    LOAD    x26, TX_STACK_OFFSET_X26(sp)                // Recover s10
    LOAD    x27, TX_STACK_OFFSET_X27(sp)                // Recover s11
    LOAD    x28, TX_STACK_OFFSET_X28(sp)                // Recover t3
    LOAD    x29, TX_STACK_OFFSET_X29(sp)                // Recover t4
    LOAD    x30, TX_STACK_OFFSET_X30(sp)                // Recover t5
    LOAD    x31, TX_STACK_OFFSET_X31(sp)                // Recover t6

#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, TX_THREAD_FRAME_SIZE_FPU            // Recover stack frame - with floating point registers
#else
    addi    sp, sp, TX_THREAD_FRAME_SIZE_INT            // Recover stack frame - without floating point registers
#endif
    mret                                                // Return to point of interrupt

_tx_thread_synch_return:

#if defined(__riscv_flen)
    LOAD    t1, TX_STACK_OFFSET_MSTATUS(t0)             // Pickup thread's floating point state */
    /* Check if floating point is enabled.
       Invariant: If FS is Off (0), skip restore. */
    srli    t1, t1, 13
    andi    t1, t1, 0x3
    beqz    t1, _tx_thread_synch_return_skip_fp_restore   // Skip floating point restore FS is Off
#if __riscv_flen == 32
    flw     f8,  TX_STACK_OFFSET_F8(sp)                 // Recover fs0
    flw     f9,  TX_STACK_OFFSET_F9(sp)                 // Recover fs1
    flw     f18, TX_STACK_OFFSET_F18(sp)                // Recover fs2
    flw     f19, TX_STACK_OFFSET_F19(sp)                // Recover fs3
    flw     f20, TX_STACK_OFFSET_F20(sp)                // Recover fs4
    flw     f21, TX_STACK_OFFSET_F21(sp)                // Recover fs5
    flw     f22, TX_STACK_OFFSET_F22(sp)                // Recover fs6
    flw     f23, TX_STACK_OFFSET_F23(sp)                // Recover fs7
    flw     f24, TX_STACK_OFFSET_F24(sp)                // Recover fs8
    flw     f25, TX_STACK_OFFSET_F25(sp)                // Recover fs9
    flw     f26, TX_STACK_OFFSET_F26(sp)                // Recover fs10
    flw     f27, TX_STACK_OFFSET_F27(sp)                // Recover fs11
    LOAD    t0,  TX_STACK_OFFSET_FCSR(sp)               // Recover fcsr
    csrw    fcsr, t0                                    //
#elif __riscv_flen == 64
    fld     f8,  TX_STACK_OFFSET_F8(sp)                 // Recover fs0
    fld     f9,  TX_STACK_OFFSET_F9(sp)                 // Recover fs1
    fld     f18, TX_STACK_OFFSET_F18(sp)                // Recover fs2
    fld     f19, TX_STACK_OFFSET_F19(sp)                // Recover fs3
    fld     f20, TX_STACK_OFFSET_F20(sp)                // Recover fs4
    fld     f21, TX_STACK_OFFSET_F21(sp)                // Recover fs5
    fld     f22, TX_STACK_OFFSET_F22(sp)                // Recover fs6
    fld     f23, TX_STACK_OFFSET_F23(sp)                // Recover fs7
    fld     f24, TX_STACK_OFFSET_F24(sp)                // Recover fs8
    fld     f25, TX_STACK_OFFSET_F25(sp)                // Recover fs9
    fld     f26, TX_STACK_OFFSET_F26(sp)                // Recover fs10
    fld     f27, TX_STACK_OFFSET_F27(sp)                // Recover fs11
    LOAD    t0,  TX_STACK_OFFSET_FCSR(sp)               // Recover fcsr
    csrw    fcsr, t0                                    //
#endif
_tx_thread_synch_return_skip_fp_restore:
#endif

    /* Recover standard preserved registers.  */
    /* Recover standard registers.  */

    LOAD    x1,  TX_STACK_OFFSET_X1(sp)                 // Recover RA
    LOAD    x8,  TX_STACK_OFFSET_X8(sp)                 // Recover s0
    LOAD    x9,  TX_STACK_OFFSET_X9(sp)                 // Recover s1
    LOAD    x18, TX_STACK_OFFSET_X18(sp)                // Recover s2
    LOAD    x19, TX_STACK_OFFSET_X19(sp)                // Recover s3
    LOAD    x20, TX_STACK_OFFSET_X20(sp)                // Recover s4
    LOAD    x21, TX_STACK_OFFSET_X21(sp)                // Recover s5
    LOAD    x22, TX_STACK_OFFSET_X22(sp)                // Recover s6
    LOAD    x23, TX_STACK_OFFSET_X23(sp)                // Recover s7
    LOAD    x24, TX_STACK_OFFSET_X24(sp)                // Recover s8
    LOAD    x25, TX_STACK_OFFSET_X25(sp)                // Recover s9
    LOAD    x26, TX_STACK_OFFSET_X26(sp)                // Recover s10
    LOAD    x27, TX_STACK_OFFSET_X27(sp)                // Recover s11
    LOAD    t0,  TX_STACK_OFFSET_MSTATUS(sp)            // Recover mstatus
    csrw    mstatus, t0                                 // Store mstatus, enables interrupt
#if defined(__riscv_flen) && ((__riscv_flen == 32) || (__riscv_flen == 64))
    addi    sp, sp, TX_THREAD_FRAME_SIZE_FPU            // Recover stack frame
#else
    addi    sp, sp, TX_THREAD_FRAME_SIZE_INT            // Recover stack frame
#endif
    ret                                                 // Return to thread

/* }  */
