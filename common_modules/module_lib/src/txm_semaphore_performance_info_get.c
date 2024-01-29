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
#ifndef TXM_SEMAPHORE_PERFORMANCE_INFO_GET_CALL_NOT_USED
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_semaphore_performance_info_get                  PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves performance information from the specified  */
/*    semaphore.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    semaphore_ptr                     Pointer to semaphore control block*/
/*    puts                              Destination for the number of     */
/*                                        puts on to this semaphore       */
/*    gets                              Destination for the number of     */
/*                                        gets on this semaphore          */
/*    suspensions                       Destination for the number of     */
/*                                        suspensions on this semaphore   */
/*    timeouts                          Destination for number of timeouts*/
/*                                        on this semaphore               */
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
UINT _tx_semaphore_performance_info_get(TX_SEMAPHORE *semaphore_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts)
{

UINT return_value;
ALIGN_TYPE extra_parameters[3];

    extra_parameters[0] = (ALIGN_TYPE) gets;
    extra_parameters[1] = (ALIGN_TYPE) suspensions;
    extra_parameters[2] = (ALIGN_TYPE) timeouts;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_SEMAPHORE_PERFORMANCE_INFO_GET_CALL, (ALIGN_TYPE) semaphore_ptr, (ALIGN_TYPE) puts, (ALIGN_TYPE) extra_parameters);

    /* Return value to the caller.  */
    return(return_value);
}
#endif
