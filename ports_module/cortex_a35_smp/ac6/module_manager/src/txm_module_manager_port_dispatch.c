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
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "txm_module.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_port_dispatch                 Cortex-A35/AC6    */
/*                                                           6.1.3        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Andres Mlinar, Microsoft Corporation                                */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function dispatches the module's kernel request based upon the */
/*    ID and parameters specified in the request.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Module pointer                    */
/*    kernel_request                    Module's kernel request           */
/*    param_0                           First parameter                   */
/*    param_1                           Second parameter                  */
/*    param_2                           Third parameter                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    Port-specific ThreadX API Calls                                     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_kernel_dispatch                                 */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2020     Andres Mlinar            Initial Version 6.1.3         */
/*                                                                        */
/**************************************************************************/
ALIGN_TYPE _txm_module_manager_port_dispatch(TXM_MODULE_INSTANCE *module_instance, ULONG kernel_request, ALIGN_TYPE param_0, ALIGN_TYPE param_1, ALIGN_TYPE param_2)
{

ALIGN_TYPE return_value = TX_NOT_AVAILABLE;

    switch(kernel_request)
    {
        default:
        {
            /* Unhandled kernel request, return an error!  */
            break;
        }
    }

    return(return_value);
}
