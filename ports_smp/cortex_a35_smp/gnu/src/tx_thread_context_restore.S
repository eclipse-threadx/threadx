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


/* Include macros for modifying the wait list.  */
#include "tx_thread_smp_protection_wait_list_macros.h"

    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_restore                         ARMv8-A-SMP      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-31-2022     Andres Mlinar            Updated comments,             */
/*                                             added ARMv8.2-A support,   */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_context_restore(VOID)
// {
    .global _tx_thread_context_restore
    .type   _tx_thread_context_restore, @function
_tx_thread_context_restore:

    /* Lockout interrupts.  */

    MSR     DAIFSet, 0x3                        // Lockout interrupts

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))

    /* Call the ISR exit function to indicate an ISR is complete.  */

    BL      _tx_execution_isr_exit              // Call the ISR exit function
#endif

    /* Pickup the CPU ID.   */

    MRS     x8, MPIDR_EL1                       // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x2, x8, #16, #8                     // Isolate cluster ID
#endif
    UBFX    x8, x8, #8, #8                      // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x2, x8, #8, #8                      // Isolate cluster ID
#endif
    UBFX    x8, x8, #0, #8                      // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x8, x8, x2, LSL #2                  // Calculate CPU ID
#endif

    /* Determine if interrupts are nested.  */
    // if (--_tx_thread_system_state)
    // {

    LDR     x3, =_tx_thread_system_state        // Pickup address of system state var
    LDR     w2, [x3, x8, LSL #2]                // Pickup system state
    SUB     w2, w2, #1                          // Decrement the counter
    STR     w2, [x3, x8, LSL #2]                // Store the counter
    CMP     w2, #0                              // Was this the first interrupt?
    BEQ     __tx_thread_not_nested_restore      // If so, not a nested restore

    /* Interrupts are nested.  */

    /* Just recover the saved registers and return to the point of
       interrupt.  */

    LDP     x4, x5, [sp], #16                   // Pickup saved SPSR/DAIF and ELR_EL
#ifdef EL1
    MSR     SPSR_EL1, x4                        // Setup SPSR for return
    MSR     ELR_EL1, x5                         // Setup point of interrupt
#else
#ifdef EL2
    MSR     SPSR_EL2, x4                        // Setup SPSR for return
    MSR     ELR_EL2, x5                         // Setup point of interrupt
#else
    MSR     SPSR_EL3, x4                        // Setup SPSR for return
    MSR     ELR_EL3, x5                         // Setup point of interrupt
#endif
#endif
    LDP     x18, x19, [sp], #16                 // Recover x18, x19
    LDP     x16, x17, [sp], #16                 // Recover x16, x17
    LDP     x14, x15, [sp], #16                 // Recover x14, x15
    LDP     x12, x13, [sp], #16                 // Recover x12, x13
    LDP     x10, x11, [sp], #16                 // Recover x10, x11
    LDP     x8,  x9,  [sp], #16                 // Recover x8, x9
    LDP     x6,  x7,  [sp], #16                 // Recover x6, x7
    LDP     x4,  x5,  [sp], #16                 // Recover x4, x5
    LDP     x2,  x3,  [sp], #16                 // Recover x2, x3
    LDP     x0,  x1,  [sp], #16                 // Recover x0, x1
    LDP     x29, x30, [sp], #16                 // Recover x29, x30
    ERET                                        // Return to point of interrupt

    // }
__tx_thread_not_nested_restore:

    /* Determine if a thread was interrupted and no preemption is required.  */
    // else if (((_tx_thread_current_ptr) && (_tx_thread_current_ptr == _tx_thread_execute_ptr)
    //           || (_tx_thread_preempt_disable))
    // {

    LDR     x1, =_tx_thread_current_ptr         // Pickup address of current thread ptr
    LDR     x0, [x1, x8, LSL #3]                // Pickup actual current thread pointer
    CMP     x0, #0                              // Is it NULL?
    BEQ     __tx_thread_idle_system_restore     // Yes, idle system was interrupted
    LDR     x3, =_tx_thread_execute_ptr         // Pickup address of execute thread ptr
    LDR     x2, [x3, x8, LSL #3]                // Pickup actual execute thread pointer
    CMP     x0, x2                              // Is the same thread highest priority?
    BEQ     __tx_thread_no_preempt_restore      // Same thread in the execute list,
                                                //   no preemption needs to happen
    LDR     x3, =_tx_thread_smp_protection      // Build address to protection structure
    LDR     w3, [x3, #4]                        // Pickup the owning core
    CMP     w3, w8                              // Is it this core?
    BNE     __tx_thread_preempt_restore         // No, proceed to preempt thread

    LDR     x3, =_tx_thread_preempt_disable     // Pickup preempt disable address
    LDR     w2, [x3, #0]                        // Pickup actual preempt disable flag
    CMP     w2, #0                              // Is it set?
    BEQ     __tx_thread_preempt_restore         // No, okay to preempt this thread

__tx_thread_no_preempt_restore:

    /* Restore interrupted thread or ISR.  */

    /* Pickup the saved stack pointer.  */
    // sp =  _tx_thread_current_ptr -> tx_thread_stack_ptr;

    LDR     x4, [x0, #8]                        // Switch to thread stack pointer
    MOV     sp, x4                              //

   /* Recover the saved context and return to the point of interrupt.  */

    LDP     x4, x5, [sp], #16                   // Pickup saved SPSR/DAIF and ELR_EL1
#ifdef EL1
    MSR     SPSR_EL1, x4                        // Setup SPSR for return
    MSR     ELR_EL1, x5                         // Setup point of interrupt
#else
#ifdef EL2
    MSR     SPSR_EL2, x4                        // Setup SPSR for return
    MSR     ELR_EL2, x5                         // Setup point of interrupt
#else
    MSR     SPSR_EL3, x4                        // Setup SPSR for return
    MSR     ELR_EL3, x5                         // Setup point of interrupt
#endif
#endif
    LDP     x18, x19, [sp], #16                 // Recover x18, x19
    LDP     x16, x17, [sp], #16                 // Recover x16, x17
    LDP     x14, x15, [sp], #16                 // Recover x14, x15
    LDP     x12, x13, [sp], #16                 // Recover x12, x13
    LDP     x10, x11, [sp], #16                 // Recover x10, x11
    LDP     x8,  x9,  [sp], #16                 // Recover x8, x9
    LDP     x6,  x7,  [sp], #16                 // Recover x6, x7
    LDP     x4,  x5,  [sp], #16                 // Recover x4, x5
    LDP     x2,  x3,  [sp], #16                 // Recover x2, x3
    LDP     x0,  x1,  [sp], #16                 // Recover x0, x1
    LDP     x29, x30, [sp], #16                 // Recover x29, x30
    ERET                                        // Return to point of interrupt

    // }
    // else
    // {
__tx_thread_preempt_restore:

    /* Was the thread being preempted waiting for the lock?  */
    // if (_tx_thread_smp_protect_wait_counts[this_core] != 0)
    // {

    LDR     x2, =_tx_thread_smp_protect_wait_counts // Load waiting count list
    LDR     w3, [x2, x8, LSL #2]                // Load waiting value for this core
    CMP     w3, #0
    BEQ     _nobody_waiting_for_lock            // Is the core waiting for the lock?

    /* Do we not have the lock? This means the ISR never got the inter-core lock.  */
    // if (_tx_thread_smp_protection.tx_thread_smp_protect_owned != this_core)
    // {

    LDR     x2, =_tx_thread_smp_protection      // Load address of protection structure
    LDR     w3, [x2, #4]                        // Pickup the owning core
    CMP     w8, w3                              // Compare our core to the owning core
    BEQ     _this_core_has_lock                 // Do we have the lock?

    /* We don't have the lock. This core should be in the list. Remove it.  */
    // _tx_thread_smp_protect_wait_list_remove(this_core);

    _tx_thread_smp_protect_wait_list_remove     // Call macro to remove core from the list
    B       _nobody_waiting_for_lock            // Leave

    // }
    // else
    // {
    /* We have the lock. This means the ISR got the inter-core lock, but
       never released it because it saw that there was someone waiting.
       Note this core is not in the list.  */

_this_core_has_lock:

    /* We're no longer waiting. Note that this should be zero since this happens during thread preemption.  */
    // _tx_thread_smp_protect_wait_counts[core]--;

    LDR     x2, =_tx_thread_smp_protect_wait_counts // Load waiting count list
    LDR     w3, [x2, x8, LSL #2]                // Load waiting value for this core
    SUB     w3, w3, #1                          // Decrement waiting value. Should be zero now
    STR     w3, [x2, x8, LSL #2]                // Store new waiting value

    /* Now release the inter-core lock.  */

    /* Set protected core as invalid.  */
    // _tx_thread_smp_protection.tx_thread_smp_protect_core = 0xFFFFFFFF;

    LDR     x2, =_tx_thread_smp_protection      // Load address of protection structure
    MOV     w3, #0xFFFFFFFF                     // Build invalid value
    STR     w3, [x2, #4]                        // Mark the protected core as invalid
    DMB     ISH                                 // Ensure that accesses to shared resource have completed

    /* Release protection.  */
    // _tx_thread_smp_protection.tx_thread_smp_protect_in_force = 0;

    MOV     w3, #0                              // Build release protection value
    STR     w3, [x2, #0]                        // Release the protection
    DSB     ISH                                 // To ensure update of the protection occurs before other CPUs awake

    /* Wake up waiting processors. Note interrupts are already enabled.  */

#ifdef TX_ENABLE_WFE
    SEV                                         // Send event to other CPUs
#endif

      // }
    // }

_nobody_waiting_for_lock:

    LDR     x4, [x0, #8]                        // Switch to thread stack pointer
    MOV     sp, x4                              //

    LDP     x4, x5, [sp], #16                   // Pickup saved SPSR/DAIF and ELR_EL1
    STP     x20, x21, [sp, #-16]!               // Save x20, x21
    STP     x22, x23, [sp, #-16]!               // Save x22, x23
    STP     x24, x25, [sp, #-16]!               // Save x24, x25
    STP     x26, x27, [sp, #-16]!               // Save x26, x27
    STP     x28, x29, [sp, #-16]!               // Save x28, x29
#ifdef ENABLE_ARM_FP
    LDR     w3, [x0, #268]                      // Pickup FP enable flag
    CMP     w3, #0                              // Is FP enabled?
    BEQ     _skip_fp_save                       // No, skip FP save
    STP     q0,  q1,  [sp, #-32]!               // Save q0, q1
    STP     q2,  q3,  [sp, #-32]!               // Save q2, q3
    STP     q4,  q5,  [sp, #-32]!               // Save q4, q5
    STP     q6,  q7,  [sp, #-32]!               // Save q6, q7
    STP     q8,  q9,  [sp, #-32]!               // Save q8, q9
    STP     q10, q11, [sp, #-32]!               // Save q10, q11
    STP     q12, q13, [sp, #-32]!               // Save q12, q13
    STP     q14, q15, [sp, #-32]!               // Save q14, q15
    STP     q16, q17, [sp, #-32]!               // Save q16, q17
    STP     q18, q19, [sp, #-32]!               // Save q18, q19
    STP     q20, q21, [sp, #-32]!               // Save q20, q21
    STP     q22, q23, [sp, #-32]!               // Save q22, q23
    STP     q24, q25, [sp, #-32]!               // Save q24, q25
    STP     q26, q27, [sp, #-32]!               // Save q26, q27
    STP     q28, q29, [sp, #-32]!               // Save q28, q29
    STP     q30, q31, [sp, #-32]!               // Save q30, q31
    MRS     x2, FPSR                            // Pickup FPSR
    MRS     x3, FPCR                            // Pickup FPCR
    STP     x2, x3, [sp, #-16]!                 // Save FPSR, FPCR
_skip_fp_save:
#endif
    STP     x4, x5, [sp, #-16]!                 // Save x4 (SPSR_EL3), x5 (ELR_E3)

    MOV     x3, sp                              // Move sp into x3
    STR     x3, [x0, #8]                        // Save stack pointer in thread control
                                                //   block
    LDR     x3, =_tx_thread_system_stack_ptr    // Pickup address of system stack
    LDR     x4, [x3, x8, LSL #3]                // Pickup system stack pointer
    MOV     sp, x4                              // Setup system stack pointer


    /* Save the remaining time-slice and disable it.  */
    // if (_tx_timer_time_slice)
    // {

    LDR     x3, =_tx_timer_time_slice           // Pickup time-slice variable address
    LDR     w2, [x3, x8, LSL #2]                // Pickup time-slice
    CMP     w2, #0                              // Is it active?
    BEQ     __tx_thread_dont_save_ts            // No, don't save it

        // _tx_thread_current_ptr -> tx_thread_time_slice =  _tx_timer_time_slice;
        // _tx_timer_time_slice =  0;

    STR     w2, [x0, #36]                       // Save thread's time-slice
    MOV     w2, #0                              // Clear value
    STR     w2, [x3, x8, LSL #2]                // Disable global time-slice flag

    // }
__tx_thread_dont_save_ts:


    /* Clear the current task pointer.  */
    // _tx_thread_current_ptr =  TX_NULL;

    MOV     x2, #0                              // NULL value
    STR     x2, [x1, x8, LSL #3]                // Clear current thread pointer

    /* Set bit indicating this thread is ready for execution.  */

    MOV     x2, #1                              // Build ready flag
    STR     w2, [x0, #260]                      // Set thread's ready flag
    DMB     ISH                                 // Ensure that accesses to shared resource have completed

    /* Return to the scheduler.  */
    // _tx_thread_schedule();

    // }

__tx_thread_idle_system_restore:

    /* Just return back to the scheduler!  */

    LDR    x1, =_tx_thread_schedule             // Build address for _tx_thread_schedule
#ifdef EL1
    MSR    ELR_EL1, x1                          // Setup point of interrupt
//    MOV    x1, #0x4                             // Setup EL1 return
//    MSR    spsr_el1, x1                         // Move into SPSR
#else
#ifdef EL2
    MSR    ELR_EL2, x1                          // Setup point of interrupt
//    MOV    x1, #0x8                             // Setup EL2 return
//    MSR    spsr_el2, x1                         // Move into SPSR
#else
    MSR    ELR_EL3, x1                          // Setup point of interrupt
//    MOV    x1, #0xC                             // Setup EL3 return
//    MSR    spsr_el3, x1                         // Move into SPSR
#endif
#endif
    ERET                                        // Return to scheduler
// }
