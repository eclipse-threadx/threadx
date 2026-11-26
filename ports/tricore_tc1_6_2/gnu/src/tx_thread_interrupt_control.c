/***************************************************************************
 * Copyright (c) 2026 safecubed GmbH
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/

/* Portions of this file were generated with AI assistance. */


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_control                        TriCore/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Andreas Neubauer, safecubed GmbH                                    */
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
/*  11-04-2026     Andreas Neubauer         Initial Version 6.5.2         */
/*                                                                        */
/**************************************************************************/
UINT   _tx_thread_interrupt_control(UINT new_posture)
{

    UINT icr;

    icr = __get_interrupt_posture();
    __restore_interrupt((icr & ~((UINT) TX_INT_ENABLE)) | (new_posture & ((UINT) TX_INT_ENABLE)));

    return(icr & ((UINT) TX_INT_ENABLE));
}


#ifdef TX_DISABLE_INLINE
UINT   _tx_thread_interrupt_disable(VOID)
{
    return(__disable_interrupts());
}


VOID   _tx_thread_interrupt_restore(UINT previous_posture)
{
    __restore_interrupt(previous_posture);
}
#endif


#ifdef TX_MISRA_ENABLE
ULONG  _tx_misra_interrupt_state_get(VOID)
{
    return((ULONG) __get_interrupt_state());
}
#endif

