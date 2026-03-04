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
/*    _tx_thread_schedule                                RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
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
    csrsi   mstatus, 0x08                               // Enable interrupts (MIE bit 3)

    /* Wait for a thread to execute.  */
    /* do
    {  */

    la      t0, _tx_thread_execute_ptr                  // Pickup address of execute ptr
_tx_thread_schedule_loop:
    ld      t1, 0(t0)                                   // Pickup next thread to execute

/* TX_USE_WFI_IDLE Configuration:
       When defined, the scheduler enters WFI (Wait-For-Interrupt) mode when
       no threads are ready, reducing power consumption. The core will wake
       on any enabled interrupt. This is recommended for battery-powered or
       low-power applications. Define TX_USE_WFI_IDLE in tx_user.h or via
       compiler flags to enable this feature.  */
#ifdef TX_USE_WFI_IDLE
    beqz    t1, 1f
    j 2f
1:  wfi
    j _tx_thread_schedule_loop
2:
    beqz    t1, _tx_thread_schedule_loop                // Fallback: If still NULL, loop
#else
    beqz    t1, _tx_thread_schedule_loop                // If NULL, wait for thread to execute
#endif

    /* }
    while(_tx_thread_execute_ptr == NULL);  */

    /* Yes! We have a thread to execute.  Lockout interrupts and
       transfer control to it.  */
    csrci   mstatus, 0x08                               // Lockout interrupts

    /* Setup the current thread pointer.  */
    /* _tx_thread_current_ptr =  _tx_thread_execute_ptr;  */

    la      t0, _tx_thread_current_ptr                  // Pickup current thread pointer address
    sd      t1, 0(t0)                                   // Set current thread pointer

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr -> tx_thread_run_count++;  */

    ld      t2, 8(t1)                                   // Pickup run count
    ld      t3, 48(t1)                                  // Pickup time slice value
    addi    t2, t2, 1                                   // Increment run count
    sd      t2, 8(t1)                                   // Store new run count

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;  */

    la      t2, _tx_timer_time_slice                    // Pickup time-slice variable address

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;  */

    ld      sp, 16(t1)                                  // Switch to thread's stack
    sd      t3, 0(t2)                                   // Store new time-slice

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_enter                  // Call the thread execution enter function
#endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    ld      t2, 0(sp)                                   // Pickup stack type
    beqz    t2, _tx_thread_synch_return                 // If 0, solicited thread return

    /* Determine if floating point registers need to be recovered.  */

#if defined(__riscv_float_abi_single)
    flw     f0, 31*8(sp)                                // Recover ft0
    flw     f1, 32*8(sp)                                // Recover ft1
    flw     f2, 33*8(sp)                                // Recover ft2
    flw     f3, 34*8(sp)                                // Recover ft3
    flw     f4, 35*8(sp)                                // Recover ft4
    flw     f5, 36*8(sp)                                // Recover ft5
    flw     f6, 37*8(sp)                                // Recover ft6
    flw     f7, 38*8(sp)                                // Recover ft7
    flw     f8, 39*8(sp)                                // Recover fs0
    flw     f9, 40*8(sp)                                // Recover fs1
    flw     f10,41*8(sp)                                // Recover fa0
    flw     f11,42*8(sp)                                // Recover fa1
    flw     f12,43*8(sp)                                // Recover fa2
    flw     f13,44*8(sp)                                // Recover fa3
    flw     f14,45*8(sp)                                // Recover fa4
    flw     f15,46*8(sp)                                // Recover fa5
    flw     f16,47*8(sp)                                // Recover fa6
    flw     f17,48*8(sp)                                // Recover fa7
    flw     f18,49*8(sp)                                // Recover fs2
    flw     f19,50*8(sp)                                // Recover fs3
    flw     f20,51*8(sp)                                // Recover fs4
    flw     f21,52*8(sp)                                // Recover fs5
    flw     f22,53*8(sp)                                // Recover fs6
    flw     f23,54*8(sp)                                // Recover fs7
    flw     f24,55*8(sp)                                // Recover fs8
    flw     f25,56*8(sp)                                // Recover fs9
    flw     f26,57*8(sp)                                // Recover fs10
    flw     f27,58*8(sp)                                // Recover fs11
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
    fld     f8, 39*8(sp)                                // Recover fs0
    fld     f9, 40*8(sp)                                // Recover fs1
    fld     f10,41*8(sp)                                // Recover fa0
    fld     f11,42*8(sp)                                // Recover fa1
    fld     f12,43*8(sp)                                // Recover fa2
    fld     f13,44*8(sp)                                // Recover fa3
    fld     f14,45*8(sp)                                // Recover fa4
    fld     f15,46*8(sp)                                // Recover fa5
    fld     f16,47*8(sp)                                // Recover fa6
    fld     f17,48*8(sp)                                // Recover fa7
    fld     f18,49*8(sp)                                // Recover fs2
    fld     f19,50*8(sp)                                // Recover fs3
    fld     f20,51*8(sp)                                // Recover fs4
    fld     f21,52*8(sp)                                // Recover fs5
    fld     f22,53*8(sp)                                // Recover fs6
    fld     f23,54*8(sp)                                // Recover fs7
    fld     f24,55*8(sp)                                // Recover fs8
    fld     f25,56*8(sp)                                // Recover fs9
    fld     f26,57*8(sp)                                // Recover fs10
    fld     f27,58*8(sp)                                // Recover fs11
    fld     f28,59*8(sp)                                // Recover ft8
    fld     f29,60*8(sp)                                // Recover ft9
    fld     f30,61*8(sp)                                // Recover ft10
    fld     f31,62*8(sp)                                // Recover ft11
    ld      t0, 63*8(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    // Restore fcsr
#endif

    /* Recover standard registers.  */

    ld      t0, 30*8(sp)                                // Recover mepc
    csrw    mepc, t0                                    // Store mepc
    li      t0, 0x1880                                  // Prepare mstatus: MPP=Machine(0x1800) | MPIE(0x80)
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    li      t1, 0x2000                                  // Set FS bits for FP state
    or      t0, t0, t1
#endif
    csrw    mstatus, t0                                 // Set mstatus

    ld      ra,  28*8(sp)                               // Recover return address
    ld      t0,  19*8(sp)                               // Recover t0
    ld      t1,  18*8(sp)                               // Recover t1
    ld      t2,  17*8(sp)                               // Recover t2
    ld      s0,  12*8(sp)                               // Recover s0
    ld      s1,  11*8(sp)                               // Recover s1
    ld      a0, 27*8(sp)                                // Recover a0
    ld      a1, 26*8(sp)                                // Recover a1
    ld      a2, 25*8(sp)                                // Recover a2
    ld      a3, 24*8(sp)                                // Recover a3
    ld      a4, 23*8(sp)                                // Recover a4
    ld      a5, 22*8(sp)                                // Recover a5
    ld      a6, 21*8(sp)                                // Recover a6
    ld      a7, 20*8(sp)                                // Recover a7
    ld      s2, 10*8(sp)                                // Recover s2
    ld      s3,  9*8(sp)                                // Recover s3
    ld      s4,  8*8(sp)                                // Recover s4
    ld      s5,  7*8(sp)                                // Recover s5
    ld      s6,  6*8(sp)                                // Recover s6
    ld      s7,  5*8(sp)                                // Recover s7
    ld      s8,  4*8(sp)                                // Recover s8
    ld      s9,  3*8(sp)                                // Recover s9
    ld      s10,  2*8(sp)                               // Recover s10
    ld      s11,  1*8(sp)                               // Recover s11
    ld      t3, 16*8(sp)                                // Recover t3
    ld      t4, 15*8(sp)                                // Recover t4
    ld      t5, 14*8(sp)                                // Recover t5
    ld      t6, 13*8(sp)                                // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*8                                // Recover stack frame - with floating point registers
#else
    addi    sp, sp, 32*8                                // Recover stack frame - without floating point registers
#endif
    mret                                                // Return to point of interrupt

_tx_thread_synch_return:

#if defined(__riscv_float_abi_single)
    flw     f8, 15*8(sp)                                // Recover fs0
    flw     f9, 16*8(sp)                                // Recover fs1
    flw     f18,17*8(sp)                                // Recover fs2
    flw     f19,18*8(sp)                                // Recover fs3
    flw     f20,19*8(sp)                                // Recover fs4
    flw     f21,20*8(sp)                                // Recover fs5
    flw     f22,21*8(sp)                                // Recover fs6
    flw     f23,22*8(sp)                                // Recover fs7
    flw     f24,23*8(sp)                                // Recover fs8
    flw     f25,24*8(sp)                                // Recover fs9
    flw     f26,25*8(sp)                                // Recover fs10
    flw     f27,26*8(sp)                                // Recover fs11
    ld      t0, 27*8(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    //
#elif defined(__riscv_float_abi_double)
    fld     f8, 15*8(sp)                                // Recover fs0
    fld     f9, 16*8(sp)                                // Recover fs1
    fld     f18,17*8(sp)                                // Recover fs2
    fld     f19,18*8(sp)                                // Recover fs3
    fld     f20,19*8(sp)                                // Recover fs4
    fld     f21,20*8(sp)                                // Recover fs5
    fld     f22,21*8(sp)                                // Recover fs6
    fld     f23,22*8(sp)                                // Recover fs7
    fld     f24,23*8(sp)                                // Recover fs8
    fld     f25,24*8(sp)                                // Recover fs9
    fld     f26,25*8(sp)                                // Recover fs10
    fld     f27,26*8(sp)                                // Recover fs11
    ld      t0, 27*8(sp)                                // Recover fcsr
    csrw    fcsr, t0                                    //
#endif

    /* Recover standard preserved registers.  */
    /* Recover standard registers.  */

    ld      ra,  13*8(sp)                               // Recover RA
    ld      s0,  12*8(sp)                               // Recover s0
    ld      s1,  11*8(sp)                               // Recover s1
    ld      s2, 10*8(sp)                                // Recover s2
    ld      s3,  9*8(sp)                                // Recover s3
    ld      s4,  8*8(sp)                                // Recover s4
    ld      s5,  7*8(sp)                                // Recover s5
    ld      s6,  6*8(sp)                                // Recover s6
    ld      s7,  5*8(sp)                                // Recover s7
    ld      s8,  4*8(sp)                                // Recover s8
    ld      s9,  3*8(sp)                                // Recover s9
    ld      s10,  2*8(sp)                               // Recover s10
    ld      s11,  1*8(sp)                               // Recover s11
    ld      t0,  14*8(sp)                               // Recover mstatus
    csrw    mstatus, t0                                 // Store mstatus, enables interrupt
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 29*8                                // Recover stack frame
#else
    addi    sp, sp, 16*8                                // Recover stack frame
#endif
    ret                                                 // Return to thread

/* }  */
