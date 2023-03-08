/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
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
#if defined(__riscv_float_abi_single)
    flw     f0, 31*REGBYTES(sp)                         // Recover ft0
    flw     f1, 32*REGBYTES(sp)                         // Recover ft1
    flw     f2, 33*REGBYTES(sp)                         // Recover ft2
    flw     f3, 34*REGBYTES(sp)                         // Recover ft3
    flw     f4, 35*REGBYTES(sp)                         // Recover ft4
    flw     f5, 36*REGBYTES(sp)                         // Recover ft5
    flw     f6, 37*REGBYTES(sp)                         // Recover ft6
    flw     f7, 38*REGBYTES(sp)                         // Recover ft7
    flw     f10,41*REGBYTES(sp)                         // Recover fa0
    flw     f11,42*REGBYTES(sp)                         // Recover fa1
    flw     f12,43*REGBYTES(sp)                         // Recover fa2
    flw     f13,44*REGBYTES(sp)                         // Recover fa3
    flw     f14,45*REGBYTES(sp)                         // Recover fa4
    flw     f15,46*REGBYTES(sp)                         // Recover fa5
    flw     f16,47*REGBYTES(sp)                         // Recover fa6
    flw     f17,48*REGBYTES(sp)                         // Recover fa7
    flw     f28,59*REGBYTES(sp)                         // Recover ft8
    flw     f29,60*REGBYTES(sp)                         // Recover ft9
    flw     f30,61*REGBYTES(sp)                         // Recover ft10
    flw     f31,62*REGBYTES(sp)                         // Recover ft11
    lw      t0, 63*REGBYTES(sp)                         // Recover fcsr
    csrw    fcsr, t0                                    //
#elif defined(__riscv_float_abi_double)
    fld     f0, 31*REGBYTES(sp)                         // Recover ft0
    fld     f1, 32*REGBYTES(sp)                         // Recover ft1
    fld     f2, 33*REGBYTES(sp)                         // Recover ft2
    fld     f3, 34*REGBYTES(sp)                         // Recover ft3
    fld     f4, 35*REGBYTES(sp)                         // Recover ft4
    fld     f5, 36*REGBYTES(sp)                         // Recover ft5
    fld     f6, 37*REGBYTES(sp)                         // Recover ft6
    fld     f7, 38*REGBYTES(sp)                         // Recover ft7
    fld     f10,41*REGBYTES(sp)                         // Recover fa0
    fld     f11,42*REGBYTES(sp)                         // Recover fa1
    fld     f12,43*REGBYTES(sp)                         // Recover fa2
    fld     f13,44*REGBYTES(sp)                         // Recover fa3
    fld     f14,45*REGBYTES(sp)                         // Recover fa4
    fld     f15,46*REGBYTES(sp)                         // Recover fa5
    fld     f16,47*REGBYTES(sp)                         // Recover fa6
    fld     f17,48*REGBYTES(sp)                         // Recover fa7
    fld     f28,59*REGBYTES(sp)                         // Recover ft8
    fld     f29,60*REGBYTES(sp)                         // Recover ft9
    fld     f30,61*REGBYTES(sp)                         // Recover ft10
    fld     f31,62*REGBYTES(sp)                         // Recover ft11
    LOAD    t0, 63*REGBYTES(sp)                         // Recover fcsr
    csrw    fcsr, t0                                    //
#endif

    /* Recover standard registers.  */

    /* Restore registers,
       Skip global pointer because that does not change.
       Also skip the saved registers since they have been restored by any function we called,
       except s0 since we use it ourselves. */

    LOAD    t0, 30*REGBYTES(sp)                         // Recover mepc
    csrw    mepc, t0                                    // Setup mepc
    li      t0, 0x1880                                  // Prepare MPIP
    csrw    mstatus, t0                                 // Enable MPIP

    LOAD    x1,  28*REGBYTES(sp)                        // Recover RA
    LOAD    x5,  19*REGBYTES(sp)                        // Recover t0
    LOAD    x6,  18*REGBYTES(sp)                        // Recover t1
    LOAD    x7,  17*REGBYTES(sp)                        // Recover t2
    LOAD    x8,  12*REGBYTES(sp)                        // Recover s0
    LOAD    x10, 27*REGBYTES(sp)                        // Recover a0
    LOAD    x11, 26*REGBYTES(sp)                        // Recover a1
    LOAD    x12, 25*REGBYTES(sp)                        // Recover a2
    LOAD    x13, 24*REGBYTES(sp)                        // Recover a3
    LOAD    x14, 23*REGBYTES(sp)                        // Recover a4
    LOAD    x15, 22*REGBYTES(sp)                        // Recover a5
    LOAD    x16, 21*REGBYTES(sp)                        // Recover a6
    LOAD    x17, 20*REGBYTES(sp)                        // Recover a7
    LOAD    x28, 16*REGBYTES(sp)                        // Recover t3
    LOAD    x29, 15*REGBYTES(sp)                        // Recover t4
    LOAD    x30, 14*REGBYTES(sp)                        // Recover t5
    LOAD    x31, 13*REGBYTES(sp)                        // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*REGBYTES                         // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*REGBYTES                         // Recover stack frame - without floating point enabled
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

    LOAD    sp, 2*REGBYTES(t1)                          // Switch back to thread's stack

    /* Recover floating point registers.  */
#if defined(__riscv_float_abi_single)
    flw     f0, 31*REGBYTES(sp)                         // Recover ft0
    flw     f1, 32*REGBYTES(sp)                         // Recover ft1
    flw     f2, 33*REGBYTES(sp)                         // Recover ft2
    flw     f3, 34*REGBYTES(sp)                         // Recover ft3
    flw     f4, 35*REGBYTES(sp)                         // Recover ft4
    flw     f5, 36*REGBYTES(sp)                         // Recover ft5
    flw     f6, 37*REGBYTES(sp)                         // Recover ft6
    flw     f7, 38*REGBYTES(sp)                         // Recover ft7
    flw     f10,41*REGBYTES(sp)                         // Recover fa0
    flw     f11,42*REGBYTES(sp)                         // Recover fa1
    flw     f12,43*REGBYTES(sp)                         // Recover fa2
    flw     f13,44*REGBYTES(sp)                         // Recover fa3
    flw     f14,45*REGBYTES(sp)                         // Recover fa4
    flw     f15,46*REGBYTES(sp)                         // Recover fa5
    flw     f16,47*REGBYTES(sp)                         // Recover fa6
    flw     f17,48*REGBYTES(sp)                         // Recover fa7
    flw     f28,59*REGBYTES(sp)                         // Recover ft8
    flw     f29,60*REGBYTES(sp)                         // Recover ft9
    flw     f30,61*REGBYTES(sp)                         // Recover ft10
    flw     f31,62*REGBYTES(sp)                         // Recover ft11
    lw      t0, 63*REGBYTES(sp)                         // Recover fcsr
    csrw    fcsr, t0                                    //
#elif defined(__riscv_float_abi_double)
    fld     f0, 31*REGBYTES(sp)                         // Recover ft0
    fld     f1, 32*REGBYTES(sp)                         // Recover ft1
    fld     f2, 33*REGBYTES(sp)                         // Recover ft2
    fld     f3, 34*REGBYTES(sp)                         // Recover ft3
    fld     f4, 35*REGBYTES(sp)                         // Recover ft4
    fld     f5, 36*REGBYTES(sp)                         // Recover ft5
    fld     f6, 37*REGBYTES(sp)                         // Recover ft6
    fld     f7, 38*REGBYTES(sp)                         // Recover ft7
    fld     f10,41*REGBYTES(sp)                         // Recover fa0
    fld     f11,42*REGBYTES(sp)                         // Recover fa1
    fld     f12,43*REGBYTES(sp)                         // Recover fa2
    fld     f13,44*REGBYTES(sp)                         // Recover fa3
    fld     f14,45*REGBYTES(sp)                         // Recover fa4
    fld     f15,46*REGBYTES(sp)                         // Recover fa5
    fld     f16,47*REGBYTES(sp)                         // Recover fa6
    fld     f17,48*REGBYTES(sp)                         // Recover fa7
    fld     f28,59*REGBYTES(sp)                         // Recover ft8
    fld     f29,60*REGBYTES(sp)                         // Recover ft9
    fld     f30,61*REGBYTES(sp)                         // Recover ft10
    fld     f31,62*REGBYTES(sp)                         // Recover ft11
    LOAD    t0, 63*REGBYTES(sp)                         // Recover fcsr
    csrw    fcsr, t0                                    //
#endif

    /* Recover the saved context and return to the point of interrupt.  */

    /* Recover standard registers.  */
    /* Restore registers,
       Skip global pointer because that does not change */

    LOAD    t0, 240(sp)                                 // Recover mepc
    csrw    mepc, t0                                    // Setup mepc
    li      t0, 0x1880                                  // Prepare MPIP
    csrw    mstatus, t0                                 // Enable MPIP

    LOAD    x1,  28*REGBYTES(sp)                        // Recover RA
    LOAD    x5,  19*REGBYTES(sp)                        // Recover t0
    LOAD    x6,  18*REGBYTES(sp)                        // Recover t1
    LOAD    x7,  17*REGBYTES(sp)                        // Recover t2
    LOAD    x8,  12*REGBYTES(sp)                        // Recover s0
    LOAD    x10, 27*REGBYTES(sp)                        // Recover a0
    LOAD    x11, 26*REGBYTES(sp)                        // Recover a1
    LOAD    x12, 25*REGBYTES(sp)                        // Recover a2
    LOAD    x13, 24*REGBYTES(sp)                        // Recover a3
    LOAD    x14, 23*REGBYTES(sp)                        // Recover a4
    LOAD    x15, 22*REGBYTES(sp)                        // Recover a5
    LOAD    x16, 21*REGBYTES(sp)                        // Recover a6
    LOAD    x17, 20*REGBYTES(sp)                        // Recover a7
    LOAD    x28, 16*REGBYTES(sp)                        // Recover t3
    LOAD    x29, 15*REGBYTES(sp)                        // Recover t4
    LOAD    x30, 14*REGBYTES(sp)                        // Recover t5
    LOAD    x31, 13*REGBYTES(sp)                        // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*REGBYTES                         // Recover stack frame - with floating point enabled
#else
    addi    sp, sp, 32*REGBYTES                         // Recover stack frame - without floating point enabled
#endif
    mret                                                // Return to point of interrupt

    /* }
    else
    {  */
_tx_thread_preempt_restore:
    /* Instead of directly activating the thread again, ensure we save the
       entire stack frame by saving the remaining registers. */

    LOAD    t0, 2*REGBYTES(t1)                          // Pickup thread's stack pointer
    ori     t3, x0, 1                                   // Build interrupt stack type
    STORE   t3, 0(t0)                                   // Store stack type

    /* Store floating point preserved registers.  */
#ifdef __riscv_float_abi_single
    fsw     f8,  39*REGBYTES(t0)                                // Store fs0
    fsw     f9,  40*REGBYTES(t0)                                // Store fs1
    fsw     f18, 49*REGBYTES(t0)                                // Store fs2
    fsw     f19, 50*REGBYTES(t0)                                // Store fs3
    fsw     f20, 51*REGBYTES(t0)                                // Store fs4
    fsw     f21, 52*REGBYTES(t0)                                // Store fs5
    fsw     f22, 53*REGBYTES(t0)                                // Store fs6
    fsw     f23, 54*REGBYTES(t0)                                // Store fs7
    fsw     f24, 55*REGBYTES(t0)                                // Store fs8
    fsw     f25, 56*REGBYTES(t0)                                // Store fs9
    fsw     f26, 57*REGBYTES(t0)                                // Store fs10
    fsw     f27, 58*REGBYTES(t0)                                // Store fs11
#elif defined(__riscv_float_abi_double)
    fsd     f8,  39*REGBYTES(t0)                                // Store fs0
    fsd     f9,  40*REGBYTES(t0)                                // Store fs1
    fsd     f18, 49*REGBYTES(t0)                                // Store fs2
    fsd     f19, 50*REGBYTES(t0)                                // Store fs3
    fsd     f20, 51*REGBYTES(t0)                                // Store fs4
    fsd     f21, 52*REGBYTES(t0)                                // Store fs5
    fsd     f22, 53*REGBYTES(t0)                                // Store fs6
    fsd     f23, 54*REGBYTES(t0)                                // Store fs7
    fsd     f24, 55*REGBYTES(t0)                                // Store fs8
    fsd     f25, 56*REGBYTES(t0)                                // Store fs9
    fsd     f26, 57*REGBYTES(t0)                                // Store fs10
    fsd     f27, 58*REGBYTES(t0)                                // Store fs11
#endif

    /* Store standard preserved registers.  */

    STORE   x9,  11*REGBYTES(t0)                        // Store s1
    STORE   x18, 10*REGBYTES(t0)                        // Store s2
    STORE   x19,  9*REGBYTES(t0)                        // Store s3
    STORE   x20,  8*REGBYTES(t0)                        // Store s4
    STORE   x21,  7*REGBYTES(t0)                        // Store s5
    STORE   x22,  6*REGBYTES(t0)                        // Store s6
    STORE   x23,  5*REGBYTES(t0)                        // Store s7
    STORE   x24,  4*REGBYTES(t0)                        // Store s8
    STORE   x25,  3*REGBYTES(t0)                        // Store s9
    STORE   x26,  2*REGBYTES(t0)                        // Store s10
    STORE   x27,  1*REGBYTES(t0)                        // Store s11
                                                        // Note: s0 is already stored!

    /* Save the remaining time-slice and disable it.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t0, _tx_timer_time_slice                    // Pickup time slice variable address
    LOAD    t2, 0(t0)                                   // Pickup time slice
    beqz    t2, _tx_thread_dont_save_ts                 // If 0, skip time slice processing

        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice
        _tx_timer_time_slice =  0;  */

    STORE   t2, 6*REGBYTES(t1)                          // Save current time slice
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
