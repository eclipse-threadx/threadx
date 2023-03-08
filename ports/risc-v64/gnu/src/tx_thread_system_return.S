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
/*    _tx_thread_system_return                           RISC-V64/GNU     */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is target processor specific.  It is used to transfer */
/*    control from a thread back to the system.  Only a minimal context   */
/*    is saved since the compiler assumes temp registers are going to get */
/*    slicked by a function call anyway.                                  */
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
/*    _tx_thread_schedule                   Thread scheduling loop        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX components                                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  03-08-2023      Scott Larson            Initial Version 6.2.1         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_system_return(VOID)
{  */
    .global  _tx_thread_system_return
_tx_thread_system_return:

    /* Save minimal context on the stack.  */

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, -29*REGBYTES                        // Allocate space on the stack - with floating point enabled
#else
    addi    sp, sp, -16*REGBYTES                        // Allocate space on the stack - without floating point enabled
#endif

    /* Store floating point preserved registers.  */
#if defined(__riscv_float_abi_single)
    fsw     f8,  15*REGBYTES(sp)                        // Store fs0
    fsw     f9,  16*REGBYTES(sp)                        // Store fs1
    fsw     f18, 17*REGBYTES(sp)                        // Store fs2
    fsw     f19, 18*REGBYTES(sp)                        // Store fs3
    fsw     f20, 19*REGBYTES(sp)                        // Store fs4
    fsw     f21, 20*REGBYTES(sp)                        // Store fs5
    fsw     f22, 21*REGBYTES(sp)                        // Store fs6
    fsw     f23, 22*REGBYTES(sp)                        // Store fs7
    fsw     f24, 23*REGBYTES(sp)                        // Store fs8
    fsw     f25, 24*REGBYTES(sp)                        // Store fs9
    fsw     f26, 25*REGBYTES(sp)                        // Store fs10
    fsw     f27, 26*REGBYTES(sp)                        // Store fs11
    csrr    t0, fcsr
    STORE   t0, 27*REGBYTES(sp)                         // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f8,  15*REGBYTES(sp)                        // Store fs0
    fsd     f9,  16*REGBYTES(sp)                        // Store fs1
    fsd     f18, 17*REGBYTES(sp)                        // Store fs2
    fsd     f19, 18*REGBYTES(sp)                        // Store fs3
    fsd     f20, 19*REGBYTES(sp)                        // Store fs4
    fsd     f21, 20*REGBYTES(sp)                        // Store fs5
    fsd     f22, 21*REGBYTES(sp)                        // Store fs6
    fsd     f23, 22*REGBYTES(sp)                        // Store fs7
    fsd     f24, 23*REGBYTES(sp)                        // Store fs8
    fsd     f25, 24*REGBYTES(sp)                        // Store fs9
    fsd     f26, 25*REGBYTES(sp)                        // Store fs10
    fsd     f27, 26*REGBYTES(sp)                        // Store fs11
    csrr    t0, fcsr
    STORE   t0, 27*REGBYTES(sp)                         // Store fcsr
#endif

    STORE   x0,  0(sp)                                  // Solicited stack type
    STORE   x1,  13*REGBYTES(sp)                        // Save RA
    STORE   x8,  12*REGBYTES(sp)                        // Save s0
    STORE   x9,  11*REGBYTES(sp)                        // Save s1
    STORE   x18, 10*REGBYTES(sp)                        // Save s2
    STORE   x19,  9*REGBYTES(sp)                        // Save s3
    STORE   x20,  8*REGBYTES(sp)                        // Save s4
    STORE   x21,  7*REGBYTES(sp)                        // Save s5
    STORE   x22,  6*REGBYTES(sp)                        // Save s6
    STORE   x23,  5*REGBYTES(sp)                        // Save s7
    STORE   x24,  4*REGBYTES(sp)                        // Save s8
    STORE   x25,  3*REGBYTES(sp)                        // Save s9
    STORE   x26,  2*REGBYTES(sp)                        // Save s10
    STORE   x27,  1*REGBYTES(sp)                        // Save s11
    csrr    t0, mstatus                                 // Pickup mstatus
    STORE   t0, 14*REGBYTES(sp)                         // Save mstatus


   /* Lockout interrupts. - will be enabled in _tx_thread_schedule  */

    csrci   mstatus, 0xF

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_exit                   // Call the thread execution exit function
#endif

    la      t0, _tx_thread_current_ptr                  // Pickup address of pointer
    LOAD    t1, 0(t0)                                   // Pickup current thread pointer
    la      t2,_tx_thread_system_stack_ptr              // Pickup stack pointer address

    /* Save current stack and switch to system stack.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  SP;
    SP = _tx_thread_system_stack_ptr;  */

    STORE   sp, 2*REGBYTES(t1)                          // Save stack pointer
    LOAD    sp, 0(t2)                                   // Switch to system stack

    /* Determine if the time-slice is active.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t4, _tx_timer_time_slice                    // Pickup time slice variable addr
    LOAD    t3, 0(t4)                                   // Pickup time slice value
    la      t2, _tx_thread_schedule                     // Pickup address of scheduling loop
    beqz    t3, _tx_thread_dont_save_ts                 // If no time-slice, don't save it

        /* Save time-slice for the thread and clear the current time-slice.  */
        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
        _tx_timer_time_slice =  0;  */

    STORE   t3, 6*REGBYTES(t1)                          // Save current time-slice for thread
    STORE   x0, 0(t4)                                   // Clear time-slice variable

    /* }  */
_tx_thread_dont_save_ts:

    /* Clear the current thread pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    STORE   x0, 0(t0)                                   // Clear current thread pointer
    jr      t2                                          // Return to thread scheduler

/* }  */
