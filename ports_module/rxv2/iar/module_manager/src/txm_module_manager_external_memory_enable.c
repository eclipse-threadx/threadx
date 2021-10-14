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
#include "tx_mutex.h"
#include "tx_queue.h"
#include "tx_thread.h"
#include "txm_module.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_external_memory_enable           RXv2/IAR       */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates an entry in the MPU table for a shared        */
/*    memory space.                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Module instance pointer           */
/*    start_address                     Start address of memory           */
/*    length                            Length of external memory         */
/*    attributes                        Memory attributes (r/w)           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_mutex_get                     Get protection mutex              */
/*    _tx_mutex_put                     Release protection mutex          */
/*    _txm_power_of_two_block_size      Round length to power of two      */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-15-2021      Scott Larson            Initial Version 6.1.9         */
/*                                                                        */
/**************************************************************************/
UINT  _txm_module_manager_external_memory_enable(TXM_MODULE_INSTANCE *module_instance,
                                                 VOID *start_address,
                                                 ULONG length,
                                                 UINT attributes)
{
ULONG   address;
UINT    attributes_check = 0;
TXM_MODULE_PREAMBLE     *module_preamble;

    /* Determine if the module manager has not been initialized yet.  */
    if (_txm_module_manager_ready != TX_TRUE)
    {
        /* Module manager has not been initialized.  */
        return(TX_NOT_AVAILABLE);
    }

    /* Determine if the module is valid.  */
    if (module_instance == TX_NULL)
    {
        /* Invalid module pointer.  */
        return(TX_PTR_ERROR);
    }

    /* Get module manager protection mutex.  */
    _tx_mutex_get(&_txm_module_manager_mutex, TX_WAIT_FOREVER);

    /* Determine if the module instance is valid.  */
    if (module_instance -> txm_module_instance_id != TXM_MODULE_ID)
    {
        /* Release the protection mutex.  */
        _tx_mutex_put(&_txm_module_manager_mutex);

        /* Invalid module pointer.  */
        return(TX_PTR_ERROR);
    }
    
    /* Determine if the module instance is in the loaded state.  */
    if (module_instance -> txm_module_instance_state != TXM_MODULE_LOADED)
    {
        /* Release the protection mutex.  */
        _tx_mutex_put(&_txm_module_manager_mutex);

        /* Return error if the module is not ready.  */
        return(TX_START_ERROR);
    }
    
    /* Check if preamble ext/shared mem property bit set.  */
    module_preamble = module_instance -> txm_module_instance_preamble_ptr;
    if(!(module_preamble -> txm_module_preamble_property_flags & TXM_MODULE_SHARED_EXTERNAL_MEMORY_ACCESS))
    {
        /* Release the protection mutex.  */
        _tx_mutex_put(&_txm_module_manager_mutex);

        /* Return error if bit not set.  */
        return(TXM_MODULE_INVALID_PROPERTIES);
    }
    
    /* Start address must be 16-byte aligned.  */
    address = (ULONG) start_address;
    if(address != (address & 0xFFFFFFF0))
    {
        /* Release the protection mutex.  */
        _tx_mutex_put(&_txm_module_manager_mutex);
        
        /* Return alignment error.  */
        return(TXM_MODULE_ALIGNMENT_ERROR);
    }
    
    /* Check length.  */
    
    /* At this point, we have a valid address and length.  */
    /* Build region start page register.  */
    module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_SHARED_MPU_INDEX] = address & 0xFFFFFFF0;
    
    /* Check for valid attributes.  */
    if(attributes & TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_EXECUTE)
    {
        attributes_check |= TXM_MODULE_MANAGER_ATTRIBUTE_EXECUTE_MPU_BIT;
    }
    if(attributes & TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_WRITE)
    {
        attributes_check |= TXM_MODULE_MANAGER_ATTRIBUTE_WRITE_MPU_BIT;
    }
    if(attributes & TXM_MODULE_MANAGER_SHARED_ATTRIBUTE_READ)
    {
        attributes_check |= TXM_MODULE_MANAGER_ATTRIBUTE_READ_MPU_BIT;
    }
    
    /* Build region end page register with attributes, OR in the Valid bit.  */
    module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_SHARED_MPU_INDEX+1] = ((address + length - 1) & 0xFFFFFFF0) | attributes_check | 0x01;
    
    /* Save address and length in module.  */
    module_instance -> txm_module_instance_shared_memory_address = address;
    module_instance -> txm_module_instance_shared_memory_length = length;
    
    /* Release the protection mutex.  */
    _tx_mutex_put(&_txm_module_manager_mutex);
    
    /* Return success.  */
    return(TX_SUCCESS);
}
