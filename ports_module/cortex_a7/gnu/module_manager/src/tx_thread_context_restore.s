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

    .syntax unified
#if defined(THUMB_MODE)
    .thumb
#else
    .arm
#endif

IRQ_MODE        =     0x12            // IRQ mode
SYS_MODE        =     0x1F            // SYS mode
SVC_MODE        =     0x13            // SVC mode

    .global     _tx_thread_system_state
    .global     _tx_thread_current_ptr
    .global     _tx_thread_execute_ptr
    .global     _tx_timer_time_slice
    .global     _tx_thread_schedule
    .global     _tx_thread_preempt_disable

    .text
    .align 2
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_restore                            ARMv7-A       */
/*                                                           6.3.0        */
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
/*  10-15-2021     William E. Lamie         Modified comment(s), added    */
/*                                            execution profile support,  */
/*                                            resulting in version 6.1.9  */
/*  04-25-2022     Zhen Kong                Updated comments,             */
/*                                            resulting in version 6.1.11 */
/*  10-31-2023     Yajun Xia                Updated comments,             */
/*                                            Added thumb mode support,   */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
#if defined(THUMB_MODE)
    .thumb_func
#endif
    .global _tx_thread_context_restore
    .type   _tx_thread_context_restore,function
_tx_thread_context_restore:

#ifdef TX_ENABLE_FIQ_SUPPORT
    CPSID   if                              // Disable IRQ and FIQ
#else
    CPSID   i                               // Disable IRQ
#endif

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the ISR exit function to indicate an ISR is complete.  */
    BL      _tx_execution_isr_exit          // Call the ISR exit function
#endif

    /* Check if interrupts are nested. */
    LDR     r1, =_tx_thread_system_state    // Load address of system state variable
    LDR     r0, [r1]                        // Load the counter
    SUBS    r0, r0, #1                      // Decrement the counter
    STR     r0, [r1]                        // Store the counter
    BNE     restore_and_return_from_irq     // If the counter is not 0, this is a nested restore, just return

    /* Check if a thread was interrupted and no preemption is required. */
    LDR     r1, =_tx_thread_current_ptr     // Load address of current thread ptr
    LDR     r0, [r1]                        // Load actual current thread pointer
    CMP     r0, #0                          // Is it NULL ?
    BEQ     restore_and_return_from_irq     // If the current thread pointer is NULL, idle system was interrupted, just return

    /* Check if the current thread can be preempted. */
    LDR     r3, =_tx_thread_preempt_disable // Load preempt disable address
    LDR     r2, [r3]                        // Load actual preempt disable flag
    CMP     r2, #0                          // Is it set ?
    BNE     restore_and_return_from_irq     // If the preempt disable flag is set, do not preempt, just return

    /* Check if the next thread is different of the current thread. */
    LDR     r3, =_tx_thread_execute_ptr     // Load address of execute thread ptr
    LDR     r2, [r3]                        // Load actual execute thread pointer
    CMP     r0, r2                          // Is the next thread the same as the current thread ?
    BEQ     restore_and_return_from_irq     // If this is the same thread, do not preempt, just return

    /* Clear the current task pointer. */
    MOV     r3, #0                          // NULL value
    STR     r3, [r1]                        // Clear current thread pointer

    POP     {r2, r10, r12, lr}              // Recover temporarily saved registers

    /* Save registers of the current thread on its stack */
    /* Save integer registers. */
    MOV     r1, lr                          // Save lr (point of interrupt)
    CPS     #SYS_MODE                       // Enter SYS mode
    STR     r1, [sp, #-4]!                  // Save point of interrupt
    PUSH    {r4-r12, lr}                    // Save upper half of registers
    CPS     #IRQ_MODE                       // Enter IRQ mode
    POP     {r4-r7}                         // Recover r0-r3
    CPS     #SYS_MODE                       // Enter SYS mode
    PUSH    {r4-r7}                         // Save r0-r3 on thread's stack
#ifdef TX_ENABLE_VFP_SUPPORT
    /* Save VFP registers. */
    LDR     r1, [r0, #144]                  // Pickup the VFP enabled flag
    CMP     r1, #0                          // Is the VFP enabled?
    BEQ     skip_vfp_save                   // No, skip VFP IRQ save
    VMRS    r1, FPSCR                       // Pickup the FPSCR
    STR     r1, [sp, #-4]!                  // Save FPSCR
    VSTMDB  sp!, {D16-D31}                  // Save D16-D31
    VSTMDB  sp!, {D0-D15}                   // Save D0-D15
skip_vfp_save:
#endif
    /* Save CPSR and stack type. */
    MOV     r1, #1                          // Build interrupt stack type
    PUSH    {r1, r2}                        // Save interrupt stack type and SPSR
    STR     sp, [r0, #8]                    // Save stack pointer in thread control block

    /* Save the remaining time-slice and disable it.  */
    LDR     r2, =_tx_timer_time_slice       // Pickup time-slice variable address
    LDR     r1, [r2]                        // Pickup time-slice
    CMP     r1, #0                          // Is it active?
    BEQ     dont_save_ts                    // No, don't save it
    STR     r1, [r0, #24]                   // Save thread's time-slice
    STR     r3, [r2]                        // Disable global time-slice flag
dont_save_ts:

    B       _tx_thread_schedule             // Go to the scheduler

    /* Return to point of interrupt */
restore_and_return_from_irq:
    /* Recover the saved context and return to the point of interrupt.  */
    POP     {r0, r10, r12, lr}              // Recover SPSR, POI, and scratch regs
    MSR     SPSR_cxsf, r0                   // Put SPSR back
    POP     {r0-r3}                         // Recover r0-r3
    MOVS    pc, lr                          // Return to point of interrupt
