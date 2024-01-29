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

    AREA ||.text||, CODE, READONLY
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_disable                     Cortex-Mx/AC5      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for disabling interrupts and returning */
/*    the previous interrupt lockout posture.                             */
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
/*  06-02-2021      Scott Larson            Initial Version 6.1.7         */
/*  10-31-2023      Tiejun Zhou             Included tx_user.h,           */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
// UINT   _tx_thread_interrupt_disable(VOID)
// {
    EXPORT  _tx_thread_interrupt_disable
_tx_thread_interrupt_disable
    /* Return current interrupt lockout posture.  */
#ifdef TX_PORT_USE_BASEPRI
    MRS     r0, BASEPRI
    LDR     r1, =TX_PORT_BASEPRI
    MSR     BASEPRI, r1
#else
    MRS     r0, PRIMASK
    CPSID   i
#endif
    BX      lr
// }
    END
