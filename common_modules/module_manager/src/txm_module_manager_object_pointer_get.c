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
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "txm_module.h"
#include "txm_module_manager_util.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_object_pointer_get              PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is deprecated and calls the secure version of this    */
/*    function (_txm_module_manager_object_pointer_get_extended) with the */
/*    maximum possible name length since none was passed.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    object_type                       Type of object, as follows:       */
/*                                                                        */
/*                                          TXM_BLOCK_POOL_OBJECT         */
/*                                          TXM_BYTE_POOL_OBJECT          */
/*                                          TXM_EVENT_FLAGS_OBJECT        */
/*                                          TXM_MUTEX_OBJECT              */
/*                                          TXM_QUEUE_OBJECT              */
/*                                          TXM_SEMAPHORE_OBJECT          */
/*                                          TXM_THREAD_OBJECT             */
/*                                          TXM_TIMER_OBJECT              */
/*    name                              Name to search for                */
/*    object_ptr                        Pointer to the object             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_SUCCESS                        Successful completion             */
/*    TX_PTR_ERROR                      Invalid name or object ptr        */
/*    TX_OPTION_ERROR                   Invalid option type               */
/*    TX_NO_INSTANCE                    Object not found                  */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_object_pointer_get_extended                     */
/*                                      Secure version of this function   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_object_pointer_get(UINT object_type, CHAR *name, VOID **object_ptr)
{

UINT    status;

    /* Call the secure version of this function with the maximum length
       possible since none was passed.  */
    status =  _txm_module_manager_object_pointer_get_extended(object_type, name, TXM_MODULE_MANAGER_UTIL_MAX_VALUE_OF_TYPE_UNSIGNED(UINT), object_ptr);
    return(status);
}
