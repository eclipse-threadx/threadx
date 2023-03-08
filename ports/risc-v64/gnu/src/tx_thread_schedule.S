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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
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
    beqz    t1, _tx_thread_schedule_loop                // If NULL, wait for thread to execute

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

    LOAD    t2, 1*REGBYTES(t1)                          // Pickup run count
    LOAD    t3, 6*REGBYTES(t1)                          // Pickup time slice value
    addi    t2, t2, 1                                   // Increment run count
    STORE   t2, 1*REGBYTES(t1)                          // Store new run count

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;  */

    la      t2, _tx_timer_time_slice                    // Pickup time-slice variable address

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;  */

    LOAD    sp, 2*REGBYTES(t1)                          // Switch to thread's stack
    STORE   t3, 0(t2)                                   // Store new time-slice*/

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_enter                  // Call the thread execution enter function
#endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    LOAD    t2, 0(sp)                                   // Pickup stack type
    beqz    t2, _tx_thread_synch_return                 // If 0, solicited thread return

    /* Determine if floating point registers need to be recovered.  */

#if defined(__riscv_float_abi_single)
    flw     f0, 31*REGBYTES(sp)                         // Recover ft0
    flw     f1, 32*REGBYTES(sp)                         // Recover ft1
    flw     f2, 33*REGBYTES(sp)                         // Recover ft2
    flw     f3, 34*REGBYTES(sp)                         // Recover ft3
    flw     f4, 35*REGBYTES(sp)                         // Recover ft4
    flw     f5, 36*REGBYTES(sp)                         // Recover ft5
    flw     f6, 37*REGBYTES(sp)                         // Recover ft6
    flw     f7, 38*REGBYTES(sp)                         // Recover ft7
    flw     f8, 39*REGBYTES(sp)                         // Recover fs0
    flw     f9, 40*REGBYTES(sp)                         // Recover fs1
    flw     f10,41*REGBYTES(sp)                         // Recover fa0
    flw     f11,42*REGBYTES(sp)                         // Recover fa1
    flw     f12,43*REGBYTES(sp)                         // Recover fa2
    flw     f13,44*REGBYTES(sp)                         // Recover fa3
    flw     f14,45*REGBYTES(sp)                         // Recover fa4
    flw     f15,46*REGBYTES(sp)                         // Recover fa5
    flw     f16,47*REGBYTES(sp)                         // Recover fa6
    flw     f17,48*REGBYTES(sp)                         // Recover fa7
    flw     f18,49*REGBYTES(sp)                         // Recover fs2
    flw     f19,50*REGBYTES(sp)                         // Recover fs3
    flw     f20,51*REGBYTES(sp)                         // Recover fs4
    flw     f21,52*REGBYTES(sp)                         // Recover fs5
    flw     f22,53*REGBYTES(sp)                         // Recover fs6
    flw     f23,54*REGBYTES(sp)                         // Recover fs7
    flw     f24,55*REGBYTES(sp)                         // Recover fs8
    flw     f25,56*REGBYTES(sp)                         // Recover fs9
    flw     f26,57*REGBYTES(sp)                         // Recover fs10
    flw     f27,58*REGBYTES(sp)                         // Recover fs11
    flw     f28,59*REGBYTES(sp)                         // Recover ft8
    flw     f29,60*REGBYTES(sp)                         // Recover ft9
    flw     f30,61*REGBYTES(sp)                         // Recover ft10
    flw     f31,62*REGBYTES(sp)                         // Recover ft11
    LOAD    t0, 63*REGBYTES(sp)                         // Recover fcsr
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
    fld     f8, 39*REGBYTES(sp)                         // Recover fs0
    fld     f9, 40*REGBYTES(sp)                         // Recover fs1
    fld     f10,41*REGBYTES(sp)                         // Recover fa0
    fld     f11,42*REGBYTES(sp)                         // Recover fa1
    fld     f12,43*REGBYTES(sp)                         // Recover fa2
    fld     f13,44*REGBYTES(sp)                         // Recover fa3
    fld     f14,45*REGBYTES(sp)                         // Recover fa4
    fld     f15,46*REGBYTES(sp)                         // Recover fa5
    fld     f16,47*REGBYTES(sp)                         // Recover fa6
    fld     f17,48*REGBYTES(sp)                         // Recover fa7
    fld     f18,49*REGBYTES(sp)                         // Recover fs2
    fld     f19,50*REGBYTES(sp)                         // Recover fs3
    fld     f20,51*REGBYTES(sp)                         // Recover fs4
    fld     f21,52*REGBYTES(sp)                         // Recover fs5
    fld     f22,53*REGBYTES(sp)                         // Recover fs6
    fld     f23,54*REGBYTES(sp)                         // Recover fs7
    fld     f24,55*REGBYTES(sp)                         // Recover fs8
    fld     f25,56*REGBYTES(sp)                         // Recover fs9
    fld     f26,57*REGBYTES(sp)                         // Recover fs10
    fld     f27,58*REGBYTES(sp)                         // Recover fs11
    fld     f28,59*REGBYTES(sp)                         // Recover ft8
    fld     f29,60*REGBYTES(sp)                         // Recover ft9
    fld     f30,61*REGBYTES(sp)                         // Recover ft10
    fld     f31,62*REGBYTES(sp)                         // Recover ft11
    LOAD    t0, 63*REGBYTES(sp)                         // Recover fcsr
#endif

    /* Recover standard registers.  */

    LOAD    t0, 30*REGBYTES(sp)                         // Recover mepc
    csrw    mepc, t0                                    // Store mepc
    li      t0, 0x1880                                  // Prepare MPIP
    csrw    mstatus, t0                                 // Enable MPIP

    LOAD    x1,  28*REGBYTES(sp)                        // Recover RA
    LOAD    x5,  19*REGBYTES(sp)                        // Recover t0
    LOAD    x6,  18*REGBYTES(sp)                        // Recover t1
    LOAD    x7,  17*REGBYTES(sp)                        // Recover t2
    LOAD    x8,  12*REGBYTES(sp)                        // Recover s0
    LOAD    x9,  11*REGBYTES(sp)                        // Recover s1
    LOAD    x10, 27*REGBYTES(sp)                        // Recover a0
    LOAD    x11, 26*REGBYTES(sp)                        // Recover a1
    LOAD    x12, 25*REGBYTES(sp)                        // Recover a2
    LOAD    x13, 24*REGBYTES(sp)                        // Recover a3
    LOAD    x14, 23*REGBYTES(sp)                        // Recover a4
    LOAD    x15, 22*REGBYTES(sp)                        // Recover a5
    LOAD    x16, 21*REGBYTES(sp)                        // Recover a6
    LOAD    x17, 20*REGBYTES(sp)                        // Recover a7
    LOAD    x18, 10*REGBYTES(sp)                        // Recover s2
    LOAD    x19,  9*REGBYTES(sp)                        // Recover s3
    LOAD    x20,  8*REGBYTES(sp)                        // Recover s4
    LOAD    x21,  7*REGBYTES(sp)                        // Recover s5
    LOAD    x22,  6*REGBYTES(sp)                        // Recover s6
    LOAD    x23,  5*REGBYTES(sp)                        // Recover s7
    LOAD    x24,  4*REGBYTES(sp)                        // Recover s8
    LOAD    x25,  3*REGBYTES(sp)                        // Recover s9
    LOAD    x26,  2*REGBYTES(sp)                        // Recover s10
    LOAD    x27,  1*REGBYTES(sp)                        // Recover s11
    LOAD    x28, 16*REGBYTES(sp)                        // Recover t3
    LOAD    x29, 15*REGBYTES(sp)                        // Recover t4
    LOAD    x30, 14*REGBYTES(sp)                        // Recover t5
    LOAD    x31, 13*REGBYTES(sp)                        // Recover t6

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 65*REGBYTES                         // Recover stack frame - with floating point registers
#else
    addi    sp, sp, 32*REGBYTES                         // Recover stack frame - without floating point registers
#endif
    mret                                                // Return to point of interrupt

_tx_thread_synch_return:

#if defined(__riscv_float_abi_single)
    flw     f8, 15*REGBYTES(sp)                         // Recover fs0
    flw     f9, 16*REGBYTES(sp)                         // Recover fs1
    flw     f18,17*REGBYTES(sp)                         // Recover fs2
    flw     f19,18*REGBYTES(sp)                         // Recover fs3
    flw     f20,19*REGBYTES(sp)                         // Recover fs4
    flw     f21,20*REGBYTES(sp)                         // Recover fs5
    flw     f22,21*REGBYTES(sp)                         // Recover fs6
    flw     f23,22*REGBYTES(sp)                         // Recover fs7
    flw     f24,23*REGBYTES(sp)                         // Recover fs8
    flw     f25,24*REGBYTES(sp)                         // Recover fs9
    flw     f26,25*REGBYTES(sp)                         // Recover fs10
    flw     f27,26*REGBYTES(sp)                         // Recover fs11
    LOAD    t0, 27*REGBYTES(sp)                         // Recover fcsr
    csrw    fcsr, t0                                    //
#elif defined(__riscv_float_abi_double)
    fld     f8, 15*REGBYTES(sp)                         // Recover fs0
    fld     f9, 16*REGBYTES(sp)                         // Recover fs1
    fld     f18,17*REGBYTES(sp)                         // Recover fs2
    fld     f19,18*REGBYTES(sp)                         // Recover fs3
    fld     f20,19*REGBYTES(sp)                         // Recover fs4
    fld     f21,20*REGBYTES(sp)                         // Recover fs5
    fld     f22,21*REGBYTES(sp)                         // Recover fs6
    fld     f23,22*REGBYTES(sp)                         // Recover fs7
    fld     f24,23*REGBYTES(sp)                         // Recover fs8
    fld     f25,24*REGBYTES(sp)                         // Recover fs9
    fld     f26,25*REGBYTES(sp)                         // Recover fs10
    fld     f27,26*REGBYTES(sp)                         // Recover fs11
    LOAD    t0, 27*REGBYTES(sp)                         // Recover fcsr
    csrw    fcsr, t0                                    //
#endif

    /* Recover standard preserved registers.  */
    /* Recover standard registers.  */

    LOAD    x1,  13*REGBYTES(sp)                        // Recover RA
    LOAD    x8,  12*REGBYTES(sp)                        // Recover s0
    LOAD    x9,  11*REGBYTES(sp)                        // Recover s1
    LOAD    x18, 10*REGBYTES(sp)                        // Recover s2
    LOAD    x19,  9*REGBYTES(sp)                        // Recover s3
    LOAD    x20,  8*REGBYTES(sp)                        // Recover s4
    LOAD    x21,  7*REGBYTES(sp)                        // Recover s5
    LOAD    x22,  6*REGBYTES(sp)                        // Recover s6
    LOAD    x23,  5*REGBYTES(sp)                        // Recover s7
    LOAD    x24,  4*REGBYTES(sp)                        // Recover s8
    LOAD    x25,  3*REGBYTES(sp)                        // Recover s9
    LOAD    x26,  2*REGBYTES(sp)                        // Recover s10
    LOAD    x27,  1*REGBYTES(sp)                        // Recover s11
    LOAD    t0,  14*REGBYTES(sp)                        // Recover mstatus
    csrw    mstatus, t0                                 // Store mstatus, enables interrupt
#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, 29*REGBYTES                         // Recover stack frame
#else
    addi    sp, sp, 16*REGBYTES                         // Recover stack frame
#endif
    ret                                                 // Return to thread

/* }  */
