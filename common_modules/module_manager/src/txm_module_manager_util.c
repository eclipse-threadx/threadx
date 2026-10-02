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


/* Include necessary system files.  */

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_queue.h"
#include "tx_event_flags.h"
#include "tx_semaphore.h"
#include "tx_mutex.h"
#include "tx_block_pool.h"
#include "tx_byte_pool.h"
#include "txm_module.h"
#include "txm_module_manager_util.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_memory_check             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks if the object is inside a module's object pool */
/*    or, if it's a privileged module, inside the module's data area.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Module instance that the object   */
/*                                        belongs to                      */
/*    object_ptr                        Pointer to object to check        */
/*    object_size                       Size of the object to check       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Whether the object resides in a   */
/*                                        valid location                  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_kernel_dispatch   Kernel dispatch function      */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_object_memory_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size)
{

    /* Is the object pointer from the module manager's object pool?  */
    if ((_txm_module_manager_object_pool_created == TX_TRUE) &&
        (object_ptr >= (ALIGN_TYPE) _txm_module_manager_object_pool.tx_byte_pool_start) &&
        ((object_ptr+object_size) <= (ALIGN_TYPE) (_txm_module_manager_object_pool.tx_byte_pool_start + _txm_module_manager_object_pool.tx_byte_pool_size)))
    {
        /* Object is from manager object pool.  */
        return(TX_SUCCESS);
    }

    /* If memory protection is not required, check if object is in module data.  */
    else if (!(module_instance -> txm_module_instance_property_flags & TXM_MODULE_MEMORY_PROTECTION))
    {
        if ((object_ptr >= (ALIGN_TYPE) module_instance -> txm_module_instance_data_start) &&
        ((object_ptr+object_size) <= (ALIGN_TYPE) module_instance -> txm_module_instance_data_end))
        {
            /* Object is from the local module memory.  */
            return(TX_SUCCESS);
        }
    }

    /* Object is from invalid memory.  */
    return(TXM_MODULE_INVALID_MEMORY);

}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_created_object_check            PORTABLE C      */
/*                                                           6.1x         */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This functions checks if the specified object was created by the    */
/*    specified module                                                    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   The module instance to check      */
/*    object_ptr                        The object to check               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Whether the module created the    */
/*                                        object                          */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    txm_module_manager*_stop              Module manager stop functions */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*  xx-xx-2025      William E. Lamie        Modified comment(s), and      */
/*                                            removed module local memory */
/*                                            check, resulting in         */
/*                                            version 6.1x                */
/*                                                                        */
/**************************************************************************/
UCHAR _txm_module_manager_created_object_check(TXM_MODULE_INSTANCE *module_instance, VOID *object_ptr)
{

TXM_MODULE_ALLOCATED_OBJECT     *allocated_object_ptr;


    /* Determine if the object pool has been created.  */
    if (_txm_module_manager_object_pool_created)
    {

        /* Determine if the current object is from the pool of dynamically allocated objects.  */
        if ((((UCHAR *) object_ptr) >= _txm_module_manager_object_pool.tx_byte_pool_start) &&
            (((UCHAR *) object_ptr) < (_txm_module_manager_object_pool.tx_byte_pool_start + _txm_module_manager_object_pool.tx_byte_pool_size)))
        {

            /* Pickup object pointer.  */
            allocated_object_ptr =  (TXM_MODULE_ALLOCATED_OBJECT *) object_ptr;

            /* Move back to get the header information.  */
            allocated_object_ptr--;

            /* Now determine if this object belongs to this module.  */
            if (allocated_object_ptr -> txm_module_allocated_object_module_instance == module_instance)
            {
                return TX_TRUE;
            }
        }
    }

    return TX_FALSE;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_size_check               PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks if the specified object's size matches what is */
/*    inside the object pool.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_ptr                        Pointer to object to check        */
/*    object_size                       Size of the object to check       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Whether the object's size matches */
/*                                        what's inside the object pool   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_kernel_dispatch   Kernel dispatch function      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_object_size_check(ALIGN_TYPE object_ptr, ULONG object_size)
{
TXM_MODULE_ALLOCATED_OBJECT *module_allocated_object_ptr;
UINT                        return_value;

    /* Pickup the allocated object pointer.  */
    module_allocated_object_ptr = ((TXM_MODULE_ALLOCATED_OBJECT *) object_ptr) - 1;

    /* Does the allocated memory match the expected object size?  */
    if (module_allocated_object_ptr -> txm_module_object_size == object_size)
        return_value =  TX_SUCCESS;
    else
        return_value =  TXM_MODULE_INVALID_MEMORY;

    return(return_value);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_name_compare                    PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function compares the specified object names.                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    search_name                       String pointer to the object's    */
/*                                        name being searched for         */
/*    search_name_length                Length of search_name             */
/*    object_name                       String pointer to an object's name*/
/*                                        to compare the search name to   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Whether the names are equal       */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    *_object_pointer_get                  Kernel dispatch function      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_object_name_compare(CHAR *search_name, UINT search_name_length, TX_NAME_CONST CHAR *object_name)
{

CHAR    search_name_char;
CHAR    object_name_char;


    /* Is the object name null? Note that the search name has already been checked
       by the caller.  */
    if (object_name == TX_NULL)
    {

        /* The strings can't match.  */
        return(TX_FALSE);
    }

    /* Loop through the names.  */
    while (1)
    {

        /* Get the current characters from each name.  */
        search_name_char =  *search_name;
        object_name_char =  *object_name;

        /* Check for match.  */
        if (search_name_char == object_name_char)
        {

            /* Are they null-terminators?  */
            if (search_name_char == '\0')
            {

                /* The strings match.  */
                return(TX_TRUE);
            }
        }
        else
        {

            /* The strings don't match.  */
            return(TX_FALSE);
        }

        /* Are we at the end of the search name?  */
        if (search_name_length == 0)
        {

            /* The strings don't match.  */
            return(TX_FALSE);
        }

        /* Move to next character.  */
        search_name++;
        object_name++;
        search_name_length--;
    }
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_param_check_object_for_creation PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, RTOSX                                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks to make sure the object pointer for one of the */
/*    creation APIs is valid.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Requesting module instance pointer*/
/*    object_ptr                        Address of object memory area     */
/*    ojbect_size                       Size of object memory area        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           Valid object pointer              */
/*    TX_FALSE                          Invalid object pointer            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    txm_module_manager_*              Module manager functions          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2025      William E. Lamie        Initial Version 6.4.3         */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_param_check_object_for_creation(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size)
{

    /* Determine if the object pointer is NULL.  */
    if ((void *) object_ptr == TX_NULL)
    {

        /* Object pointer is NULL, which is invalid.  */
        return(TX_FALSE);
    }

    /* Determine if the object pointer is inside the module object pool.  */
    if (TXM_MODULE_MANAGER_ENSURE_INSIDE_OBJ_POOL(module_instance, object_ptr, object_size) == TX_FALSE)
    {

        /* Object pointer is not inside the object pool, which is invalid.  */
        return(TX_FALSE);
    }

    /* Determine if the object size is correct.  */
    if (_txm_module_manager_object_size_check(object_ptr, object_size) != TX_SUCCESS)
    {

        /* Object size is invalid.  */
        return(TX_FALSE);
    }

    /* Determine if the ojbect has already been created.  */
    if (_txm_module_manager_created_object_check(module_instance, (void *) object_ptr) == TX_FALSE)
    {

        /* Object has already been created, which is invalid.  */
        return(TX_FALSE);
    }

    /* Everything is okay with the object, return TX_TRUE.  */
    return(TX_TRUE);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_param_check_object_for_use      PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, RTOSX                                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks to make sure the object pointer is valid.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Requesting module instance pointer*/
/*    object_ptr                        Address of object memory area     */
/*    ojbect_size                       Size of object memory area        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           Valid object pointer              */
/*    TX_FALSE                          Invalid object pointer            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    txm_module_manager_*              Module manager functions          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2025      William E. Lamie        Initial Version 6.4.3         */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_param_check_object_for_use(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size)
{

    /* Determine if the object pointer is NULL.  */
    if ((void *) object_ptr == TX_NULL)
    {

        /* Object pointer is NULL, which is invalid.  */
        return(TX_FALSE);
    }

    /* Determine if the object is outside the calling module.  */
    if (TXM_MODULE_MANAGER_ENSURE_OUTSIDE_MODULE(module_instance, object_ptr, object_size) == TX_FALSE)
    {

        /* Object pointer is not outside the module, which is invalid.  */
        return(TX_FALSE);
    }

    /* Being outside the module does not make an address an object.  The manager's
       object pool is outside every module, so an address shifted into the interior of
       one of this module's own privileged allocations satisfies the test above while
       denoting no object at all, and the bytes the shifted address then presents as a
       control block are bytes the module chose through ordinary create and set
       services.  An address in the object pool is therefore only usable if it is the
       exact address the manager handed out for an allocation of this size.

       This function is given a size rather than a type, so it can go no further than
       that: it cannot establish which type of object it is, and it cannot authenticate
       an application-owned object outside the pool.  Dispatchers in this repository use
       _txm_module_manager_param_check_typed_object_for_use, which does both.  */
    if ((_txm_module_manager_object_pool_created == TX_TRUE) &&
        (object_ptr >= (ALIGN_TYPE) _txm_module_manager_object_pool.tx_byte_pool_start) &&
        (object_ptr < (ALIGN_TYPE) (_txm_module_manager_object_pool.tx_byte_pool_start + _txm_module_manager_object_pool.tx_byte_pool_size)))
    {

        if (_txm_module_manager_allocated_object_check(module_instance, object_ptr, object_size) == TX_FALSE)
        {

            /* An address in the object pool that is not the exact start of one of this
               module's allocations, which is invalid.  */
            return(TX_FALSE);
        }
    }

    /* Define application-specific object memory check.  */
#ifdef TXM_MODULE_MANGER_APPLICATION_VALID_OBJECT_MEMORY_CHECK

    /* Bring in the application-spefic objeft memory check, defined by the user.  */
    TXM_MODULE_MANGER_APPLICATION_VALID_OBJECT_MEMORY_CHECK
#endif /* TXM_MODULE_MANGER_APPLICATION_VALID_OBJECT_MEMORY_ENABLE  */

    /* Everything is okay with the object, return TX_TRUE.  */
    return(TX_TRUE);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_type_size_get            PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the size of the control block belonging to a  */
/*    module object type. It is the size a service of that type is        */
/*    entitled to dereference, and the size the manager recorded when it  */
/*    allocated object memory for that type.                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_type                       Module object type                */
/*    object_size                       Destination for the control block */
/*                                        size of that type               */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           Known object type                 */
/*    TX_FALSE                          Unknown object type               */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_param_check_typed_object_for_use                */
/*                                          Module object authentication  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_object_type_size_get(UINT object_type, ULONG *object_size)
{

UINT    status;


    /* Assume the type is one this manager knows.  */
    status =  TX_TRUE;

    switch (object_type)
    {

    case TXM_BLOCK_POOL_OBJECT:

        *object_size =  (ULONG) sizeof(TX_BLOCK_POOL);
        break;

    case TXM_BYTE_POOL_OBJECT:

        *object_size =  (ULONG) sizeof(TX_BYTE_POOL);
        break;

    case TXM_EVENT_FLAGS_OBJECT:

        *object_size =  (ULONG) sizeof(TX_EVENT_FLAGS_GROUP);
        break;

    case TXM_MUTEX_OBJECT:

        *object_size =  (ULONG) sizeof(TX_MUTEX);
        break;

    case TXM_QUEUE_OBJECT:

        *object_size =  (ULONG) sizeof(TX_QUEUE);
        break;

    case TXM_SEMAPHORE_OBJECT:

        *object_size =  (ULONG) sizeof(TX_SEMAPHORE);
        break;

    case TXM_THREAD_OBJECT:

        *object_size =  (ULONG) sizeof(TX_THREAD);
        break;

    case TXM_TIMER_OBJECT:

        *object_size =  (ULONG) sizeof(TX_TIMER);
        break;

    default:

        /* Not a type the manager authenticates. Report it rather than guessing a
           size, so that a caller cannot be given a range to validate that has no
           relationship to the object the service will dereference.  */
        *object_size =  ((ULONG) 0);
        status =        TX_FALSE;
        break;
    }

    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_allocated_object_find           PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the manager's private header for the object   */
/*    allocation an address names, or TX_NULL when the address names      */
/*    none of the specified module's allocations.  The size the           */
/*    allocation was made for is reported to callers that ask for it.     */
/*                                                                        */
/*    The allocation list is the manager's own record, built as it hands  */
/*    object memory out, and it is searched rather than reached through   */
/*    a header taken from in front of the caller's address.  That order   */
/*    is what makes the answer both correct and safe.                     */
/*                                                                        */
/*    Correct, because the address immediately after a header is the      */
/*    only address in an allocation that the manager ever gave out, so    */
/*    an address chosen inside an allocation is distinguishable from the  */
/*    start of one.  Reading a header cannot make that distinction: the   */
/*    bytes in front of an interior address are part of the object, and   */
/*    a module places values there through ordinary services.             */
/*                                                                        */
/*    Safe, because the caller's address is only ever compared, never     */
/*    dereferenced.  An address that belongs to no allocation of this     */
/*    module -- an object the application owns, an object another module  */
/*    owns, or an address nowhere near the object pool -- is rejected     */
/*    without a privileged read of the words in front of it.              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Requesting module instance pointer*/
/*    object_ptr                        Address of object memory area     */
/*    object_size_ptr                   Address to return the size the    */
/*                                        allocation was made for, or     */
/*                                        TX_NULL to not return it        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    allocation                        The allocation's private header,  */
/*                                        or TX_NULL when the address     */
/*                                        names none of this module's     */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_allocated_object_check                          */
/*                                          Module object ownership check */
/*    _txm_module_manager_object_deallocate Module object deallocation    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
TXM_MODULE_ALLOCATED_OBJECT  *_txm_module_manager_allocated_object_find(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG *object_size_ptr)
{

TX_INTERRUPT_SAVE_AREA

TXM_MODULE_ALLOCATED_OBJECT     *allocated_object_ptr;
TXM_MODULE_ALLOCATED_OBJECT     *found_object_ptr;
ULONG                           objects_examined;


    /* Assume the address names none of this module's allocations.  */
    found_object_ptr =  TX_NULL;

    /* Determine if there is a module whose allocations can be searched.  A request
       that does not come from a module owns no allocation.  */
    if (module_instance == TX_NULL)
    {

        /* Nothing to search, and nothing to report.  */
        return(TX_NULL);
    }

    /* Disable interrupts.  The allocation list is maintained by threads holding the
       manager protection mutex, so a scan that runs to completion with interrupts
       disabled cannot observe it being changed, and unlike taking the mutex it adds
       no blocking point and no priority inversion to a kernel request.  */
    TX_DISABLE

    allocated_object_ptr =  module_instance -> txm_module_instance_object_list_head;
    objects_examined =      ((ULONG) 0);

    /* Loop through the objects allocated to this module.  The loop is bounded by the
       count the manager maintains alongside the list, so the cost of one search is
       bounded by the number of objects this module has allocated, and a list whose
       links have been damaged cannot make the scan run on.  */
    while ((objects_examined < module_instance -> txm_module_instance_object_list_count) &&
           (allocated_object_ptr != TX_NULL))
    {

        /* The address the module was given is the one immediately after the private
           header, so that is the only address in this allocation that names it.  */
        if (((ALIGN_TYPE) (allocated_object_ptr + 1)) == object_ptr)
        {

            /* Found it.  An address matches at most one allocation, so there is
               nothing further to look at.  */
            found_object_ptr =  allocated_object_ptr;

            /* Report the size this allocation was made for, if the caller asked for
               it.  It is read here, inside the scan, so that the answer cannot be
               taken from a header that stopped being one after the search.  */
            if (object_size_ptr != TX_NULL)
            {

                *object_size_ptr =  allocated_object_ptr -> txm_module_object_size;
            }

            break;
        }

        /* Move to the next allocated object.  */
        objects_examined++;
        allocated_object_ptr =  allocated_object_ptr -> txm_module_allocated_object_next;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    return(found_object_ptr);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_allocated_object_check          PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function determines whether an address is the exact address    */
/*    the manager handed the specified module for one of its object       */
/*    allocations, and whether that allocation is the size the caller     */
/*    expects.                                                            */
/*                                                                        */
/*    This is the question of ownership, and it is deliberately a         */
/*    separate question from whether an object is there at all.  A        */
/*    module may hold, and legitimately use, a pointer to an object it    */
/*    does not own: the manager hands application-owned objects to        */
/*    modules by name through its object lookup service, and sharing an   */
/*    object is what that service is for.  Destroying one is not sharing  */
/*    it, so the delete services ask this question in addition to the     */
/*    ones that establish that the address denotes a usable object.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Requesting module instance pointer*/
/*    object_ptr                        Address of object memory area     */
/*    object_size                       Expected size of the allocation   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           An exact allocation of that size  */
/*    TX_FALSE                          Anything else                     */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_allocated_object_find                           */
/*                                          Find the module's allocation  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    txm_module_manager_*                  Module manager functions      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_allocated_object_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, ULONG object_size)
{

ULONG   allocated_size;
UINT    status;


    /* Assume the address is not one of this module's allocations.  */
    status =  TX_FALSE;

    /* Initialize the size, so that nothing is read if no allocation is found.  */
    allocated_size =  ((ULONG) 0);

    /* Determine if the address is the exact start of one of this module's
       allocations.  */
    if (_txm_module_manager_allocated_object_find(module_instance, object_ptr, &allocated_size) != TX_NULL)
    {

        /* Is the allocation the size the caller expects?  An allocation made for a
           smaller object does not become a larger one because a service was asked
           to treat it as one.  */
        if (allocated_size == object_size)
        {

            status =  TX_TRUE;
        }
    }

    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_param_check_typed_object_for_use                */
/*                                                        PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function authenticates a kernel object a module has named,     */
/*    before a privileged service is allowed to dereference it. It        */
/*    establishes that the address is the exact address of a live kernel   */
/*    object of the type the service expects.                             */
/*                                                                        */
/*    The address is accepted only if it is on the kernel's created list   */
/*    for that type. That list is the record the create and delete        */
/*    services maintain, so being on it establishes at once that the      */
/*    address is an object start rather than an address inside an object, */
/*    that the object is of this type, and that it has not been deleted.  */
/*    It is also how an object the application created and shared with a  */
/*    module is authenticated, since the manager allocated no such object */
/*    and has no record of its own to consult.                            */
/*                                                                        */
/*    Nothing a module can influence is used to establish a type. In      */
/*    particular the control block ID is not, on its own, evidence of     */
/*    anything: a module can arrange for the value of an ID to appear     */
/*    inside an object it legitimately owns, which is what lets a shifted */
/*    pointer pass an ID test, and distinct object types of equal size    */
/*    exist, so an ID is not even a type at an address known to be an     */
/*    object start. It is checked, after the created list has settled the */
/*    question, because it is the check the _txe_ services make and it is */
/*    compiled away with them under TX_DISABLE_ERROR_CHECKING.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Requesting module instance pointer*/
/*    object_ptr                        Address of object memory area     */
/*    object_type                       Module object type the service     */
/*                                        expects                         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           Authenticated object pointer      */
/*    TX_FALSE                          Invalid object pointer            */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_object_type_size_get                            */
/*                                          Get control block size        */
/*    _txm_module_manager_created_object_type_check                        */
/*                                          Check kernel created list     */
/*    _txm_module_manager_object_id_check   Check control block ID        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    txm_module_manager_*              Module manager functions          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_param_check_typed_object_for_use(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE object_ptr, UINT object_type)
{

ULONG   object_size;
UINT    exact_object;


    /* Determine if the object pointer is NULL.  */
    if ((void *) object_ptr == TX_NULL)
    {

        /* Object pointer is NULL, which is invalid.  */
        return(TX_FALSE);
    }

    /* Pickup the size of the control block this type of service dereferences.  */
    if (_txm_module_manager_object_type_size_get(object_type, &object_size) == TX_FALSE)
    {

        /* Not an object type this manager authenticates, so it cannot be used.  */
        return(TX_FALSE);
    }

    /* Determine if the object is outside the calling module.  A kernel object inside
       the module's own memory is one the module can write behind the kernel's back,
       so it is rejected here as it always has been.  This also rejects a size that
       wraps when added to the address.  */
    if (TXM_MODULE_MANAGER_ENSURE_OUTSIDE_MODULE(module_instance, object_ptr, object_size) == TX_FALSE)
    {

        /* Object pointer is not outside the module, which is invalid.  */
        return(TX_FALSE);
    }

    /* Establish that the address is the exact address of a live object of this type,
       from the kernel's created list for the type.  That list is the record the create
       and delete services maintain, so being on it establishes at once that the
       address is an object start, that the object is of this type, and that it has not
       been deleted.

       It is deliberately the only ground on which an address is accepted.  The
       manager's own allocation list would establish the address and the size of an
       object a module allocated, and would do it by walking a shorter list, but it
       records no type, and the type cannot then be taken from the control block
       itself: distinct object types of equal size exist -- on a 32-bit target a byte
       pool, a mutex and a timer are all the same size -- so a control block whose ID
       has been made to read as one of them would be accepted as that type.  Nothing a
       module can influence is used to establish a type.

       The address is not dereferenced to reach this decision.  An address a module
       named is not read until a kernel record says there is an object there.  */
    exact_object =  _txm_module_manager_created_object_type_check(object_ptr, object_type);

    if (exact_object == TX_FALSE)
    {

        /* The address is not the start of a live object of this type.  */
        return(TX_FALSE);
    }

    /* The address is a live object of the requested type.  Confirm it against the
       control block's own ID, which is the check the _txe_ services would make and
       which is compiled away with them under TX_DISABLE_ERROR_CHECKING.  */
    if (_txm_module_manager_object_id_check(object_ptr, object_type) == TX_FALSE)
    {

        /* The created list and the control block disagree, so the object is not in a
           state the manager is prepared to vouch for.  */
        return(TX_FALSE);
    }

    /* Define application-specific object memory check.  */
#ifdef TXM_MODULE_MANGER_APPLICATION_VALID_OBJECT_MEMORY_CHECK

    /* Bring in the application-spefic objeft memory check, defined by the user.  */
    TXM_MODULE_MANGER_APPLICATION_VALID_OBJECT_MEMORY_CHECK
#endif /* TXM_MODULE_MANGER_APPLICATION_VALID_OBJECT_MEMORY_ENABLE  */

    /* Everything is okay with the object, return TX_TRUE.  */
    return(TX_TRUE);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_created_object_type_check       PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function determines whether an address is the exact address    */
/*    of an object on the kernel's created list for a module object type. */
/*                                                                        */
/*    The created list is the record the create and delete services       */
/*    maintain, so being on it establishes at once that the address is an */
/*    object start rather than an address inside an object, that the      */
/*    object is of this type, and that it has not been deleted. It is     */
/*    also the only record of an object the application created and       */
/*    shared with a module, since the manager allocated no such object.   */
/*                                                                        */
/*    Nothing the module can influence is consulted. In particular the    */
/*    control block ID is not: a module can arrange for the value of an   */
/*    ID to appear inside memory it owns, and distinct object types of    */
/*    equal size exist, so an ID is not evidence that an object is there  */
/*    nor of what type it is.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_ptr                        Address of object memory area     */
/*    object_type                       Module object type                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           A created object of that type     */
/*    TX_FALSE                          Anything else                     */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_live_object_check Module object liveness check  */
/*    _txm_module_manager_param_check_typed_object_for_use                */
/*                                          Module object authentication  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_created_object_type_check(ALIGN_TYPE object_ptr, UINT object_type)
{

TX_INTERRUPT_SAVE_AREA

ULONG                   objects_examined;
ULONG                   objects_created;
ALIGN_TYPE              candidate_ptr;
UINT                    status;
TX_BLOCK_POOL           *block_pool_ptr;
TX_BYTE_POOL            *byte_pool_ptr;
TX_EVENT_FLAGS_GROUP    *event_flags_ptr;
TX_MUTEX                *mutex_ptr;
TX_QUEUE                *queue_ptr;
TX_SEMAPHORE            *semaphore_ptr;
TX_THREAD               *thread_ptr;
TX_TIMER                *timer_ptr;


    /* Assume the address is not on the created list for this type.  */
    status =  TX_FALSE;

    /* Disable interrupts.  The created lists are maintained with interrupts disabled,
       so a scan that runs to completion this way sees a consistent list, and it adds
       no blocking point to a kernel request.  The cost of one check is bounded by the
       number of objects of this type the system has created, which is the price of
       authenticating an object the manager did not allocate; a module using its own
       allocated objects does not reach this function.  */
    TX_DISABLE

    /* Start each list from its head and pick up the count that bounds the walk, so a
       list whose links have been damaged cannot make the scan run on.  */
    switch (object_type)
    {

    case TXM_BLOCK_POOL_OBJECT:

        block_pool_ptr =   _tx_block_pool_created_ptr;
        objects_created =  _tx_block_pool_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (block_pool_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) block_pool_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            block_pool_ptr =  block_pool_ptr -> tx_block_pool_created_next;
        }
        break;

    case TXM_BYTE_POOL_OBJECT:

        byte_pool_ptr =    _tx_byte_pool_created_ptr;
        objects_created =  _tx_byte_pool_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (byte_pool_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) byte_pool_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            byte_pool_ptr =  byte_pool_ptr -> tx_byte_pool_created_next;
        }
        break;

    case TXM_EVENT_FLAGS_OBJECT:

        event_flags_ptr =  _tx_event_flags_created_ptr;
        objects_created =  _tx_event_flags_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (event_flags_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) event_flags_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            event_flags_ptr =  event_flags_ptr -> tx_event_flags_group_created_next;
        }
        break;

    case TXM_MUTEX_OBJECT:

        mutex_ptr =        _tx_mutex_created_ptr;
        objects_created =  _tx_mutex_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (mutex_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) mutex_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            mutex_ptr =  mutex_ptr -> tx_mutex_created_next;
        }
        break;

    case TXM_QUEUE_OBJECT:

        queue_ptr =        _tx_queue_created_ptr;
        objects_created =  _tx_queue_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (queue_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) queue_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            queue_ptr =  queue_ptr -> tx_queue_created_next;
        }
        break;

    case TXM_SEMAPHORE_OBJECT:

        semaphore_ptr =    _tx_semaphore_created_ptr;
        objects_created =  _tx_semaphore_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (semaphore_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) semaphore_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            semaphore_ptr =  semaphore_ptr -> tx_semaphore_created_next;
        }
        break;

    case TXM_THREAD_OBJECT:

        thread_ptr =       _tx_thread_created_ptr;
        objects_created =  _tx_thread_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (thread_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) thread_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            thread_ptr =  thread_ptr -> tx_thread_created_next;
        }
        break;

    case TXM_TIMER_OBJECT:

        timer_ptr =        _tx_timer_created_ptr;
        objects_created =  _tx_timer_created_count;
        objects_examined = ((ULONG) 0);

        while ((objects_examined < objects_created) && (timer_ptr != TX_NULL))
        {
            candidate_ptr =  (ALIGN_TYPE) timer_ptr;
            if (candidate_ptr == object_ptr)
            {
                status =  TX_TRUE;
                break;
            }
            objects_examined++;
            timer_ptr =  timer_ptr -> tx_timer_created_next;
        }
        break;

    default:

        /* Not a type the manager authenticates.  */
        break;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_id_check                 PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function determines whether the control block at an object     */
/*    start carries the ID of a module object type. The kernel writes     */
/*    that ID when it creates an object and clears it when it deletes     */
/*    one, so at an address already established to be an object start the  */
/*    ID reports both the type and whether the object is still created.   */
/*                                                                        */
/*    This check must not be used on its own to decide that an address is */
/*    an object. A module can place the value of an ID inside an object    */
/*    it legitimately owns, so an ID found at an address the manager has   */
/*    not otherwise authenticated proves nothing.                          */
/*                                                                        */
/*    The check matters most where the error checking layer is absent:     */
/*    with TX_DISABLE_ERROR_CHECKING the _txe_ services that would        */
/*    otherwise test the ID are compiled away, and this is then the only  */
/*    ID test between a module and a privileged dereference.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_ptr                        Address of an object start        */
/*    object_type                       Module object type                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           A created object of that type     */
/*    TX_FALSE                          Anything else                     */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_param_check_typed_object_for_use                */
/*                                          Module object authentication  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_object_id_check(ALIGN_TYPE object_ptr, UINT object_type)
{

TX_INTERRUPT_SAVE_AREA

ULONG   object_id;
ULONG   expected_id;
UINT    status;


    /* Read the ID through a pointer to the type the caller named, so that the field
       read is the one that type declares rather than an assumed offset.  */
    TX_DISABLE

    switch (object_type)
    {

    case TXM_BLOCK_POOL_OBJECT:

        object_id =    ((TX_BLOCK_POOL *) object_ptr) -> tx_block_pool_id;
        expected_id =  TX_BLOCK_POOL_ID;
        break;

    case TXM_BYTE_POOL_OBJECT:

        object_id =    ((TX_BYTE_POOL *) object_ptr) -> tx_byte_pool_id;
        expected_id =  TX_BYTE_POOL_ID;
        break;

    case TXM_EVENT_FLAGS_OBJECT:

        object_id =    ((TX_EVENT_FLAGS_GROUP *) object_ptr) -> tx_event_flags_group_id;
        expected_id =  TX_EVENT_FLAGS_ID;
        break;

    case TXM_MUTEX_OBJECT:

        object_id =    ((TX_MUTEX *) object_ptr) -> tx_mutex_id;
        expected_id =  TX_MUTEX_ID;
        break;

    case TXM_QUEUE_OBJECT:

        object_id =    ((TX_QUEUE *) object_ptr) -> tx_queue_id;
        expected_id =  TX_QUEUE_ID;
        break;

    case TXM_SEMAPHORE_OBJECT:

        object_id =    ((TX_SEMAPHORE *) object_ptr) -> tx_semaphore_id;
        expected_id =  TX_SEMAPHORE_ID;
        break;

    case TXM_THREAD_OBJECT:

        object_id =    ((TX_THREAD *) object_ptr) -> tx_thread_id;
        expected_id =  TX_THREAD_ID;
        break;

    case TXM_TIMER_OBJECT:

        object_id =    ((TX_TIMER *) object_ptr) -> tx_timer_id;
        expected_id =  TX_TIMER_ID;
        break;

    default:

        /* Not a type the manager authenticates.  Choose values that cannot match.  */
        object_id =    TX_CLEAR_ID;
        expected_id =  ~((ULONG) TX_CLEAR_ID);
        break;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    if (object_id == expected_id)
    {

        status =  TX_TRUE;
    }
    else
    {

        status =  TX_FALSE;
    }

    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_live_object_check               PORTABLE C      */
/*                                                           6.4.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function determines whether an address is the exact address    */
/*    of a live kernel object of any type the Module Manager knows.       */
/*                                                                        */
/*    It answers the question object deallocation has to ask before it    */
/*    gives memory back: is the kernel still going to use these bytes as  */
/*    a control block. Deallocation is told an address and nothing else,  */
/*    so unlike the checks a typed service makes it cannot be given the   */
/*    type to look for, and every type has to be looked for in turn.      */
/*                                                                        */
/*    The eight created lists are searched whatever the size of the       */
/*    allocation at the address. Skipping a type whose control block is   */
/*    larger than the allocation would be sound only if every object had  */
/*    been created through a size-checked path, and a module running      */
/*    without memory protection creates objects without one.              */
/*                                                                        */
/*    Each list is searched in its own interrupts-disabled window rather  */
/*    than all of them in one, so the longest window is bounded by the    */
/*    number of objects of a single type. The whole search costs one pass */
/*    over the objects the system has created, and is paid once per       */
/*    object deallocation.                                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_ptr                        Address of object memory area     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TRUE                           A live kernel object is there     */
/*    TX_FALSE                          Anything else                     */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_created_object_type_check                       */
/*                                          Check one kernel created list */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_object_deallocate Deallocate object memory      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-2026      Eclipse ThreadX         Initial Version 6.4.3         */
/*                    contributors                                        */
/*                                                                        */
/**************************************************************************/
UINT    _txm_module_manager_live_object_check(ALIGN_TYPE object_ptr)
{

UINT    object_type;
UINT    status;


    /* Assume no live object is there.  */
    status =  TX_FALSE;

    /* The eight module object types that have a kernel created list occupy a
       contiguous range, so each is asked in turn by walking the range. A value in
       the range that the per-type check does not recognise answers TX_FALSE, so a
       type the manager stops knowing does not silently pass this search.  */
    for (object_type = ((UINT) TXM_BLOCK_POOL_OBJECT); object_type <= ((UINT) TXM_TIMER_OBJECT); object_type++)
    {

        if (_txm_module_manager_created_object_type_check(object_ptr, object_type) == TX_TRUE)
        {

            /* An address is the start of at most one object, so there is nothing
               further to look for.  */
            status =  TX_TRUE;
            break;
        }
    }

    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_util_code_allocation_size_and_alignment_get     */
/*                                                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the required alignment and allocation size    */
/*    for a module's code area.                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_preamble                   Preamble of module to return code */
/*                                        values for                      */
/*    code_alignment_dest               Address to return code alignment  */
/*    code_allocation_size_desk         Address to return code allocation */
/*                                        size                            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Success if no math overflow       */
/*                                        occurred during calculation     */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    txm_module_manager_*_load             Module load functions         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_util_code_allocation_size_and_alignment_get(TXM_MODULE_PREAMBLE *module_preamble,
                                                                      ULONG *code_alignment_dest, ULONG *code_allocation_size_dest)
{

ULONG   code_size;
ULONG   code_alignment;
ULONG   data_size_ignored;
ULONG   data_alignment_ignored;


    /* Pickup the module code size.  */
    code_size =  module_preamble -> txm_module_preamble_code_size;

    /* Adjust the size of the module elements to be aligned to the default alignment.  */
    TXM_MODULE_MANAGER_UTIL_MATH_ADD_ULONG(code_size, TXM_MODULE_CODE_ALIGNMENT, code_size);
    code_size =  ((code_size - 1)/TXM_MODULE_CODE_ALIGNMENT) * TXM_MODULE_CODE_ALIGNMENT;

    /* Setup the default code and data alignments.  */
    code_alignment =  (ULONG) TXM_MODULE_CODE_ALIGNMENT;

    /* Get the port-specific alignment for the code size. Note we only want code so we pass 'null' values for data.  */
    data_size_ignored = 1;
    data_alignment_ignored = 1;
    TXM_MODULE_MANAGER_ALIGNMENT_ADJUST(module_preamble, code_size, code_alignment, data_size_ignored, data_alignment_ignored)

    /* Calculate the code memory allocation size.  */
    TXM_MODULE_MANAGER_UTIL_MATH_ADD_ULONG(code_size, code_alignment, *code_allocation_size_dest);

    /* Write the alignment result into the caller's destination address.  */
    *code_alignment_dest =  code_alignment;

    /* Return success.  */
    return(TX_SUCCESS);
}


