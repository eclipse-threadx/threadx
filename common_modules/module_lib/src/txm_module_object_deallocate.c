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
#ifndef TXM_MODULE_OBJECT_DEALLOCATE_CALL_NOT_USED

/* DEPRECATION NOTICE
 * txm_module_object_deallocate() is deprecated and must not be used in new
 * code.  It is retained only for backward-compatibility with existing modules.
 *
 * WHY: when a module calls tx_timer_delete(), tx_semaphore_delete(), or any
 * other tx_*_delete() service, the Module Manager dispatch layer automatically
 * releases the associated object pool memory after the kernel cleanup
 * completes.  No separate deallocation call is required or expected.
 *
 * WHAT TO DO: remove any call to txm_module_object_deallocate() from your
 * module.  The corresponding tx_*_delete() call already handles everything.
 *
 * RISK: calling txm_module_object_deallocate() on a live kernel object (one
 * whose tx_*_delete() has not yet been called) frees the backing pool memory
 * while the object is still referenced by the kernel, which is undefined
 * behaviour (use-after-free).
 */
#pragma message("txm_module_object_deallocate() is deprecated and must not be used. " \
                "Call tx_*_delete() instead; the Module Manager dispatch layer " \
                "releases pool memory automatically on success.")
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_deallocate               PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deallocates a previously allocated object.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_ptr                        Object pointer to deallocate      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
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
UINT _txm_module_object_deallocate(VOID *object_ptr)
{

UINT return_value;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_MODULE_OBJECT_DEALLOCATE_CALL, (ALIGN_TYPE) object_ptr, 0, 0);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
