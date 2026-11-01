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
#include "tx_initialize.h"
#include "tx_mutex.h"
#include "tx_thread.h"
#include "tx_byte_pool.h"
#include "txm_module.h"
#include "txm_module_manager_util.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_in_place_load                   PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function ensures the code-related parts of the module preamble */
/*    are valid and calls _txm_module_manager_internal_load to load the   */
/*    data and prepare the module for execution.                          */
/*                                                                        */
/*    Deprecated: this function cannot bound the module image, because    */
/*    it is not given its length. Use _txm_module_manager_in_place_load_ext*/
/*    instead.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Module instance pointer           */
/*    module_name                       Module name pointer               */
/*    module_location                   Module code location              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_internal_load Load data and prepare module for  */
/*                                        execution                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_in_place_load(TXM_MODULE_INSTANCE *module_instance, CHAR *module_name, VOID *module_location)
{

TXM_MODULE_PREAMBLE     *module_preamble;
ULONG                   code_size;
ULONG                   code_alignment;
ULONG                   code_allocation_size_ignored;
UINT                    status;


    /* Pickup the module's information.  */
    module_preamble =  (TXM_MODULE_PREAMBLE *) module_location;

    /* Pickup the basic module sizes.  */
    code_size =  module_preamble -> txm_module_preamble_code_size;

    /* Check for valid sizes.  */
    if (code_size == 0)
    {

        /* Invalid module preamble.  */
        return(TXM_MODULE_INVALID);
    }

    /* Get the amount of the bytes we need to allocate for the module's code
       as well as the required alignment. Note that because this is an in-place
       load, we only want the code alignment so we can check it.  */
    status =  _txm_module_manager_util_code_allocation_size_and_alignment_get(module_preamble, &code_alignment, &code_allocation_size_ignored);
    if (status != TX_SUCCESS)
    {

        /* Math overflow error occurred.  */
        return(status);
    }

    /* Now load the module in-place. The internal load checks the alignment of the module's
       instruction area while it holds the protection mutex.  */
    status =  _txm_module_manager_internal_load(module_instance, module_name, module_location,
                                                code_size, TX_NULL, ((ULONG) 0), code_alignment);

    /* Return status.  */
    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_in_place_load_ext               PORTABLE C      */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function loads a module in place, as                           */
/*    _txm_module_manager_in_place_load does, after checking the module   */
/*    image against its length: the image must hold a complete preamble, */
/*    and the instruction area the preamble declares must not extend      */
/*    past the image.                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Module instance pointer           */
/*    module_name                       Module name pointer               */
/*    module_location                   Module code location              */
/*    module_size                       Length of the module image        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_util_image_size_check                           */
/*                                      Check the image against its length*/
/*    _txm_module_manager_in_place_load Load the module in place          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_in_place_load_ext(TXM_MODULE_INSTANCE *module_instance, CHAR *module_name, VOID *module_location, ULONG module_size)
{

UINT    status;


    /* Nothing in the image is read until it is known to hold its preamble and its code.  */
    status =  _txm_module_manager_util_image_size_check(module_location, module_size);
    if (status == TX_SUCCESS)
    {
        status =  _txm_module_manager_in_place_load(module_instance, module_name, module_location);
    }

    return(status);
}
