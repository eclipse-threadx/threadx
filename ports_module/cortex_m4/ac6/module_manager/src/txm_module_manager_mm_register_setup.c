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
/*    _txm_module_manager_region_size_get               Cortex-M4/MPU     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts the region size in bytes to the block size   */
/*    for the Cortex-M4 MPU specification.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block_size                        Size of the block in bytes        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    MPU size specification                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_mm_register_setup                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
ULONG  _txm_module_manager_region_size_get(ULONG block_size)
{

ULONG   return_value;

    /* Process relative to the input block size.  */
    if (block_size == 32)
    {
        return_value =  0x04;
    }
    else if (block_size == 64)
    {
        return_value =  0x05;
    }
    else if (block_size == 128)
    {
        return_value =  0x06;
    }
    else if (block_size == 256)
    {
        return_value =  0x07;
    }
    else if (block_size == 512)
    {
        return_value =  0x08;
    }
    else if (block_size == 1024)
    {
        return_value =  0x09;
    }
    else if (block_size == 2048)
    {
        return_value =  0x0A;
    }
    else if (block_size == 4096)
    {
        return_value =  0x0B;
    }
    else if (block_size == 8192)
    {
        return_value =  0x0C;
    }
    else if (block_size == 16384)
    {
        return_value =  0x0D;
    }
    else if (block_size == 32768)
    {
        return_value =  0x0E;
    }
    else if (block_size == 65536)
    {
        return_value =  0x0F;
    }
    else if (block_size == 131072)
    {
        return_value =  0x10;
    }
    else if (block_size == 262144)
    {
        return_value =  0x11;
    }
    else if (block_size == 524288)
    {
        return_value =  0x12;
    }
    else if (block_size == 1048576)
    {
        return_value =  0x13;
    }
    else if (block_size == 2097152)
    {
        return_value =  0x14;
    }
    else
    {
        /* Max 4MB MPU pages for modules.  */
        return_value =  0x15;
    }

    return(return_value);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_calculate_srd_bits          Cortex-M4/MPU/AC6   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function calculates the SRD bits that need to be set to        */
/*    protect "length" bytes in a block.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block_size                        Size of the block in bytes        */
/*    length                            Actual length in bytes            */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    SRD bits to be OR'ed with region attribute register.                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_mm_register_setup                               */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
ULONG  _txm_module_manager_calculate_srd_bits(ULONG block_size, ULONG length)
{

ULONG   srd_bits = 0;
UINT    srd_bit_index;

    /* length is smaller than block_size, set SRD bits if block_size is 256 or more. */
    if((block_size >= 256) && (length < block_size))
    {
        /* Divide block_size by 8 by shifting right 3. Result is size of subregion.  */
        block_size = block_size >> 3;
        
        /* Set SRD index into attribute register. */
        srd_bit_index = 8;
        
        /* If subregion overlaps length, move to the next subregion. */
        while(length > block_size)
        {
            length = length - block_size;
            srd_bit_index++;
        }
        /* Check for a portion of code remaining. */
        if(length)
        {
            srd_bit_index++;
        }
        
        /* Set unused subregion bits. */
        while(srd_bit_index < 16)
        {
            srd_bits = srd_bits | (0x1 << srd_bit_index);
            srd_bit_index++;
        }
    }
    
    return(srd_bits);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_mm_register_setup             Cortex-M4/MPU     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets up the Cortex-M4 MPU register definitions based  */
/*    on the module's memory characteristics.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Pointer to module instance        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    MPU specifications for module in module_instance                    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _txm_module_manager_region_size_get                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_manager_thread_create                                   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _txm_module_manager_mm_register_setup(TXM_MODULE_INSTANCE *module_instance)
{

ULONG   code_address;
ULONG   code_size;
ULONG   data_address;
ULONG   data_size;
ULONG   start_stop_stack_size;
ULONG   callback_stack_size;
ULONG   block_size;
ULONG   base_address_register;
ULONG   base_attribute_register;
ULONG   region_size;
ULONG   srd_bits = 0;
UINT    mpu_register = 0;
UINT    mpu_table_index;
UINT    i;


    /* Setup the first region for the ThreadX trampoline code.  */
    /* Set base register to user mode entry, which is guaranteed to be at least 32-byte aligned.  */
    base_address_register =  (ULONG) _txm_module_manager_user_mode_entry;
    
    /* Mask address to proper range,  region 0, set Valid bit. */
    base_address_register =  (base_address_register & 0xFFFFFFE0) | mpu_register | 0x10;
    module_instance -> txm_module_instance_mpu_registers[0] =  base_address_register;
    
    /* Attributes: read only, write-back, shareable, size 32 bytes, region enabled. */
    module_instance -> txm_module_instance_mpu_registers[1] =  0x06070009;

    /* Initialize the MPU register.  */
    mpu_register =  1;

    /* Initialize the MPU table index.  */
    mpu_table_index = 2;

    /* Setup values for code area.  */
    code_address =  (ULONG) module_instance -> txm_module_instance_code_start;
    code_size =     module_instance -> txm_module_instance_preamble_ptr -> txm_module_preamble_code_size;
    
    /* Check if shared memory was set up. If so, only 3 entries are available for
       code protection. If not set up, 4 code entries are available.  */
    if(module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_SHARED_MPU_INDEX] == 0)
    {
        /* Determine code block sizes. Minimize the alignment requirement.
           There are 4 MPU code entries available. The following is how the code size
           will be distributed:
           1. 1/4 of the largest power of two that is greater than or equal to code size.
           2. 1/4 of the largest power of two that is greater than or equal to code size.
           3. Largest power of 2 that fits in the remaining space.
           4. Smallest power of 2 that exceeds the remaining space, minimum 32. */
        
        /* Now loop through to setup MPU protection for the code area.  */
        for (i = 0; i < TXM_MODULE_MANAGER_CODE_MPU_ENTRIES; i++)
        {
            /* First two MPU blocks are 1/4 of the largest power of two
               that is greater than or equal to code size.  */
            if (i < 2)
            {
                block_size = _txm_power_of_two_block_size(code_size) >> 2;
            }
            
            /* Third MPU block is the largest power of 2 that fits in the remaining space. */
            else if (i == 2)
            {
                /* Subtract (block_size*2) from code_size to calculate remaining space.  */
                code_size =  code_size - (block_size << 1);
                block_size = _txm_power_of_two_block_size(code_size) >> 1;
            }
            
            /* Last MPU block is the smallest power of 2 that exceeds the remaining space, minimum 32. */
            else
            {
                /* Calculate remaining space.  */
                code_size =  code_size - block_size;
                block_size = _txm_power_of_two_block_size(code_size);
                srd_bits = _txm_module_manager_calculate_srd_bits(block_size, code_size);
            }
            
            /* Build the base address register.  */
            base_address_register =  (code_address & ~(block_size - 1)) | mpu_register | 0x10;
            
            /* Calculate the region size information.  */
            region_size = (_txm_module_manager_region_size_get(block_size) << 1);
            
            /* Build the base attribute register. */
            base_attribute_register =  region_size | srd_bits | 0x06070001;
            
            /* Setup the MPU Base Address Register.  */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index] =  base_address_register;
            
            /* Setup the MPU Base Attribute Register.   */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index+1] =  base_attribute_register;

            /* Adjust the code address.  */
            code_address =  code_address + block_size;
            
            /* Move MPU table index.  */
            mpu_table_index =  mpu_table_index + 2;
            
            /* Increment the MPU register index.  */
            mpu_register++;
        }
    }
    
    /* Only 3 code entries available.  */
    else
    {
        /* Calculate block size, one code entry taken up by shared memory.  */
        block_size = _txm_power_of_two_block_size(code_size / (TXM_MODULE_MANAGER_CODE_MPU_ENTRIES - 1));
        
        /* Calculate the region size information.  */
        region_size = (_txm_module_manager_region_size_get(block_size) << 1);
        
        /* Now loop through to setup MPU protection for the code area.  */
        for (i = 0; i < TXM_MODULE_MANAGER_CODE_MPU_ENTRIES - 1; i++)
        {
            /* Build the base address register.  */
            base_address_register =  code_address & ~(block_size - 1) | mpu_register | 0x10;
            
            /* Check if SRD bits need to be set.  */
            if (code_size < block_size)
            {
                srd_bits = _txm_module_manager_calculate_srd_bits(block_size, code_size);
            }
            
            /* Build the base attribute register. */
            base_attribute_register =  region_size | srd_bits | 0x06070000;
            
            /* Is there still some code?  If so set the region enable bit.  */
            if (code_size)
            {
                /* Set the region enable bit.  */
                base_attribute_register =  base_attribute_register | 0x1;
            }
            /* Setup the MPU Base Address Register.  */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index] =  base_address_register;
            
            /* Setup the MPU Base Attribute Register.   */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index+1] =  base_attribute_register;
            
            /* Adjust the code address.  */
            code_address =  code_address + block_size;
            
            /* Decrement the code size.  */
            if (code_size > block_size)
            {
                code_size =  code_size - block_size;
            }
            else
            {
                code_size =  0;
            }
            
            /* Move MPU table index.  */
            mpu_table_index =  mpu_table_index + 2;
            
            /* Increment the MPU register index.  */
            mpu_register++;
        }
        
        /* Adjust indeces to pass over the shared memory entry.  */
        /* Move MPU table index.  */
        mpu_table_index =  mpu_table_index + 2;
        
        /* Increment the MPU register index.  */
        mpu_register++;
    }
    
    /* Setup values for data area.  */
    data_address =  (ULONG) module_instance -> txm_module_instance_data_start;
    
    /* Adjust the size of the module elements to be aligned to the default alignment. We do this
       so that when we partition the allocated memory, we can simply place these regions right beside
       each other without having to align their pointers. Note this only works when they all have
       the same alignment.  */
    
    data_size =             module_instance -> txm_module_instance_preamble_ptr -> txm_module_preamble_data_size;
    start_stop_stack_size = module_instance -> txm_module_instance_preamble_ptr -> txm_module_preamble_start_stop_stack_size;
    callback_stack_size =   module_instance -> txm_module_instance_preamble_ptr -> txm_module_preamble_callback_stack_size;
    
    data_size =              ((data_size + TXM_MODULE_DATA_ALIGNMENT - 1)/TXM_MODULE_DATA_ALIGNMENT) * TXM_MODULE_DATA_ALIGNMENT;

    start_stop_stack_size =  ((start_stop_stack_size + TXM_MODULE_DATA_ALIGNMENT - 1)/TXM_MODULE_DATA_ALIGNMENT) * TXM_MODULE_DATA_ALIGNMENT;

    callback_stack_size =    ((callback_stack_size + TXM_MODULE_DATA_ALIGNMENT - 1)/TXM_MODULE_DATA_ALIGNMENT) * TXM_MODULE_DATA_ALIGNMENT;

    /* Update the data size to include thread stacks.  */
    data_size = data_size + start_stop_stack_size + callback_stack_size;
    
    block_size = _txm_power_of_two_block_size(data_size / TXM_MODULE_MANAGER_DATA_MPU_ENTRIES);
    
    /* Reset SRD bitfield.  */
    srd_bits = 0;
    
    /* Calculate the region size information.  */
    region_size =   (_txm_module_manager_region_size_get(block_size) << 1);
    
    /* Now loop through to setup MPU protection for the data area.  */
    for (i = 0; i < TXM_MODULE_MANAGER_DATA_MPU_ENTRIES; i++)
    {
        /* Build the base address register.  */
        base_address_register =  (data_address & ~(block_size - 1)) | mpu_register | 0x10;
        
        /* Check if SRD bits need to be set.  */
        if (data_size < block_size)
        {
            srd_bits = _txm_module_manager_calculate_srd_bits(block_size, data_size);
        }
        
        /* Build the base attribute register. */
        base_attribute_register =  region_size | srd_bits | 0x13070000;
        
        /* Is there still some data?  If so set the region enable bit.  */
        if (data_size)
        {
            /* Set the region enable bit.  */
            base_attribute_register =  base_attribute_register | 0x1;
        }
        
        /* Setup the MPU Base Address Register.  */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index] =  base_address_register;
        
        /* Setup the MPU Base Attribute Register.   */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index+1] =  base_attribute_register;

        /* Adjust the data address.  */
        data_address =  data_address + block_size;
        
        /* Decrement the data size.  */
        if (data_size > block_size)
        {
            data_size =  data_size - block_size;
        }
        else
        {
            data_size =  0;
        }
        
        /* Move MPU table index.  */
        mpu_table_index =  mpu_table_index + 2;
        
        /* Increment the MPU register index.  */
        mpu_register++;
    }
}
