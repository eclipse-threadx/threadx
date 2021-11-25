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
/*    _txm_module_manager_setup_mpu_registers              RXv2/IAR       */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets up the RX MPU register definitions based         */
/*    on the module's memory characteristics.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Pointer to module instance        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    MPU specifications for module                                       */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    none                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_thread_create                                   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-15-2021      Scott Larson            Initial Version 6.1.9         */
/*                                                                        */
/**************************************************************************/
VOID  _txm_module_manager_setup_mpu_registers(TXM_MODULE_INSTANCE *module_instance)
{

ULONG   region_size;
ULONG   region_start_page_register;
ULONG   region_end_page_register;

    /* Setup region 0 for the ThreadX trampoline code.  */
    region_start_page_register = (ULONG) _txm_module_manager_user_mode_entry;
    region_start_page_register = region_start_page_register & 0xFFFFFFF0;
    module_instance -> txm_module_instance_mpu_registers[0] = region_start_page_register;
    /* Region 0 End page is 2 pages away (for a total of 3 pages). 
     * Set reading permitted, writing prohibited, execution permitted, enable region. */
    module_instance -> txm_module_instance_mpu_registers[1] = (region_start_page_register + 0x20) | 0x0B;
    
    /* Place the trampoline protection in the MPU registers  */
    RSPAGE0 = module_instance -> txm_module_instance_mpu_registers[0];
    REPAGE0 = module_instance -> txm_module_instance_mpu_registers[1];
    
    /* Setup region 1 for code area.  */
    /* Set reading permitted, writing prohibited, execution permitted, enable region. */
    region_start_page_register =    (ULONG) module_instance -> txm_module_instance_code_start; 
    region_size =                   (ULONG) module_instance -> txm_module_instance_code_size;
    
    region_end_page_register =      (region_start_page_register + region_size - 1) & 0xFFFFFFF0;
    region_start_page_register =    region_start_page_register & 0xFFFFFFF0;
    
    module_instance -> txm_module_instance_mpu_registers[2] = region_start_page_register;
    module_instance -> txm_module_instance_mpu_registers[3] = region_end_page_register | 0x0B;
    
    /* Setup region 2 for data area.  */
    /* Set reading permitted, writing permitted, execution prohibited, enable region. */
    region_start_page_register =    (ULONG) module_instance -> txm_module_instance_data_start; 
    region_size =                   (ULONG) module_instance -> txm_module_instance_data_size;
    
    region_end_page_register =      (region_start_page_register + region_size - 1) & 0xFFFFFFF0;
    region_start_page_register =    region_start_page_register & 0xFFFFFFF0;
    
    module_instance -> txm_module_instance_mpu_registers[4] = region_start_page_register;
    module_instance -> txm_module_instance_mpu_registers[5] = region_end_page_register | 0x0D;
    
}
