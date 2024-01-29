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

#ifdef TX_INCLUDE_USER_DEFINE_FILE
#include "tx_user.h"
#endif

    EXTERN  _tx_execution_isr_exit
    SECTION `.text`:CODE:NOROOT(2)
    THUMB
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_restore                       Cortex-M4/IAR      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is only needed for legacy applications and it should  */
/*    not be called in any new development on a Cortex-M.                 */
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
/*    [_tx_execution_isr_exit]              Execution profiling ISR exit  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ISRs                                  Interrupt Service Routines    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021      Scott Larson            Initial Version 6.1.7         */
/*  10-31-2023      Tiejun Zhou             Included tx_user.h,           */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
// VOID   _tx_thread_context_restore(VOID)
// {
    PUBLIC  _tx_thread_context_restore
_tx_thread_context_restore:

#if (defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE))
    /* Call the ISR exit function to indicate an ISR is complete.  */
    PUSH    {r0, lr}                                // Save return address
    BL      _tx_execution_isr_exit                  // Call the ISR exit function
    POP     {r0, lr}                                // Recover return address
#endif

    BX      lr
// }
    END
