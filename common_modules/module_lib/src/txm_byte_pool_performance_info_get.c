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
/*    _tx_byte_pool_performance_info_get                  PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function retrieves performance information from the specified  */ 
/*    byte pool.                                                          */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    pool_ptr                          Pointer to byte pool control block*/ 
/*    allocates                         Destination for number of         */ 
/*                                        allocates on this pool          */ 
/*    releases                          Destination for number of         */ 
/*                                        releases on this pool           */ 
/*    fragments_searched                Destination for number of         */ 
/*                                        fragments searched during       */ 
/*                                        allocation                      */ 
/*    merges                            Destination for number of adjacent*/ 
/*                                        free fragments merged           */ 
/*    splits                            Destination for number of         */ 
/*                                        fragments split during          */ 
/*                                        allocation                      */ 
/*    suspensions                       Destination for number of         */ 
/*                                        suspensions on this pool        */
/*    timeouts                          Destination for number of timeouts*/ 
/*                                        on this byte pool               */ 
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
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT _tx_byte_pool_performance_info_get(TX_BYTE_POOL *pool_ptr, ULONG *allocates, ULONG *releases, ULONG *fragments_searched, ULONG *merges, ULONG *splits, ULONG *suspensions, ULONG *timeouts)
{

UINT return_value;
ALIGN_TYPE extra_parameters[6];

    extra_parameters[0] = (ALIGN_TYPE) releases;
    extra_parameters[1] = (ALIGN_TYPE) fragments_searched;
    extra_parameters[2] = (ALIGN_TYPE) merges;
    extra_parameters[3] = (ALIGN_TYPE) splits;
    extra_parameters[4] = (ALIGN_TYPE) suspensions;
    extra_parameters[5] = (ALIGN_TYPE) timeouts;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_BYTE_POOL_PERFORMANCE_INFO_GET_CALL, (ALIGN_TYPE) pool_ptr, (ALIGN_TYPE) allocates, (ALIGN_TYPE) extra_parameters);

    /* Return value to the caller.  */
    return(return_value);
}
