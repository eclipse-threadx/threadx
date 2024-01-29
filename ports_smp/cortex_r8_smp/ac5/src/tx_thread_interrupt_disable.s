/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
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

    AREA ||.text||, CODE, READONLY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_disable                    SMP/Cortex-R8/ARM   */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for disabling interrupts               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
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
// UINT   _tx_thread_interrupt_disable(void)
// {
    EXPORT _tx_thread_interrupt_disable
_tx_thread_interrupt_disable

    /* Pickup current interrupt lockout posture.  */

    MRS     r0, CPSR                            // Pickup current CPSR

    /* Mask interrupts.  */

    IF :DEF:TX_ENABLE_FIQ_SUPPORT
    CPSID   if                                  // Disable IRQ and FIQ
    ELSE
    CPSID   i                                   // Disable IRQ
    ENDIF

    IF  {INTER} = {TRUE}
    BX      lr                                  // Return to caller
    ELSE
    MOV     pc, lr                              // Return to caller
    ENDIF
// }

    END
