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
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_system_return                           RISC-V32/GNU     */
/*                                                           6.4.x        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Akif Ejaz, 10xEngineers                                             */
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
    addi    sp, sp, -29*4                               // Allocate space on the stack - with floating point enabled
#else
    addi    sp, sp, -16*4                               // Allocate space on the stack - without floating point enabled
#endif

    /* Store floating point preserved registers.  */
#if defined(__riscv_float_abi_single)
    fsw     f8,  15*4(sp)                               // Store fs0
    fsw     f9,  16*4(sp)                               // Store fs1
    fsw     f18, 17*4(sp)                               // Store fs2
    fsw     f19, 18*4(sp)                               // Store fs3
    fsw     f20, 19*4(sp)                               // Store fs4
    fsw     f21, 20*4(sp)                               // Store fs5
    fsw     f22, 21*4(sp)                               // Store fs6
    fsw     f23, 22*4(sp)                               // Store fs7
    fsw     f24, 23*4(sp)                               // Store fs8
    fsw     f25, 24*4(sp)                               // Store fs9
    fsw     f26, 25*4(sp)                               // Store fs10
    fsw     f27, 26*4(sp)                               // Store fs11
    csrr    t0, fcsr
    sw      t0, 27*4(sp)                                // Store fcsr
#elif defined(__riscv_float_abi_double)
    fsd     f8,  15*4(sp)                               // Store fs0
    fsd     f9,  16*4(sp)                               // Store fs1
    fsd     f18, 17*4(sp)                               // Store fs2
    fsd     f19, 18*4(sp)                               // Store fs3
    fsd     f20, 19*4(sp)                               // Store fs4
    fsd     f21, 20*4(sp)                               // Store fs5
    fsd     f22, 21*4(sp)                               // Store fs6
    fsd     f23, 22*4(sp)                               // Store fs7
    fsd     f24, 23*4(sp)                               // Store fs8
    fsd     f25, 24*4(sp)                               // Store fs9
    fsd     f26, 25*4(sp)                               // Store fs10
    fsd     f27, 26*4(sp)                               // Store fs11
    csrr    t0, fcsr
    sw      t0, 27*4(sp)                                // Store fcsr
#endif

    sw      zero,  0(sp)                                // Solicited stack type
    sw      ra,  13*4(sp)                               // Save return address
    sw      s0,  12*4(sp)                               // Save s0
    sw      s1,  11*4(sp)                               // Save s1
    sw      s2,  10*4(sp)                               // Save s2
    sw      s3,   9*4(sp)                               // Save s3
    sw      s4,   8*4(sp)                               // Save s4
    sw      s5,   7*4(sp)                               // Save s5
    sw      s6,   6*4(sp)                               // Save s6
    sw      s7,   5*4(sp)                               // Save s7
    sw      s8,   4*4(sp)                               // Save s8
    sw      s9,   3*4(sp)                               // Save s9
    sw      s10,  2*4(sp)                               // Save s10
    sw      s11,  1*4(sp)                               // Save s11
    csrr    t0, mstatus                                 // Pickup mstatus
    sw      t0, 14*4(sp)                                // Save mstatus


    /* Lockout interrupts. will be enabled in _tx_thread_schedule  */

    csrci   mstatus, 0x08                               // Disable interrupts (MIE bit 3)

#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    call    _tx_execution_thread_exit                   // Call the thread execution exit function
#endif

    la      t0, _tx_thread_current_ptr                  // Pickup address of pointer
    lw      t1, 0(t0)                                   // Pickup current thread pointer
    la      t2, _tx_thread_system_stack_ptr             // Pickup stack pointer address

    /* Save current stack and switch to system stack.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  SP;
    SP = _tx_thread_system_stack_ptr;  */

    sw      sp, 8(t1)                                   // Save stack pointer
    lw      sp, 0(t2)                                   // Switch to system stack

    /* Determine if the time-slice is active.  */
    /* if (_tx_timer_time_slice)
    {  */

    la      t4, _tx_timer_time_slice                    // Pickup time slice variable addr
    lw      t3, 0(t4)                                   // Pickup time slice value
    la      t2, _tx_thread_schedule                     // Pickup address of scheduling loop
    beqz    t3, _tx_thread_dont_save_ts                 // If no time-slice, don't save it

        /* Save time-slice for the thread and clear the current time-slice.  */
        /* _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
        _tx_timer_time_slice =  0;  */

    sw      t3, 24(t1)                                  // Save current time-slice for thread
    sw      zero, 0(t4)                                 // Clear time-slice variable

    /* }  */
_tx_thread_dont_save_ts:

    /* Clear the current thread pointer.  */
    /* _tx_thread_current_ptr =  TX_NULL;  */

    sw      x0, 0(t0)                                   // Clear current thread pointer
    jr      t2                                          // Return to thread scheduler

/* }  */
