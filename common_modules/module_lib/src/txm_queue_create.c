/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


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

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txe_queue_create                                   PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function checks for errors in the queue create function call.  */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    queue_ptr                         Pointer to queue control block    */ 
/*    name_ptr                          Pointer to queue name             */ 
/*    message_size                      Size of each queue message        */ 
/*    queue_start                       Starting address of the queue area*/ 
/*    queue_size                        Number of bytes in the queue      */ 
/*    queue_control_block_size          Size of queue control block       */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_QUEUE_ERROR                    Invalid queue pointer             */ 
/*    TX_PTR_ERROR                      Invalid starting address of queue */ 
/*    TX_SIZE_ERROR                     Invalid message queue size        */ 
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
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT _txe_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size, UINT queue_control_block_size)
{

UINT return_value;
ALIGN_TYPE extra_parameters[4];

    extra_parameters[0] = (ALIGN_TYPE) message_size;
    extra_parameters[1] = (ALIGN_TYPE) queue_start;
    extra_parameters[2] = (ALIGN_TYPE) queue_size;
    extra_parameters[3] = (ALIGN_TYPE) queue_control_block_size;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_QUEUE_CREATE_CALL, (ALIGN_TYPE) queue_ptr, (ALIGN_TYPE) name_ptr, (ALIGN_TYPE) extra_parameters);

    /* Return value to the caller.  */
    return(return_value);
}
