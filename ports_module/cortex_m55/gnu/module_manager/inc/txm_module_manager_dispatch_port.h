/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026 Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/
// Portions of this file were generated with AI assistance.


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Define port-specific dispatch functions. */

/* UINT _txe_thread_secure_stack_allocate(
    TX_THREAD *thread_ptr, -> param_0
    ULONG stack_size -> param_1
   ); */
static ALIGN_TYPE _txm_module_manager_tx_thread_secure_stack_allocate_dispatch(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE param_0, ALIGN_TYPE param_1)
{

ALIGN_TYPE return_value;

    if (module_instance -> txm_module_instance_property_flags & TXM_MODULE_MEMORY_PROTECTION)
    {
        if (TXM_MODULE_MANAGER_PARAM_CHECK_TYPED_OBJECT_FOR_USE(module_instance, param_0, TXM_THREAD_OBJECT) == TX_FALSE)
            return(TXM_MODULE_INVALID_MEMORY);
    }

    return_value = (ALIGN_TYPE) _txe_thread_secure_stack_allocate(
        (TX_THREAD *) param_0,
        (ULONG) param_1
    );
    return(return_value);
}

/* UINT _txe_thread_secure_stack_free(
    TX_THREAD *thread_ptr -> param_0
   ); */
static ALIGN_TYPE _txm_module_manager_tx_thread_secure_stack_free_dispatch(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE param_0)
{

ALIGN_TYPE return_value;

    if (module_instance -> txm_module_instance_property_flags & TXM_MODULE_MEMORY_PROTECTION)
    {
        if (TXM_MODULE_MANAGER_PARAM_CHECK_TYPED_OBJECT_FOR_USE(module_instance, param_0, TXM_THREAD_OBJECT) == TX_FALSE)
            return(TXM_MODULE_INVALID_MEMORY);
    }

    return_value = (ALIGN_TYPE) _txe_thread_secure_stack_free(
        (TX_THREAD *) param_0
    );
    return(return_value);
}
