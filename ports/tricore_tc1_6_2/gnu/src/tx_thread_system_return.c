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

#define    TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_system_return                            TriCore/GNU     */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Andreas Neubauer, safecubed GmbH                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function transfers control from a thread back to the           */
/*    scheduler. Thread code enters the scheduler through the system      */
/*    call. Interrupt and trap handlers (PSW.IS = 1) request the          */
/*    dispatch interrupt instead. The function is compiled only with      */
/*    TX_DISABLE_INLINE; otherwise tx_port.h maps it to                   */
/*    _tx_thread_system_return_inline.                                    */
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
/*    _tx_thread_system_return_inline       System return (inline)        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX components                                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  11-04-2026     Andreas Neubauer         Initial Version 6.5.2         */
/*                                                                        */
/**************************************************************************/

#ifdef TX_DISABLE_INLINE
VOID   _tx_thread_system_return(VOID)
{
    _tx_thread_system_return_inline();
}
#endif

