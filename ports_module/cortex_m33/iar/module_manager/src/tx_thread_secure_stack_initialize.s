/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
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

    SECTION `.text`:CODE:NOROOT(2)
    THUMB
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_secure_stack_initialize                Cortex-M33/IAR    */
/*                                                           6.1.12       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function enters the SVC handler to initialize a secure stack.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    none                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    none                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    SVC 3                                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    TX_PORT_SPECIFIC_PRE_INITIALIZATION                                 */
/**************************************************************************/
// VOID   _tx_thread_secure_stack_initialize(VOID)
// {
    EXPORT _tx_thread_secure_stack_initialize
_tx_thread_secure_stack_initialize:
#if !defined(TX_SINGLE_MODE_SECURE) && !defined(TX_SINGLE_MODE_NON_SECURE)
    CPSIE   i               // Enable interrupts for SVC call
    SVC     3
    CPSID   i               // Disable interrupts
#else
    MOV     r0, #0xFF       // Feature not enabled
#endif
    BX      lr
    END
