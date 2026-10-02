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

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "tx_thread.h"
#include "txm_module.h"
#include "txm_module_manager_util.h"

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
/*    code.                                                               */
/*                                                                        */
/*    Module authors: remove any explicit call to                         */
/*    txm_module_object_deallocate().  Calling the appropriate            */
/*    tx_*_delete() service is sufficient; pool deallocation is handled   */
/*    automatically by the dispatch layer.                                */
/*                                                                        */
/*    Only memory the calling module allocated from the object pool is    */
/*    released, and the allocation is identified by searching the         */
/*    module's own allocation list rather than by reading a private       */
/*    header from in front of the address the caller supplied.  An        */
/*    address that names none of this module's allocations returns        */
/*    TX_PTR_ERROR and is not dereferenced.                               */
/*    Memory holding a live kernel object is not given back.  Such a      */
/*    request returns TX_DELETE_ERROR and changes nothing: the object     */
/*    stays created, the allocation stays on the module's allocation      */
/*    list, and the memory stays owned by the module.  Storage that was   */
/*    allocated but never made into an object is still released, so a     */
/*    module can still clean up after a create that failed or was         */
/*    abandoned.                                                          */
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
/*    _txm_module_manager_allocated_object_find                           */
/*                                          Find the module's allocation  */
/*    _txm_module_manager_live_object_check  Check for a live object      */
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

        /* Find the allocation this address names.

           The private header in front of an allocation is the manager's own
           record of who the memory belongs to, but it is only a header when the
           address is one the manager gave out.  Reaching it by subtraction from
           whatever address the caller supplied assumes the answer: for an object
           the application allocated statically, or for any address outside the
           object pool, the words in front of it are unrelated memory, and this
           read them in privileged mode before deciding they were not a header.
           Every delete dispatcher reaches this function with the address the
           module named, so an object the module does not own arrived here as a
           matter of course rather than exceptionally.

           Searching the module's own allocation list answers the same question
           without that assumption.  The address is compared against the
           allocations the manager made for this module and is never
           dereferenced, so an address that names none of them -- an object the
           application owns, an object another module owns, or an address nowhere
           near the object pool -- is refused without a privileged read.  The
           search also establishes what the header read could not: that the
           address is the exact start of an allocation rather than somewhere
           inside one.  */
        module_allocated_object_ptr =  _txm_module_manager_allocated_object_find(module_instance, (ALIGN_TYPE) object_ptr, TX_NULL);

        /* Make sure the object is valid.  */
        if (module_allocated_object_ptr == TX_NULL)
        {
            /* Set return value to invalid pointer.  */
            return_value =  TX_PTR_ERROR;
        }

        /* Determine if a live kernel object is in this memory.

           Releasing the memory of a created object leaves the kernel holding the
           only references to it.  The object stays on the created list for its
           type, a thread stays on the ready or suspension list it was on and stays
           schedulable, and an active timer stays on the timer list.  None of those
           consult a control block ID, so nothing about the freed memory stops the
           kernel using it: a created list walk reads a name pointer out of it, a
           create or delete of another object of the same type writes through the
           links in it, the scheduler restores a context from the stack pointer in
           it, and timer expiration calls the function pointer in it.  The byte pool
           is meanwhile free to hand those bytes to the next allocation, so what the
           kernel goes on reading as a control block is whatever the next owner of
           the memory puts there.

           This is asked before the allocation is unlinked, so a refusal leaves the
           allocation list, the object and the memory exactly as they were.  What is
           refused is releasing the memory of an object that is still created; an
           allocation that was never made into an object, or one whose object has
           been deleted, is released as before, which is what keeps cleanup after a
           failed or abandoned create working and what makes the release the delete
           dispatchers perform after a successful delete go through.  */
        else if (_txm_module_manager_live_object_check((ALIGN_TYPE) object_ptr) == TX_TRUE)
        {

            /* Set return value to indicate the object must be deleted first.  */
            return_value =  TX_DELETE_ERROR;
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

            /* Clear the first word of the object being given back, which for every kernel
               object is its control block ID.

               Object authentication reads that ID to establish the type of an object and
               that it is still created, having first established that the address is the
               exact start of an allocation.  The kernel clears the ID when it deletes an
               object, so the ordinary sequence of delete and then deallocate has already
               cleared it.  What has not is a deallocation of an object that was never
               deleted: the memory returns to the pool still carrying a valid ID, and the
               next allocation to be placed there is a raw allocation that presents one.
               Clearing it here means the manager stops vouching for a control block at
               the moment it stops owning the memory, whatever order the module chose.

               An allocation too small to hold an ID cannot be presenting one, and is
               left alone rather than written past its end.  */
            if (module_allocated_object_ptr -> txm_module_object_size >= ((ULONG) sizeof(ULONG)))
            {

                *((ULONG *) object_ptr) =  TX_CLEAR_ID;
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
