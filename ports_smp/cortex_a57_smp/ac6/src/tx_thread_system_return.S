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
#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_system_return                           ARMv8-A-SMP      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is target processor specific.  It is used to transfer */
/*    control from a thread back to the ThreadX system.  Only a           */
/*    minimal context is saved since the compiler assumes temp registers  */
/*    are going to get slicked by a function call anyway.                 */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-31-2022     Andres Mlinar            Updated comments,             */
/*                                             added ARMv8.2-A support,   */
/*                                            resulting in version 6.1.10 */
/*  10-31-2023     Tiejun Zhou              Modified comment(s), added    */
/*                                            #include tx_user.h, added   */
/*                                            memory barrier,             */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_system_return(VOID)
// {
    .global _tx_thread_system_return
    .type   _tx_thread_system_return, @function
_tx_thread_system_return:

    /* Save minimal context on the stack.  */

    MRS     x0, DAIF                            // Pickup DAIF
    MSR     DAIFSet, 0x3                        // Lockout interrupts
    STP     x29, x30, [sp, #-16]!               // Save x29 (frame pointer), x30 (link register)
    STP     x19, x20, [sp, #-16]!               // Save x19, x20
    STP     x21, x22, [sp, #-16]!               // Save x21, x22
    STP     x23, x24, [sp, #-16]!               // Save x23, x24
    STP     x25, x26, [sp, #-16]!               // Save x25, x26
    STP     x27, x28, [sp, #-16]!               // Save x27, x28
    MRS     x8, MPIDR_EL1                       // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x3, x8, #16, #8                     // Isolate cluster ID
#endif
    UBFX    x8, x8, #8, #8                      // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x3, x8, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x8, x8, #0, #8                      // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x8, x8, x3, LSL #2                  // Calculate CPU ID
#endif
    LDR     x5, =_tx_thread_current_ptr         // Pickup address of current ptr
    LDR     x6, [x5, x8, LSL #3]                // Pickup current thread pointer

#ifdef ENABLE_ARM_FP
    LDR     w7, [x6, #268]                      // Pickup FP enable flag
    CMP     w7, #0                              // Is FP enabled?
    BEQ     _skip_fp_save                       // No, skip FP save
    STP     q8,  q9,  [sp, #-32]!               // Save q8, q9
    STP     q10, q11, [sp, #-32]!               // Save q10, q11
    STP     q12, q13, [sp, #-32]!               // Save q12, q13
    STP     q14, q15, [sp, #-32]!               // Save q14, q15
    MRS     x2, FPSR                            // Pickup FPSR
    MRS     x3, FPCR                            // Pickup FPCR
    STP     x2, x3, [sp, #-16]!                 // Save FPSR, FPCR
_skip_fp_save:
#endif

    MOV     x1, #0                              // Clear x1
    STP     x0, x1, [sp, #-16]!                 // Save DAIF and clear value for ELR_EK1

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))

    /* Call the thread exit function to indicate the thread is no longer executing.  */

    MOV     x19, x5                             // Save x5
    MOV     x20, x6                             // Save x6
    MOV     x21, x8                             // Save x2
    BL      _tx_execution_thread_exit           // Call the thread exit function
    MOV     x8, x21                             // Restore x2
    MOV     x5, x19                             // Restore x5
    MOV     x6, x20                             // Restore x6
#endif

    LDR     x2, =_tx_timer_time_slice           // Pickup address of time slice
    LDR     w1, [x2, x8, LSL #2]                // Pickup current time slice

    /* Save current stack and switch to system stack.  */
    // _tx_thread_current_ptr[core] -> tx_thread_stack_ptr =  sp;
    // sp = _tx_thread_system_stack_ptr[core];

    MOV     x4, sp                              //
    STR     x4, [x6, #8]                        // Save thread stack pointer
    LDR     x3, =_tx_thread_system_stack_ptr    // Pickup address of system stack
    LDR     x4, [x3, x8, LSL #3]                // Pickup system stack pointer
    MOV     sp, x4                              // Setup system stack pointer

    /* Determine if the time-slice is active.  */
    // if (_tx_timer_time_slice[core])
    // {

    MOV     x4, #0                              // Build clear value
    CMP     w1, #0                              // Is a time-slice active?
    BEQ     __tx_thread_dont_save_ts            // No, don't save the time-slice

       /* Save the current remaining time-slice.  */
       // _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
       // _tx_timer_time_slice =  0;

    STR     w4, [x2, x8, LSL #2]                // Clear time-slice
    STR     w1, [x6, #36]                       // Store current time-slice

    // }
__tx_thread_dont_save_ts:

    /* Clear the current thread pointer.  */
    // _tx_thread_current_ptr =  TX_NULL;

    STR     x4, [x5, x8, LSL #3]                // Clear current thread pointer

    /* Set ready bit in thread control block.  */

    DMB     ISH                                 // Ensure that accesses to shared resource have completed
    MOV     x3, #1                              // Build ready value
    STR     w3, [x6, #260]                      // Make the thread ready
    DMB     ISH                                 //

    /* Now clear protection. It is assumed that protection is in force whenever this routine is called.  */

    LDR     x3, =_tx_thread_smp_protection      // Pickup address of protection structure
    LDR     x1, =_tx_thread_preempt_disable     // Build address to preempt disable flag
    STR     w4, [x1, #0]                        // Clear preempt disable flag
    STR     w4, [x3, #8]                        // Cear protection count
    MOV     x1, #0xFFFFFFFF                     // Build invalid value
    STR     w1, [x3, #4]                        // Set core to an invalid value
    DMB     ISH                                 // Ensure that accesses to shared resource have completed
    STR     w4, [x3, #0]                        // Clear protection
    DSB     ISH                                 // To ensure update of the shared resource occurs before other CPUs awake
    SEV                                         // Send event to other CPUs, wakes anyone waiting on a mutex (using WFE)
    B       _tx_thread_schedule                 // Jump to scheduler!

// }
