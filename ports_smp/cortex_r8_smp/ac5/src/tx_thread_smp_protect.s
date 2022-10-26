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

    AREA ||.text||, CODE, READONLY
        PRESERVE8
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_protect                          SMP/Cortex-R8/ARM   */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets protection for running inside the ThreadX        */
/*    source. This is acomplished by a combination of a test-and-set      */
/*    flag and periodically disabling interrupts.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Previous Status Register                                            */
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
    EXPORT  _tx_thread_smp_protect
_tx_thread_smp_protect

    MRS     r3, CPSR                            // Pickup current CPSR

    IF  :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ interrupts
    ELSE
    CPSID   i                                   // Disable IRQ interrupts
    ENDIF

    /* Pickup the CPU ID.   */

    MRC     p15, 0, r2, c0, c0, 5               // Read CPU ID register
    AND     r2, r2, #0x03                       // Mask off, leaving the CPU ID field
    LDR     r0,=_tx_thread_smp_protection       // Build address to protection structure
    LDR     r1, [r0, #8]                        // Pickup the owning core
    CMP     r1, r2                              // Is it this core?
    BEQ     _owned                              // Yes, the protection is already owned

    LDREX   r1, [r0]                            // Pickup the protection
    CMP     r1, #0                              // Is it available?
    BEQ     _get_protection                     // Yes, get the protection
    MSR     CPSR_c, r3                          // Restore CPSR
    WFE                                         // Go into standby
    B       _tx_thread_smp_protect              // On waking restart the protection attempt

_get_protection
    MOV     r1, #1                              // Build lock value
    STREX   r2, r1, [r0]                        // Attempt to get protection
    CMP     r2, #0x0                            // Check whether store completed successfully (0=succeeded)
    MSRNE   CPSR_c, r3                          // If unsuccessful, restore CPSR
    BNE     _tx_thread_smp_protect              // If unsuccessful, try the process again!
    DMB                                         ;
    MRC     p15, 0, r2, c0, c0, 5               // Read CPU ID register
    AND     r2, r2, #0x03                       // Mask off, leaving the CPU ID field
    STR     r2, [r0, #8]                        // Save owning core

    IF :DEF:TX_MPCORE_DEBUG_ENABLE
    LSL     r2, r2, #2                          // Build offset to array indexes
    LDR     r1, =_tx_thread_current_ptr         // Pickup start of the current thread array
    ADD     r1, r1, r2                          // Build index into the current thread array
    LDR     r2, [r1]                            // Pickup current thread for this core
    STR     r2, [r0, #4]                        // Save current thread pointer
    STR     LR, [r0, #16]                       // Save caller's return address
    STR     r3, [r0, #20]                       // Save CPSR
    ENDIF
_owned
    LDR     r1, [r0, #12]                       // Pickup ownership count
    ADD     r1, r1, #1                          // Increment ownership count
    STR     r1, [r0, #12]                       // Store new ownership count

    MOV     r0, r3                              // Return the CPSR

    IF  {INTER} = {TRUE}
    BX      lr                                  // Return to caller
    ELSE
    MOV     pc, lr                              // Return to caller
    ENDIF

    END
