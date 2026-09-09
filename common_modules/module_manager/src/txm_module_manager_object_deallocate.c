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
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "tx_thread.h"
#include "txm_module.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_deallocate               PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    DEPRECATED.  This function is called internally by the Module       */
/*    Manager dispatch layer after a successful tx_*_delete() call from   */
/*    a module.  It must not be called directly by module or application  */
/*    code.  Calling it on a live kernel object (one whose tx_*_delete()  */
/*    has not yet been called) frees the backing pool memory while the    */
/*    object is still referenced by the kernel (use-after-free).          */
/*                                                                        */
/*    Module authors: remove any explicit call to                         */
/*    txm_module_object_deallocate().  Calling the appropriate            */
/*    tx_*_delete() service is sufficient; pool deallocation is handled   */
/*    automatically by the dispatch layer.                                */
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
/*    _txe_mutex_get                        Get module instance mutex     */
/*    _txe_mutex_put                        Release module instance mutex */
/*    _txe_byte_release                     Release object back to pool   */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_object_deallocate(VOID *object_ptr)
{
TXM_MODULE_INSTANCE         *module_instance;
TXM_MODULE_ALLOCATED_OBJECT *module_allocated_object_ptr;
UINT                        return_value;

    /* Get module manager protection mutex.  */
    _txe_mutex_get(&_txm_module_manager_mutex, TX_WAIT_FOREVER);

    /* Determine if an object pool was created.  */
    if (_txm_module_manager_object_pool_created)
    {

    TXM_MODULE_ALLOCATED_OBJECT   *next_object, *previous_object;

        /* Pickup module instance pointer.  */
        module_instance =  _tx_thread_current_ptr -> tx_thread_module_instance_ptr;

        /* Setup the memory pointer.  */
        module_allocated_object_ptr =  (TXM_MODULE_ALLOCATED_OBJECT *) object_ptr;

        /* Position the object pointer backwards to position back to the module manager information.  */
        previous_object =  module_allocated_object_ptr--;

        /* Make sure the object is valid.  */
        if ((module_allocated_object_ptr == TX_NULL) || (module_allocated_object_ptr -> txm_module_allocated_object_module_instance != module_instance) || (module_instance -> txm_module_instance_object_list_count == 0))
        {
            /* Set return value to invalid pointer.  */
            return_value =  TX_PTR_ERROR;
        }
        else
        {

            /* Unlink the node.  */
            if ((--module_instance -> txm_module_instance_object_list_count) == 0)
            {
                /* Only allocated object, just set the allocated list to NULL.  */
                module_instance -> txm_module_instance_object_list_head =  TX_NULL;
            }
            else
            {
                /* Otherwise, not the only allocated object, link-up the neighbors.  */
                next_object =                                           module_allocated_object_ptr -> txm_module_allocated_object_next;
                previous_object =                                       module_allocated_object_ptr -> txm_module_allocated_object_previous;
                next_object -> txm_module_allocated_object_previous =   previous_object;
                previous_object -> txm_module_allocated_object_next =   next_object;

                /* See if we have to update the allocated object list head pointer.  */
                if (module_instance -> txm_module_instance_object_list_head == module_allocated_object_ptr)
                {
                    /* Yes, move the head pointer to the next link. */
                    module_instance -> txm_module_instance_object_list_head =  next_object;
                }
            }

            /* Release the object memory.  */
            return_value =  (ULONG)  _txe_byte_release((VOID *) module_allocated_object_ptr);
        }
    }
    else
    {
        /* Set return value to not enabled.  */
        return_value =  TX_NOT_AVAILABLE;
    }

    /* Release the protection mutex.  */
    _txe_mutex_put(&_txm_module_manager_mutex);

    return(return_value);
}
