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
#ifndef TXM_QUEUE_FRONT_SEND_CALL_NOT_USED
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txe_queue_front_send                               PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the queue send function call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    queue_ptr                         Pointer to queue control block    */
/*    source_ptr                        Pointer to message source         */
/*    wait_option                       Suspension option                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_QUEUE_ERROR                    Invalid queue pointer             */
/*    TX_PTR_ERROR                      Invalid source pointer - NULL     */
/*    TX_WAIT_ERROR                     Invalid wait option - non thread  */
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
UINT _txe_queue_front_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option)
{

UINT return_value;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_QUEUE_FRONT_SEND_CALL, (ALIGN_TYPE) queue_ptr, (ALIGN_TYPE) source_ptr, (ALIGN_TYPE) wait_option);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
