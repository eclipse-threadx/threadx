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

    IF :DEF:TX_ENABLE_FIQ_SUPPORT
INT_MASK        EQU         0xC0                // Interrupt bit mask
    ELSE
INT_MASK        EQU         0x80                // Interrupt bit mask
    ENDIF

    AREA ||.text||, CODE, READONLY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_control                    SMP/Cortex-R8/ARM   */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for changing the interrupt lockout     */
/*    posture of the system.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_posture                           New interrupt lockout posture */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    old_posture                           Old interrupt lockout posture */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2022      Scott Larson            Initial Version 6.2.0         */
/*                                                                        */
/**************************************************************************/
// UINT   _tx_thread_interrupt_control(UINT new_posture)
// {
    EXPORT  _tx_thread_interrupt_control
_tx_thread_interrupt_control

    /* Pickup current interrupt lockout posture.  */

    MRS     r3, CPSR                            // Pickup current CPSR
    BIC     r1, r3, #INT_MASK                   // Clear interrupt lockout bits
    ORR     r1, r1, r0                          // Or-in new interrupt lockout bits

    /* Apply the new interrupt posture.  */

    MSR     CPSR_c, r1                          // Setup new CPSR
    AND     r0, r3, #INT_MASK                   // Return previous interrupt mask
    IF  {INTER} = {TRUE}
    BX      lr                                  // Return to caller
    ELSE
    MOV     pc, lr                              // Return to caller
    ENDIF

// }

    END
