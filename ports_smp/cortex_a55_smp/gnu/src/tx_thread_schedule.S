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


    .text
    .align 3
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                                ARMv8-A-SMP      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-31-2022     Andres Mlinar            Updated comments,             */
/*                                             added ARMv8.2-A support,   */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_schedule(VOID)
// {
    .global _tx_thread_schedule
    .type   _tx_thread_schedule, @function
_tx_thread_schedule:

    /* Enable interrupts.  */

    MSR     DAIFClr, 0x3                        // Enable interrupts

    /* Pickup the CPU ID.   */

    MRS     x20, MPIDR_EL1                      // Pickup the core ID
#ifdef TX_ARMV8_2
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x1, x20, #16, #8                    // Isolate cluster ID
#endif
    UBFX    x20, x20, #8, #8                    // Isolate core ID
#else
#if TX_THREAD_SMP_CLUSTERS > 1
    UBFX    x1, x20, #8, #8                     // Isolate cluster ID
#endif
    UBFX    x20, x20, #0, #8                    // Isolate core ID
#endif
#if TX_THREAD_SMP_CLUSTERS > 1
    ADDS    x20, x20, x1, LSL #2                // Calculate CPU ID
#endif

    /* Wait for a thread to execute.  */
    // do
    // {

    LDR     x1, =_tx_thread_execute_ptr         // Address of thread execute ptr

#ifdef TX_ENABLE_WFI
__tx_thread_schedule_loop:
    MSR     DAIFSet, 0x3                        // Lockout interrupts
    LDR     x0, [x1, x20, LSL #3]               // Pickup next thread to execute
    CMP     x0, #0                              // Is it NULL?
    BNE     _tx_thread_schedule_thread          //
    MSR     DAIFClr, 0x3                        // Enable interrupts
    WFI                                         //
    B       __tx_thread_schedule_loop           // Keep looking for a thread
_tx_thread_schedule_thread:
#else
    MSR     DAIFSet, 0x3                        // Lockout interrupts
    LDR     x0, [x1, x20, LSL #3]               // Pickup next thread to execute
    CMP     x0, #0                              // Is it NULL?
    BEQ     _tx_thread_schedule                 // Keep looking for a thread
#endif

    // }
    // while(_tx_thread_execute_ptr == TX_NULL);

    /* Get the lock for accessing the thread's ready bit.  */

    MOV     w2, #280                            // Build offset to the lock
    ADD     x2, x0, x2                          // Get the address to the lock
    LDAXR   w3, [x2]                            // Pickup the lock value
    CMP     w3, #0                              // Check if it's available
    BNE     _tx_thread_schedule                 // No, lock not available
    MOV     w3, #1                              // Build the lock set value
    STXR    w4, w3, [x2]                        // Try to get the lock
    CMP     w4, #0                              // Check if we got the lock
    BNE     _tx_thread_schedule                 // No, another core got it first
    DMB     ISH                                 // Ensure write to lock completes

    /* Now make sure the thread's ready bit is set.  */

    LDR     w3, [x0, #260]                      // Pickup the thread ready bit
    CMP     w3, #0                              // Is it set?
    BNE     _tx_thread_ready_for_execution      // Yes, schedule the thread

    /* The ready bit isn't set. Release the lock and jump back to the scheduler.  */

    MOV     w3, #0                              // Build clear value
    STR     w3, [x2]                            // Release the lock
    DMB     ISH                                 // Ensure write to lock completes
    B       _tx_thread_schedule                 // Jump back to the scheduler

_tx_thread_ready_for_execution:

    /* We have a thread to execute. */

    /* Clear the ready bit and release the lock.  */

    MOV     w3, #0                              // Build clear value
    STR     w3, [x0, #260]                      // Store it back in the thread control block
    DMB     ISH
    MOV     w3, #0                              // Build clear value for the lock
    STR     w3, [x2]                            // Release the lock
    DMB     ISH

    /* Setup the current thread pointer.  */
    // _tx_thread_current_ptr =  _tx_thread_execute_ptr;

    LDR     x2, =_tx_thread_current_ptr         // Pickup address of current thread
    STR     x0, [x2, x20, LSL #3]               // Setup current thread pointer

    LDR     x1, [x1, x20, LSL #3]               // Reload the execute pointer
    CMP     w0, w1                              // Did it change?
    BEQ     _execute_pointer_did_not_change     // If not, skip handling

    /* In the time between reading the execute pointer and assigning
       it to the current pointer, the execute pointer was changed by
       some external code.  If the current pointer was still null when
       the external code checked if a core preempt was necessary, then
       it wouldn't have done it and a preemption will be missed.  To
       handle this, undo some things and jump back to the scheduler so
       it can schedule the new thread.  */

    MOV     w1, #0                              // Build clear value
    STR     x1, [x2, x20, LSL #3]               // Clear current thread pointer

    MOV     w1, #1                              // Build set value
    STR     w1, [x0, #260]                      // Re-set the ready bit
    DMB     ISH                                 //

    B       _tx_thread_schedule                 // Jump back to the scheduler to schedule the new thread

_execute_pointer_did_not_change:
    /* Increment the run count for this thread.  */
    // _tx_thread_current_ptr -> tx_thread_run_count++;

    LDR     w2, [x0, #4]                        // Pickup run counter
    LDR     w3, [x0, #36]                       // Pickup time-slice for this thread
    ADD     w2, w2, #1                          // Increment thread run-counter
    STR     w2, [x0, #4]                        // Store the new run counter

    /* Setup time-slice, if present.  */
    // _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;

    LDR     x2, =_tx_timer_time_slice           // Pickup address of time slice
                                                //   variable
    LDR     x4, [x0, #8]                        // Switch stack pointers
    MOV     sp, x4                              //
    STR     w3, [x2, x20, LSL #2]               // Setup time-slice

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))

    /* Call the thread entry function to indicate the thread is executing.  */

    MOV     x19, x0                             // Save x0
    BL      _tx_execution_thread_enter          // Call the thread execution enter function
    MOV     x0, x19                             // Restore x0
#endif

    /* Switch to the thread's stack.  */
    // sp =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    LDP     x4, x5, [sp], #16                   // Pickup saved SPSR/DAIF and ELR_EL1
    CMP     x5, #0                              // Check for synchronous context switch (ELR_EL1 = NULL)
    BEQ     _tx_solicited_return
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
#ifdef ENABLE_ARM_FP
    LDR     w1, [x0, #268]                      // Pickup FP enable flag
    CMP     w1, #0                              // Is FP enabled?
    BEQ     _skip_interrupt_fp_restore          // No, skip FP restore
    LDP     x0, x1,   [sp], #16                 // Pickup FPSR, FPCR
    MSR     FPSR, x0                            // Recover FPSR
    MSR     FPCR, x1                            // Recover FPCR
    LDP     q30, q31, [sp], #32                 // Recover q30, q31
    LDP     q28, q29, [sp], #32                 // Recover q28, q29
    LDP     q26, q27, [sp], #32                 // Recover q26, q27
    LDP     q24, q25, [sp], #32                 // Recover q24, q25
    LDP     q22, q23, [sp], #32                 // Recover q22, q23
    LDP     q20, q21, [sp], #32                 // Recover q20, q21
    LDP     q18, q19, [sp], #32                 // Recover q18, q19
    LDP     q16, q17, [sp], #32                 // Recover q16, q17
    LDP     q14, q15, [sp], #32                 // Recover q14, q15
    LDP     q12, q13, [sp], #32                 // Recover q12, q13
    LDP     q10, q11, [sp], #32                 // Recover q10, q11
    LDP     q8,  q9,  [sp], #32                 // Recover q8, q9
    LDP     q6,  q7,  [sp], #32                 // Recover q6, q7
    LDP     q4,  q5,  [sp], #32                 // Recover q4, q5
    LDP     q2,  q3,  [sp], #32                 // Recover q2, q3
    LDP     q0,  q1,  [sp], #32                 // Recover q0, q1
_skip_interrupt_fp_restore:
#endif
    LDP     x28, x29, [sp], #16                 // Recover x28
    LDP     x26, x27, [sp], #16                 // Recover x26, x27
    LDP     x24, x25, [sp], #16                 // Recover x24, x25
    LDP     x22, x23, [sp], #16                 // Recover x22, x23
    LDP     x20, x21, [sp], #16                 // Recover x20, x21
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

_tx_solicited_return:

#ifdef ENABLE_ARM_FP
    LDR     w1, [x0, #268]                      // Pickup FP enable flag
    CMP     w1, #0                              // Is FP enabled?
    BEQ     _skip_solicited_fp_restore          // No, skip FP restore
    LDP     x0, x1,   [sp], #16                 // Pickup FPSR, FPCR
    MSR     FPSR, x0                            // Recover FPSR
    MSR     FPCR, x1                            // Recover FPCR
    LDP     q14, q15, [sp], #32                 // Recover q14, q15
    LDP     q12, q13, [sp], #32                 // Recover q12, q13
    LDP     q10, q11, [sp], #32                 // Recover q10, q11
    LDP     q8,  q9,  [sp], #32                 // Recover q8, q9
_skip_solicited_fp_restore:
#endif
    LDP     x27, x28, [sp], #16                 // Recover x27, x28
    LDP     x25, x26, [sp], #16                 // Recover x25, x26
    LDP     x23, x24, [sp], #16                 // Recover x23, x24
    LDP     x21, x22, [sp], #16                 // Recover x21, x22
    LDP     x19, x20, [sp], #16                 // Recover x19, x20
    LDP     x29, x30, [sp], #16                 // Recover x29, x30
    MSR     DAIF, x4                            // Recover DAIF
    RET                                         // Return to caller
// }
