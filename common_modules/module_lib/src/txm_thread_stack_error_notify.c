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
#ifndef TXM_THREAD_STACK_ERROR_NOTIFY_CALL_NOT_USED
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_error_notify                       PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function registers an application stack error handler. If      */
/*    ThreadX detects a stack error, this application handler is called.  */
/*                                                                        */
/*    Note: stack checking must be enabled for this routine to serve any  */
/*    purpose via the TX_ENABLE_STACK_CHECKING define.                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    stack_error_handler                   Pointer to stack error        */
/*                                            handler, TX_NULL to disable */
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
UINT _tx_thread_stack_error_notify(VOID (*stack_error_handler)(TX_THREAD *thread_ptr))
{

UINT return_value;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_THREAD_STACK_ERROR_NOTIFY_CALL, (ALIGN_TYPE) stack_error_handler, 0, 0);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
