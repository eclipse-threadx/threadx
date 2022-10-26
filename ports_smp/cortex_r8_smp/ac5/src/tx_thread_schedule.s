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

    IMPORT     _tx_thread_execute_ptr
    IMPORT     _tx_thread_current_ptr
    IMPORT     _tx_timer_time_slice
    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY
    IMPORT     _tx_execution_thread_enter
    ENDIF

    AREA ||.text||, CODE, READONLY
        PRESERVE8
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                             SMP/Cortex-R8/ARM   */
/*                                                           6.2.0        */
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
/*  10-31-2022      Scott Larson            Initial Version 6.2.0         */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_schedule(VOID)
// {
    EXPORT  _tx_thread_schedule
_tx_thread_schedule

    /* Enable interrupts.  */

    IF :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSIE   if                                  // Enable IRQ and FIQ interrupts
    ELSE
    CPSIE   i                                   // Enable IRQ interrupts
    ENDIF

    /* Pickup the CPU ID.   */

    MRC     p15, 0, r10, c0, c0, 5              // Read CPU ID register
    AND     r10, r10, #0x03                     // Mask off, leaving the CPU ID field
    LSL     r12, r10, #2                        // Build offset to array indexes

    LDR     r1, =_tx_thread_execute_ptr         // Address of thread execute ptr
    ADD     r1, r1, r12                         // Build offset to execute ptr for this core

    /* Lockout interrupts transfer control to it.  */

    IF :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ interrupts
    ELSE
    CPSID   i                                   // Disable IRQ interrupts
    ENDIF

    /* Wait for a thread to execute.  */
    // do
    // {

    LDR     r0, [r1, #0]                        // Pickup next thread to execute
    CMP     r0, #0                              // Is it NULL?
    BEQ     _tx_thread_schedule                 // If so, keep looking for a thread

    // }
    // while(_tx_thread_execute_ptr[core] == TX_NULL);

    /* Now make sure the thread's ready bit is set.  */

    LDR     r2, [r0, #152]                      // Pickup the thread ready bit
    AND     r3, r2, #0x8000                     // Isolate the ready bit
    CMP     r3, #0                              // Is it set?
    BEQ     _tx_thread_schedule                 // If not, restart the scheduling loop

    /* Yes! We have a thread to execute. ;

    /* Clear the ready bit.  */

    BIC     r2, r2, #0x8000                     // Clear ready bit
    STR     r2, [r0, #152]                      // Store it back in the thread control block
    DMB

    /* Setup the current thread pointer.  */
    // _tx_thread_current_ptr[core] =  _tx_thread_execute_ptr[core];

    LDR     r1, =_tx_thread_current_ptr         // Pickup address of current thread
    ADD     r1, r1, r12                         // Build index into the current thread array
    STR     r0, [r1, #0]                        // Setup current thread pointer

    /* Increment the run count for this thread.  */
    // _tx_thread_current_ptr[core] -> tx_thread_run_count++;

    LDR     r2, [r0, #4]                        // Pickup run counter
    LDR     r3, [r0, #24]                       // Pickup time-slice for this thread
    ADD     r2, r2, #1                          // Increment thread run-counter
    STR     r2, [r0, #4]                        // Store the new run counter

    /* Setup time-slice, if present.  */
    // _tx_timer_time_slice[core] =  _tx_thread_current_ptr[core] -> tx_thread_time_slice;

    LDR     r2, =_tx_timer_time_slice           // Pickup address of time slice
                                                //   variable
    ADD     r2, r2, r12                         // Build index into the time-slice array
    LDR     sp, [r0, #8]                        // Switch stack pointers
    STR     r3, [r2, #0]                        // Setup time-slice

    /* Switch to the thread's stack.  */
    // sp =  _tx_thread_execute_ptr[core] -> tx_thread_stack_ptr;

    IF :DEF:TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    /* Call the thread entry function to indicate the thread is executing.  */

    MOV     r5, r0                              // Save r0
    BL      _tx_execution_thread_enter          // Call the thread execution enter function
    MOV     r0, r5                              // Restore r0
    ENDIF

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    LDMIA   sp!, {r4, r5}                       // Pickup the stack type and saved CPSR
    CMP     r4, #0                              // Check for synchronous context switch
    BEQ     _tx_solicited_return
    MSR     SPSR_cxsf, r5                       // Setup SPSR for return
    IF  {TARGET_FPU_VFP} = {TRUE}
    LDR     r1, [r0, #160]                      // Pickup the VFP enabled flag
    CMP     r1, #0                              // Is the VFP enabled?
    BEQ     _tx_skip_interrupt_vfp_restore      // No, skip VFP interrupt restore
    VLDMIA  sp!, {D0-D15}                       // Recover D0-D15
    LDR     r4, [sp], #4                        // Pickup FPSCR
    VMSR    FPSCR, r4                           // Restore FPSCR
_tx_skip_interrupt_vfp_restore
    ENDIF
    LDMIA   sp!, {r0-r12, lr, pc}^              // Return to point of thread interrupt

_tx_solicited_return
    IF  {TARGET_FPU_VFP} = {TRUE}
    MSR     CPSR_cxsf, r5                       // Recover CPSR
    LDR     r1, [r0, #160]                      // Pickup the VFP enabled flag
    CMP     r1, #0                              // Is the VFP enabled?
    BEQ     _tx_skip_solicited_vfp_restore      // No, skip VFP solicited restore
    VLDMIA  sp!, {D0-D15}                       // Recover D0-D15
    LDR     r4, [sp], #4                        // Pickup FPSCR
    VMSR    FPSCR, r4                           // Restore FPSCR
_tx_skip_solicited_vfp_restore
    ENDIF
    MSR     CPSR_cxsf, r5                       // Recover CPSR
    LDMIA   sp!, {r4-r11, lr}                   // Return to thread synchronously
    BX      lr                                  // Return to caller

// }


    IF  {TARGET_FPU_VFP} = {TRUE}
    EXPORT  tx_thread_vfp_enable
tx_thread_vfp_enable
    MRS     r2, CPSR                            // Pickup the CPSR
    IF :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ interrupts
    ELSE
    CPSID   i                                   // Disable IRQ interrupts
    ENDIF
    MRC     p15, 0, r1, c0, c0, 5               // Read CPU ID register
    AND     r1, r1, #0x03                       // Mask off, leaving the CPU ID field
    LSL     r1, r1, #2                          // Build offset to array indexes
    LDR     r0, =_tx_thread_current_ptr         // Build current thread pointer address
    ADD     r0, r0, r1                          // Build index into the current thread array
    LDR     r1, [r0]                            // Pickup current thread pointer
    CMP     r1, #0                              // Check for NULL thread pointer
    BEQ     __tx_no_thread_to_enable            // If NULL, skip VFP enable
    MOV     r0, #1                              // Build enable value
    STR     r0, [r1, #160]                      // Set the VFP enable flag (tx_thread_vfp_enable field in TX_THREAD)
__tx_no_thread_to_enable
    MSR     CPSR_cxsf, r2                       // Recover CPSR
    BX      LR                                  // Return to caller

    EXPORT  tx_thread_vfp_disable
tx_thread_vfp_disable
    MRS     r2, CPSR                            // Pickup the CPSR
    IF :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ interrupts
    ELSE
    CPSID   i                                   // Disable IRQ interrupts
    ENDIF
    MRC     p15, 0, r1, c0, c0, 5               // Read CPU ID register
    AND     r1, r1, #0x03                       // Mask off, leaving the CPU ID field
    LSL     r1, r1, #2                          // Build offset to array indexes
    LDR     r0, =_tx_thread_current_ptr         // Build current thread pointer address
    ADD     r0, r0, r1                          // Build index into the current thread array
    LDR     r1, [r0]                            // Pickup current thread pointer
    CMP     r1, #0                              // Check for NULL thread pointer
    BEQ     __tx_no_thread_to_disable           // If NULL, skip VFP disable
    MOV     r0, #0                              // Build disable value
    STR     r0, [r1, #160]                      // Clear the VFP enable flag (tx_thread_vfp_enable field in TX_THREAD)
__tx_no_thread_to_disable
    MSR     CPSR_cxsf, r2                       // Recover CPSR
    BX      LR                                  // Return to caller
    ENDIF

// }

    END
