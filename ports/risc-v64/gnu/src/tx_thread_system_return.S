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
/**************************************************************************/
/* VOID   _tx_thread_system_return(VOID)
{  */
    .global  _tx_thread_system_return
_tx_thread_system_return:

    /* Save minimal context on the stack.  */
    /* sp -= sizeof(stack_frame);  */

#if defined(__riscv_float_abi_single) || defined(__riscv_float_abi_double)
    addi    sp, sp, -29*8                               // Allocate space on the stack - with floating point enabled
#else
    addi    sp, sp, -16*8                               // Allocate space on the stack - without floating point enabled
#endif

    /* Store floating point preserved registers.  */
#if defined(__riscv_float_abi_single)
    fsw     f8,  15*8(sp)                               // Store fs0
    fsw     f9,  16*8(sp)                               // Store fs1
    fsw     f18, 17*8(sp)                               // Store fs2
    fsw     f19, 18*8(sp)                               // Store fs3
    fsw     f20, 19*8(sp)                               // Store fs4
    fsw     f21, 20*8(sp)                               // Store fs5
    fsw     f22, 21*8(sp)                               // Store fs6
    fsw     f23, 22*8(sp)                               // Store fs7
    fsw     f24, 23*8(sp)                               // Store fs8
    fsw     f25, 24*8(sp)                               // Store fs9
    fsw     f26, 25*8(sp)                               // Store fs10
    fsw     f27, 26*8(sp)                               // Store fs11
    csrr    t0, fcsr
    sd      t0, 27*8(sp)                                // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f8,  15*8(sp)                               // Store fs0
    fsd     f9,  16*8(sp)                               // Store fs1
    fsd     f18, 17*8(sp)                               // Store fs2
    fsd     f19, 18*8(sp)                               // Store fs3
    fsd     f20, 19*8(sp)                               // Store fs4
    fsd     f21, 20*8(sp)                               // Store fs5
    fsd     f22, 21*8(sp)                               // Store fs6
    fsd     f23, 22*8(sp)                               // Store fs7
    fsd     f24, 23*8(sp)                               // Store fs8
    fsd     f25, 24*8(sp)                               // Store fs9
    fsd     f26, 25*8(sp)                               // Store fs10
    fsd     f27, 26*8(sp)                               // Store fs11
    csrr    t0, fcsr
    sd      t0, 27*8(sp)                                // Store fcsr
#endif

    sd      zero,  0(sp)                                // Solicited stack type
    sd      ra,  13*8(sp)                               // Save return address
    sd      s0,  12*8(sp)                               // Save s0
    sd      s1,  11*8(sp)                               // Save s1
    sd      s2,  10*8(sp)                               // Save s2
    sd      s3,   9*8(sp)                               // Save s3
    sd      s4,   8*8(sp)                               // Save s4
    sd      s5,   7*8(sp)                               // Save s5
    sd      s6,   6*8(sp)                               // Save s6
    sd      s7,   5*8(sp)                               // Save s7
    sd      s8,   4*8(sp)                               // Save s8
    sd      s9,   3*8(sp)                               // Save s9
    sd      s10,  2*8(sp)                               // Save s10
    sd      s11,  1*8(sp)                               // Save s11
    csrr    t0, mstatus                                 // Pickup mstatus
    sd      t0, 14*8(sp)                                // Save mstatus


    /* Lockout interrupts. will be enabled in _tx_thread_schedule  */

    csrci   mstatus, 0x08                               // Disable interrupts (MIE bit 3)

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_exit                   // Call the thread execution exit function
#endif

    la      t0, _tx_thread_current_ptr                  // Pickup address of pointer
    ld      t1, 0(t0)                                   // Pickup current thread pointer
    la      t2, _tx_thread_system_stack_ptr             // Pickup stack pointer address

    /* Save current stack and switch to system stack.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  SP;
    SP = _tx_thread_system_stack_ptr;  */

    sd      sp, 16(t1)                                  // Save stack pointer
    ld      sp, 0(t2)                                   // Switch to system stack

    /* Determine if the time-slice is active.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t4, _tx_timer_time_slice                    // Pickup time slice variable addr
    ld      t3, 0(t4)                                   // Pickup time slice value
    la      t2, _tx_thread_schedule                     // Pickup address of scheduling loop
    beqz    t3, _tx_thread_dont_save_ts                 // If no time-slice, don't save it

        /* Save time-slice for the thread and clear the current time-slice.  */
        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
        _tx_timer_time_slice =  0;  */

    sd      t3, 48(t1)                                  // Save current time-slice for thread
    sd      zero, 0(t4)                                 // Clear time-slice variable

    /* }  */
_tx_thread_dont_save_ts:

    /* Clear the current thread pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    sd      x0, 0(t0)                                   // Clear current thread pointer
    jr      t2                                          // Return to thread scheduler

/* }  */
