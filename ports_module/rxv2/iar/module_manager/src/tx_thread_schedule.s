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

    EXTERN  __tx_thread_execute_ptr
    EXTERN  __tx_thread_current_ptr
    EXTERN  __tx_timer_time_slice
    IMPORT  __txm_module_manager_kernel_dispatch
    IMPORT  __tx_thread_system_state
    EXTERN  __txm_module_manager_memory_fault_info
    EXTERN  __txm_module_manager_memory_fault_handler

    section .text:CODE:ROOT

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                                  RXv2/IAR       */
/*                                                           6.1.10       */
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
/*  12-30-2020     William E. Lamie         Initial Version 6.1.3         */
/*  10-15-2021     William E. Lamie         Modified comment(s), and      */
/*                                            removed unnecessary stack   */
/*                                            type checking,              */
/*                                            resulting in version 6.1.9  */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_schedule(VOID)
// {
    public __tx_thread_schedule
__tx_thread_schedule:

    /* Enable interrupts.  */

    SETPSW  I

    /* Wait for a thread to execute.  */
    // do
    // {
    MOV.L   #__tx_thread_execute_ptr, R1        // Address of thread to executer ptr
__tx_thread_schedule_loop:
    MOV.L   [R1],R2                             // Pickup next thread to execute
    CMP     #0,R2                               // Is it NULL?
    BEQ     __tx_thread_schedule_loop           // Yes, idle system, keep checking

    // }
    // while(_tx_thread_execute_ptr == TX_NULL);

    /* Yes! We have a thread to execute.  Lockout interrupts and transfer control to it.  */

    CLRPSW  I                                   // Disable interrupts

    /* Setup the current thread pointer.  */
    // _tx_thread_current_ptr =  _tx_thread_execute_ptr;

    MOV.L   #__tx_thread_current_ptr, R3
    MOV.L   R2,[R3]                             // Setup current thread pointer

    /* Increment the run count for this thread.  */
    // _tx_thread_current_ptr -> tx_thread_run_count++;

    MOV.L   4[R2],R3                            // Pickup run count
    ADD     #1,R3                               // Increment run counter
    MOV.L   R3,4[R2]                            // Store it back in control block

    /* Setup time-slice, if present.  */
    // _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;

    MOV.L   24[R2],R3                           // Pickup thread time-slice
    MOV.L   #__tx_timer_time_slice,R4           // Pickup pointer to time-slice
    MOV.L   R3, [R4]                            // Setup time-slice

    /* Switch to the thread's stack.  */
    // SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;
    SETPSW  U                                   // User stack mode
    MOV.L   8[R2],SP                            // Pickup stack pointer


    /* Set up MPU. */
    // Disable MPU
    MOV.L   #0x00086500,R1                      // Pickup MPEN
    MOV     #0,R3                               // Build disable value
    MOV.L   R3,[R1]                             // Disable MPU
    // Determine if protection for this thread is required
    MOV.L   156[R2],R1                          // Pickup user_mode
    CMP     #0,R1                               // Is protection required for this thread?
    BEQ     skip_mpu_setup                      // No, skip MPU setup
    
    MOV.L   #0x00086408,R1                      // Region 1 Start Page Register address
    MOV.L   144[R2],R2                          // Address of module instance ptr at offset 144 in TCB
    ADD     #100,R2,R2                          // Get address of MPU table in module struct, starting at region 1
    
    // Region 0 (Trampoline from User mode to ThreadX) set up in txm_module_manager_setup_mpu_registers.c
    
    // Build region 1 (User code)
    MOV.L   [R2+],R4                            // Pickup region 1 start page, increment to region 1 end page
    MOV.L   R4,[R1+]                            // Setup region 1 start page reg, increment to region 1 end page reg.
    MOV.L   [R2+],R4                            // Pickup region 1 end page, increment to region 2 start page
    MOV.L   R4,[R1+]                            // Setup region 1 end page reg, increment to region 2 start page reg.
    
    // Build region 2 (User data)
    MOV.L   [R2+],R4                            // Pickup region 2 start page, increment to region 2 end page
    MOV.L   R4,[R1+]                            // Setup region 2 start page reg, increment to region 2 end page reg.
    MOV.L   [R2+],R4                            // Pickup region 2 end page, increment to region 3 start page
    MOV.L   R4,[R1+]                            // Setup region 2 end page reg, increment to region 3 start page reg.
    
    // Build region 3 (Shared memory)
    MOV.L   [R2+],R4                            // Pickup region 3 start page, increment to region 3 end page
    MOV.L   R4,[R1+]                            // Setup region 3 start page reg, increment to region 3 end page reg.
    MOV.L   [R2+],R4                            // Pickup region 3 end page, increment to region 4 start page
    MOV.L   R4,[R1+]                            // Setup region 3 end page reg, increment to region 4 start page reg.
    
    // Region 4-7 unused
    
    // Setup background region
    MOV.L   #0x00086504,R1                      // Pickup MPBAC
    MOV     #0,[R1]                             // Read/Write/Execute prohibited.
    // Enable MPU
    MOV.L   #0x00086500,R1                      // Pickup MPEN
    MOV     #1,[R1]                             // Enable MPU
    
skip_mpu_setup

    POPM    R1-R3                               // Restore accumulators.
    MVTACLO R3, A0
    MVTACHI R2, A0
    MVTACGU R1, A0
    POPM    R1-R3
    MVTACLO R3, A1
    MVTACHI R2, A1
    MVTACGU R1, A1

    POPM    R6-R13                              // Recover interrupt stack frame
    POPC    FPSW 
    POPM    R14-R15
    POPM    R3-R5
    POPM    R1-R2
    RTE                                         // Return to point of interrupt, this restores PC and PSW

// }

    extern __tx_thread_context_save
    extern __tx_thread_context_restore

/*  Software triggered interrupt used to perform context switches.
    The priority of this interrupt is set to the lowest priority within
    tx_initialize_low_level() and triggered by ThreadX when calling
    _tx_thread_system_return(). */
    
    public ___interrupt_27
___interrupt_27:

    PUSHM R1-R2

    BSR __tx_thread_context_save

    BRA __tx_thread_context_restore


/* You may have to modify BSP to use this handler. 
    // MPU Memory access violation
    PUBLIC ___excep_access_inst
    PUBLIC ___violation_handler
___excep_access_inst
___violation_handler
    // Disable interrupts
    CLRPSW  I                                   // disable interrupts
    // Save contents of R1 and R2
    PUSH    R1
    PUSH    R2
    // Increment and save system state
    MOV.L   #__tx_thread_system_state, R1       // Pickup address of system state
    MOV.L   [R1], R2                            // Pickup system state
    ADD     #1, R2                              // Increment
    MOV.L   R2, [R1]                            // Store new system state

    // Now pickup and store all the fault related information.

    MOV.L   #__txm_module_manager_memory_fault_info, R1
    MOV.L   #__tx_thread_current_ptr, R2        // Build current thread pointer address
    MOV.L   [R2], R2                            // Pickup the current thread pointer
    MOV.L   R2, 0[R1]                           // Save current thread pointer in fault info structure
    MOV.L   8[SP], R2                           // Pickup instruction address at point of fault
    MOV.L   R2, 4[R1]                           // Save point of fault
    MOV.L   #0x0008650C, R2                     // Pickup Memory-Protection Error Status Register
    MOV.L   [R2], 8[R1]                         // Save MPESTS
    MOV.L   #0x00086514, R2                     // Pickup Data Memory-Protection Error Address Register
    MOV.L   [R2], 12[R1]                        // Save MPDEA
    MOV.L   #0x00086528, R2                     // Pickup Instruction Hit Region Register
    MOV.L   [R2], 16[R1]                        // Save MHITI
    MOV.L   #0x0008652C, R2                     // Pickup Data Hit Region Register
    MOV.L   [R2], 20[R1]                        // Save MHITD
    MOV.L   12[SP], R2                          // Pickup PSW
    MOV.L   R2, 24[R1]                          // Save PSW
    MVFC    USP, R2                             // Pickup user stack pointer
    MOV.L   R2, 28[R1]                          // Save user stack pointer
    MOV.L   R3, 40[R1]                          // Save R3
    MOV.L   R4, 44[R1]                          // Save R4
    MOV.L   R5, 48[R1]                          // Save R5
    MOV.L   R6, 52[R1]                          // Save R6
    MOV.L   R7, 56[R1]                          // Save R7
    MOV.L   R8, 60[R1]                          // Save R8
    MOV.L   R9, 64[R1]                          // Save R9
    MOV.L   R10, 68[R1]                         // Save R10
    MOV.L   R11, 72[R1]                         // Save R11
    MOV.L   R12, 76[R1]                         // Save R12
    MOV.L   R13, 80[R1]                         // Save R13
    MOV.L   R14, 84[R1]                         // Save R14
    MOV.L   R15, 84[R1]                         // Save R15
    POP     R3                                  // Recall R1
    MOV.L   R3, 32[R1]                          // Save R1
    POP     R3                                  // Recall R2
    MOV.L   R3, 36[R1]                          // Save R2
    
    BSR     __txm_module_manager_memory_fault_handler    // Call memory manager fault handler
    
    // Decrement and save system state
    MOV.L   #__tx_thread_system_state, R1       // Pickup address of system state
    MOV.L   [R1], R2                            // Pickup system state
    SUB     #1, R2                              // Decrement
    MOV.L   R2, [R1]                            // Store new system state
    
    MOV.L   #__tx_thread_current_ptr, R2        // Pickup address of current thread pointer
    MOV.L   #0, [R2]                            // Clear current thread pointer
    BRA     __tx_thread_schedule                // Attempt to schedule the next thread
*/


    public ___interrupt_26
___interrupt_26:
    PUBLIC __txm_module_manager_kernel
__txm_module_manager_kernel

    MOV.L   [SP], R5                            // Get return address
    CMP     #__txm_module_manager_user_mode_entry+3, R5   // Did we come from user_mode_entry?
    BEQ     __txm_module_manager_entry          // If so, continue.
    RTE                                         // If not, then return from where we came.
    
__txm_module_manager_entry
    // We are entering the kernel from a module thread with user mode selected.
    // At this point, we are out of user mode!    
    // Clear current user mode.
    MOV.L   #__tx_thread_current_ptr, R5
    MOV     [R5],R5
    MOV     #0,152[R5]
    
    // Switch to kernel stack
    PUSHM   R1-R2
    MVFC    USP, R1                             // Pickup module thread stack pointer
    MOV.L   R1, 172[R5]                         // Save module thread stack pointer
    MOV.L   164[R5], R1                         // Pickup kernel stack end
#ifndef TXM_MODULE_KERNEL_STACK_MAINTENANCE_DISABLE
    MOV.L   R1, 16[R5]                          // Set stack end
    MOV.L   160[R5], R2                         // Pickup kernel stack start
    MOV.L   R2, 12[R5]                          // Set stack start
    MOV.L   168[R5], R2                         // Pickup kernel stack size
    MOV.L   R2, 20[R5]                          // Set stack size
#endif
    MVTC    R1, USP                             // Set stack pointer
    POPM    R1-R2

    // Modify PSW in ISP to return in supervisor mode and user stack
    MOV.L   4[SP], R5
    BCLR    #20, R5
    MOV.L   R5, 4[SP]
    
    // Return to user_mode_entry where kernel_dispatch will be called.
    RTE



    SECTION CODE:CODE (4)   // Align 4, smallest page size for MPU is 16 bytes.
    ALIGN   4
    PUBLIC __txm_module_manager_user_mode_entry
__txm_module_manager_user_mode_entry

    INT #26     // Enter ThreadX kernel (exit User mode).
    
    // At this point, we are out of user mode.
    // Simply call the kernel dispatch function.
    MOV.L #__txm_module_manager_kernel_dispatch,R5
    JSR R5
    
    // Restore user mode while inside of ThreadX.
    MOV.L   #__tx_thread_current_ptr, R5
    MOV     [R5],R5
    MOV     #1,152[R5]

    // Switch to module thread stack
#ifndef TXM_MODULE_KERNEL_STACK_MAINTENANCE_DISABLE
    PUSH    R1
    MOV.L   176[R5], R1                         // Pickup module thread stack start
    MOV.L   R1, 12[R5]                          // Set stack start
    MOV.L   180[R5], R1                         // Pickup module thread stack end
    MOV.L   R1, 16[R5]                          // Set stack end
    MOV.L   184[R5], R1                         // Pickup kernel stack size
    MOV.L   R1, 20[R5]                          // Set stack size
    POP     R1
#endif
    MOV.L   172[R5], R5                         // Pickup module thread stack pointer
    MVTC    R5, USP                             // Set stack pointer
    

    // USP is set for an RTS, need to set for RTE to set User mode in PSW.
    // Push return address on SP
    MOV.L  [SP],R5
    PUSH   R5
    // Set user mode and place PSW in prev position in SP
    MVFC   PSW,R5
    BSET   #20,R5
    MOV.L  R5,4[SP]
    RTE
    
    // Fill rest of page with NOPs.
    NOP
    NOP
    NOP
    NOP
    
    NOP
    NOP
    NOP
    NOP
    
    NOP
    NOP
    NOP
    NOP
    
    END
