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
/*    _txm_module_manager_memory_load                     PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function allocates memory for module code and and calls        */
/*    _txm_module_manager_internal_load to load the data and prepare the  */
/*    module for execution.                                               */
/*                                                                        */
/*    Deprecated: this function cannot bound the copy of the module       */
/*    image, because it is not given its length. Use                      */
/*    _txm_module_manager_memory_load_ext instead.                        */
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
/*    _tx_byte_allocate                 Allocate data area                */
/*    _txm_module_manager_internal_load Load data and prepare module for  */
/*                                        execution                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_memory_load(TXM_MODULE_INSTANCE *module_instance, CHAR *module_name, VOID *module_location)
{


TXM_MODULE_PREAMBLE     *module_preamble;
ALIGN_TYPE              code_start;
ULONG                   code_size;
ULONG                   code_alignment;
ULONG                   code_allocation_size;
CHAR                    *code_memory_ptr;
UCHAR                   *source_ptr;
UCHAR                   *destination_ptr;
ULONG                   copy_size;
ULONG                   i;
UINT                    status;


    /* Pickup the module's information.  */
    module_preamble =  (TXM_MODULE_PREAMBLE *) module_location;

    /* Pickup the basic module sizes.  */
    code_size =  module_preamble -> txm_module_preamble_code_size;

    /* Check for valid sizes. The instruction area holds the preamble, so it is never smaller.  */
    if (code_size < ((ULONG) sizeof(TXM_MODULE_PREAMBLE)))
    {

        /* Invalid module preamble.  */
        return(TXM_MODULE_INVALID);
    }

    /* Get the amount of the bytes we need to allocate for the module's code as well as the required alignment.  */
    status =  _txm_module_manager_util_code_allocation_size_and_alignment_get(module_preamble, &code_alignment, &code_allocation_size);
    if (status != TX_SUCCESS)
    {

        /* Math overflow error occurred.  */
        return(status);
    }

    /* Allocate code memory for the module.  */
    status =  _tx_byte_allocate(&_txm_module_manager_byte_pool, (VOID **) &code_memory_ptr, code_allocation_size, TX_NO_WAIT);

    /* Determine if the module's code memory allocation was successful.  */
    if (status != TX_SUCCESS)
    {

        /* No memory, return an error.  */
        return(TX_NO_MEMORY);
    }

    /* Copy the module code into memory.  */
    source_ptr =       (UCHAR *)  module_location;
    code_start =       (ALIGN_TYPE) code_memory_ptr;
    code_start =       (code_start + (code_alignment - 1)) & ~(code_alignment - 1);
    destination_ptr =  (UCHAR *) code_start;

    /* Calculate the size.  */
    copy_size =  module_preamble -> txm_module_preamble_code_size;

    /* Loop to copy the code to RAM.  */
    for (i = 0; i < copy_size; i++)
    {

        /* Copy one byte at a time.  */
        *destination_ptr++ =  *source_ptr++;
    }

    /* At this point, the module's instruction area is now in the RAM code area.  */

    /* Now load it in-place.  */
    status =  _txm_module_manager_internal_load(module_instance, module_name, (VOID *) code_start,
                                                code_size, code_memory_ptr, code_allocation_size, ((ULONG) 0));
    if (status != TX_SUCCESS)
    {

        /* Release code memory.  */
        _tx_byte_release(code_memory_ptr);

        /* Return error.  */
        return(status);
    }

    /* Return success.  */
    return(TX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_memory_load_ext                 PORTABLE C      */
/*                                                           6.5.2        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Eclipse ThreadX contributors                                        */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function copies a module into module memory and loads it, as  */
/*    _txm_module_manager_memory_load does, after checking the module     */
/*    image against its length, so the copy never reads past the image.   */
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
/*    _txm_module_manager_memory_load   Copy and load the module          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_memory_load_ext(TXM_MODULE_INSTANCE *module_instance, CHAR *module_name, VOID *module_location, ULONG module_size)
{

UINT    status;


    /* Nothing in the image is read until it is known to hold its preamble and its code.  */
    status =  _txm_module_manager_util_image_size_check(module_location, module_size);
    if (status == TX_SUCCESS)
    {
        status =  _txm_module_manager_memory_load(module_instance, module_name, module_location);
    }

    return(status);
}
