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
/**   Module                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TXM_MODULE
#include "txm_module.h"
#ifndef TXM_MUTEX_CREATE_CALL_NOT_USED
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txe_mutex_create                                   PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the create mutex function        */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    mutex_ptr                         Pointer to mutex control block    */
/*    name_ptr                          Pointer to mutex name             */
/*    inherit                           Initial mutex count               */
/*    mutex_control_block_size          Size of mutex control block       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_MUTEX_ERROR                    Invalid mutex pointer             */
/*    TX_CALLER_ERROR                   Invalid caller of this function   */
/*    TX_INHERIT_ERROR                  Invalid inherit option            */
/*    status                            Actual completion status          */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_kernel_call_dispatcher                                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Module application code                                             */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020      Scott Larson            Initial Version 6.1           */
/*  01-31-2022      Scott Larson            Modified comments and added   */
/*                                            CALL_NOT_USED option,       */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT _txe_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT mutex_control_block_size)
{

UINT return_value;
ALIGN_TYPE extra_parameters[2];

    extra_parameters[0] = (ALIGN_TYPE) inherit;
    extra_parameters[1] = (ALIGN_TYPE) mutex_control_block_size;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_MUTEX_CREATE_CALL, (ALIGN_TYPE) mutex_ptr, (ALIGN_TYPE) name_ptr, (ALIGN_TYPE) extra_parameters);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
