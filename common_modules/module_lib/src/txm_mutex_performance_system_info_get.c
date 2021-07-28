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
/*    _tx_mutex_performance_system_info_get               PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function retrieves system mutex performance information.       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    puts                              Destination for total number of   */
/*                                        mutex puts                      */ 
/*    gets                              Destination for total number of   */ 
/*                                        mutex gets                      */ 
/*    suspensions                       Destination for total number of   */ 
/*                                        mutex suspensions               */ 
/*    timeouts                          Destination for total number of   */ 
/*                                        mutex timeouts                  */ 
/*    inversions                        Destination for total number of   */ 
/*                                        mutex priority inversions       */ 
/*    inheritances                      Destination for total number of   */ 
/*                                        mutex priority inheritances     */ 
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
UINT _tx_mutex_performance_system_info_get(ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances)
{

UINT return_value;
ALIGN_TYPE extra_parameters[4];

    extra_parameters[0] = (ALIGN_TYPE) suspensions;
    extra_parameters[1] = (ALIGN_TYPE) timeouts;
    extra_parameters[2] = (ALIGN_TYPE) inversions;
    extra_parameters[3] = (ALIGN_TYPE) inheritances;

    /* Call module manager dispatcher.  */
    return_value = (UINT) (_txm_module_kernel_call_dispatcher)(TXM_MUTEX_PERFORMANCE_SYSTEM_INFO_GET_CALL, (ALIGN_TYPE) puts, (ALIGN_TYPE) gets, (ALIGN_TYPE) extra_parameters);

    /* Return value to the caller.  */
    return(return_value);
}
