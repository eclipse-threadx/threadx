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

    IMPORT     _tx_thread_current_ptr
    IMPORT     _tx_thread_smp_protection
    IMPORT     _tx_thread_preempt_disable

    AREA ||.text||, CODE, READONLY
        PRESERVE8
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_unprotect                        SMP/Cortex-R8/ARM   */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function releases previously obtained protection. The supplied */
/*    previous SR is restored. If the value of _tx_thread_system_state    */
/*    and _tx_thread_preempt_disable are both zero, then multithreading   */
/*    is enabled as well.                                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    Previous Status Register                                            */
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
/*    ThreadX Source                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2022      Scott Larson            Initial Version 6.2.0         */
/*                                                                        */
/**************************************************************************/
    EXPORT  _tx_thread_smp_unprotect
_tx_thread_smp_unprotect

    /* Lockout interrupts.  */

    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ interrupts
    ELSE
    CPSID   i                                   // Disable IRQ interrupts
    ENDIF

    LDR     r1,=_tx_thread_smp_protection       // Build address of protection structure
    MRC     p15, 0, r3, c0, c0, 5               // Read CPU ID register
    AND     r3, r3, #0x03                       // Mask off, leaving the CPU ID field
    LDR     r2, [r1, #8]                        // Pickup the owning core
    CMP     r2, r3                              // Is it this core?
    BNE     _still_protected                    // If this is not the owning core, protection is in force elsewhere

    LDR     r2, [r1, #12]                       // Pickup the protection count
    CMP     r2, #0                              // Check to see if the protection is still active
    BEQ     _still_protected                    // If the protection count is zero, protection has already been cleared

    SUB     r2, r2, #1                          // Decrement the protection count
    STR     r2, [r1, #12]                       // Store the new count back
    CMP     r2, #0                              // Check to see if the protection is still active
    BNE     _still_protected                    // If the protection count is non-zero, protection is still in force
    LDR     r2,=_tx_thread_preempt_disable      // Build address of preempt disable flag
    LDR     r3, [r2]                            // Pickup preempt disable flag
    CMP     r3, #0                              // Is the preempt disable flag set?
    BNE     _still_protected                    // Yes, skip the protection release
    MOV     r2, #0xFFFFFFFF                     // Build invalid value
    STR     r2, [r1, #8]                        // Mark the protected core as invalid
    IF :DEF:TX_MPCORE_DEBUG_ENABLE
    STR     LR, [r0, #16]                       // Save caller's return address
    ENDIF
    DMB                                         // Ensure that accesses to shared resource have completed
    MOV     r2, #0                              // Build release protection value
    STR     r2, [r1]                            // Release the protection
    DSB                                         // To ensure update of the protection occurs before other CPUs awake
    SEV                                         // Send event to other CPUs, wakes anyone waiting on the protection (using WFE)

_still_protected
    MSR     CPSR_c, r0                          // Restore CPSR

    IF  {INTER} = {TRUE}
    BX      lr                                  // Return to caller
    ELSE
    MOV     pc, lr                              // Return to caller
    ENDIF

    END
