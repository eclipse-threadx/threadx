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
/**   Module                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TXM_MODULE
#include "txm_module.h"
#ifndef TXM_THREAD_ENTRY_EXIT_NOTIFY_CALL_NOT_USED
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txe_thread_entry_exit_notify                       PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the thread entry/exit notify     */
/*    callback function call.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread             */
/*    thread_entry_exit_notify              Pointer to notify callback    */
/*                                            function, TX_NULL to disable*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Service return status         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_kernel_call_dispatcher                                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Module application code                                             */
/*                                                                        */
/**************************************************************************/
UINT _txe_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *notify_thread_ptr, UINT type))
{

UINT return_value;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_THREAD_ENTRY_EXIT_NOTIFY_CALL, (ALIGN_TYPE) thread_ptr, (ALIGN_TYPE) thread_entry_exit_notify, 0);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
