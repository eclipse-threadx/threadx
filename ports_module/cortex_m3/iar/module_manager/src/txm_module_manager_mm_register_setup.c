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
/**   Module Manager                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE

#include "tx_api.h"
#include "txm_module.h"

#ifdef TXM_MODULE_MPU_DEFAULT
const ULONG txm_module_default_mpu_registers[32] =
    {
        TXM_MODULE_MPU_DEFAULT_RBAR_0,
        TXM_MODULE_MPU_DEFAULT_RASR_0,
        TXM_MODULE_MPU_DEFAULT_RBAR_1,
        TXM_MODULE_MPU_DEFAULT_RASR_1,
        TXM_MODULE_MPU_DEFAULT_RBAR_2,
        TXM_MODULE_MPU_DEFAULT_RASR_2,
        TXM_MODULE_MPU_DEFAULT_RBAR_3,
        TXM_MODULE_MPU_DEFAULT_RASR_3,
        TXM_MODULE_MPU_DEFAULT_RBAR_4,
        TXM_MODULE_MPU_DEFAULT_RASR_4,
        TXM_MODULE_MPU_DEFAULT_RBAR_5,
        TXM_MODULE_MPU_DEFAULT_RASR_5,
        TXM_MODULE_MPU_DEFAULT_RBAR_6,
        TXM_MODULE_MPU_DEFAULT_RASR_6,
        TXM_MODULE_MPU_DEFAULT_RBAR_7,
        TXM_MODULE_MPU_DEFAULT_RASR_7,
        TXM_MODULE_MPU_DEFAULT_RBAR_8,
        TXM_MODULE_MPU_DEFAULT_RASR_8,
        TXM_MODULE_MPU_DEFAULT_RBAR_9,
        TXM_MODULE_MPU_DEFAULT_RASR_9,
        TXM_MODULE_MPU_DEFAULT_RBAR_10,
        TXM_MODULE_MPU_DEFAULT_RASR_10,
        TXM_MODULE_MPU_DEFAULT_RBAR_11,
        TXM_MODULE_MPU_DEFAULT_RASR_11,
        TXM_MODULE_MPU_DEFAULT_RBAR_12,
        TXM_MODULE_MPU_DEFAULT_RASR_12,
        TXM_MODULE_MPU_DEFAULT_RBAR_13,
        TXM_MODULE_MPU_DEFAULT_RASR_13,
        TXM_MODULE_MPU_DEFAULT_RBAR_14,
        TXM_MODULE_MPU_DEFAULT_RASR_14,
        TXM_MODULE_MPU_DEFAULT_RBAR_15,
        TXM_MODULE_MPU_DEFAULT_RASR_15
    };
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_region_size_get                 Cortex-M3       */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function converts the region size in bytes to the block size   */
/*    for the Cortex-M3 MPU specification.                                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    block_size                        Size of the block in bytes        */
/*                                        Must be a power of two          */
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
/*  10-15-2021      Scott Larson            Initial Version 6.1.9         */
/*  03-08-2023      Scott Larson            Changed from lookup table to  */
/*                                            calculation and check for   */
/*                                            minumum block size,         */
/*                                            resulting in version 6.2.1  */
/*                                                                        */
/**************************************************************************/
ULONG  _txm_module_manager_region_size_get(ULONG block_size)
{

ULONG   return_value = 5;       /* 5 is the region size for 64 byte block. */

    /* Check if at or below minumum block size. */
    if (block_size <= 32)
    {
        /* Return minimum region size. */
        return 0x04;
    }

    /* Remove some trailing zeros from block_size. */
    block_size = block_size >> 6;

    /* Increment return_value until block_size lsb is set. */
    while((block_size & 1) == 0)
    {
        block_size = block_size >> 1;
        return_value++;
    }

    return(return_value);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_calculate_srd_bits              Cortex-M3       */
/*                                                           6.1.9        */
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
/*  10-15-2021      Scott Larson            Initial Version 6.1.9         */
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
/*    _txm_module_manager_mm_register_setup               Cortex-M3       */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets up the MPU register definitions based on the     */
/*    module's memory characteristics.                                    */
/*                                                                        */
/*    Default MPU layout:                                                 */
/*    Entry     Description                                               */
/*      0       Kernel mode entry                                         */
/*      1       Module code region                                        */
/*      2       Module code region                                        */
/*      3       Module code region                                        */
/*      4       Module code region [optional shared memory region]        */
/*      5       Module data region                                        */
/*      6       Module data region                                        */
/*      7       Module data region                                        */
/*                                                                        */
/*    If TXM_MODULE_MANAGER_16_MPU is defined, there are 16 MPU slots.    */
/*    MPU layout for the Cortex-M7:                                       */
/*    Entry     Description                                               */
/*      0       Kernel mode entry                                         */
/*      1       Module code region                                        */
/*      2       Module code region                                        */
/*      3       Module code region                                        */
/*      4       Module code region                                        */
/*      5       Module data region                                        */
/*      6       Module data region                                        */
/*      7       Module data region                                        */
/*      8       Module data region                                        */
/*      9       Module shared memory region                               */
/*      10      Module shared memory region                               */
/*      11      Module shared memory region                               */
/*      12      User-defined region                                       */
/*      13      User-defined region                                       */
/*      14      User-defined region                                       */
/*      15      User-defined region                                       */
/*                                                                        */
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
/*  10-15-2021      Scott Larson            Initial Version 6.1.9         */
/*  07-29-2022      Scott Larson            Enable user defined regions,  */
/*                                            resulting in version 6.1.12 */
/*  03-08-2023      Scott Larson            Initialize unused MPU region, */
/*                                            fix MPU settings for region */
/*                                            size less than 32 bytes,    */
/*                                            resulting in version 6.2.1  */
/*                                                                        */
/**************************************************************************/
VOID  _txm_module_manager_mm_register_setup(TXM_MODULE_INSTANCE *module_instance)
{
#ifdef TXM_MODULE_MANAGER_16_MPU

ULONG   code_address;
ULONG   code_size;
ULONG   data_address;
ULONG   data_size;
ULONG   start_stop_stack_size;
ULONG   callback_stack_size;
ULONG   block_size;
ULONG   region_size;
ULONG   srd_bits = 0;
UINT    mpu_table_index = 1;
UINT    i;


    /* Setup the first MPU region for kernel mode entry.  */
    /* Set address register to user mode entry function address, which is guaranteed to be at least 32-byte aligned.
       Mask address to proper range, region 0, set Valid bit. */
    module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_MPU_KERNEL_ENTRY_INDEX].txm_module_mpu_region_address = ((ULONG) _txm_module_manager_user_mode_entry & 0xFFFFFFE0) | TXM_MPU_VALID_BIT;
    /* Set the attributes, region size (32 bytes), and enable bit.  */
    module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_MPU_KERNEL_ENTRY_INDEX].txm_module_mpu_region_attribute_size = TXM_MODULE_MPU_CODE_ACCESS_CONTROL | 0x08 | TXM_ENABLE_REGION;
    /* End of kernel mode entry setup.  */

    /* Setup code protection.  */

    /* Pickup code starting address and actual size.  */
    code_address =  (ULONG) module_instance -> txm_module_instance_code_start;
    code_size =     module_instance -> txm_module_instance_preamble_ptr -> txm_module_preamble_code_size;

    /* Determine code block sizes. Minimize the alignment requirement.
       There are 4 MPU code entries available. The following is how the code size
       will be distributed:
       1. 1/4 of the largest power of two that is greater than or equal to code size.
       2. 1/4 of the largest power of two that is greater than or equal to code size.
       3. Largest power of 2 that fits in the remaining space.
       4. Smallest power of 2 that exceeds the remaining space, minimum 32. */

    /* Now loop through to setup MPU protection for the code area.  */
    for (i = 0; i < TXM_MODULE_MANAGER_MPU_CODE_ENTRIES; i++)
    {
        switch(i)
        {
            /* First two MPU blocks are 1/4 of the largest power of two
               that is greater than or equal to code size.  */
            case 0:
            {
                block_size = _txm_power_of_two_block_size(code_size) >> 2;
                break;
            }
            case 2:
            {
                /* Third MPU block is the largest power of 2 that fits within the remaining space. */
                /* Subtract (block_size*2) from code_size to calculate remaining space.  */
                code_size =  code_size - (block_size << 1);
                block_size = _txm_power_of_two_block_size(code_size);

                /* Minimum block size is 32.  */
                if(block_size > 32)
                {
                    /* POW2 function result is divided by two to fit in the remaining space. */
                    block_size = block_size >> 1;
                }
                break;
            }
            case 3:
            {
                /* Last MPU block is the smallest power of 2 that exceeds the remaining space, minimum 32. */
                /* Calculate remaining space.  */
                if(code_size - block_size > code_size)
                {
                    /* Case 2 covered the remaining code size. This region will not be used. */
                    block_size = 0;
                }
                else
                {
                    code_size =  code_size - block_size;
                    block_size = _txm_power_of_two_block_size(code_size);
                    srd_bits = _txm_module_manager_calculate_srd_bits(block_size, code_size);
                }
                break;
            }
            default:
            {
                /* Case 1 is the same as 0 - the block size was already calculated. */
                break;
            }
        }

        /* Build the base address register with address, MPU region, set Valid bit.  */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_address = (code_address & ~(block_size - 1)) | mpu_table_index | TXM_MPU_VALID_BIT;

        /* Only configure attribute register if the block is valid.  */
        if(block_size)
        {
            /* Calculate the region size information.  */
            region_size = _txm_module_manager_region_size_get(block_size);

            /* Build the attribute-size register with permissions, SRD, size, enable.  */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size = TXM_MODULE_MPU_CODE_ACCESS_CONTROL | srd_bits | (region_size << 1) | TXM_ENABLE_REGION;

            /* Adjust the code address.  */
            code_address =  code_address + block_size;
        }
        else
        {
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size = 0;
        }

        /* Increment MPU table index.  */
        mpu_table_index++;
    }
    /* End of code protection.  */

    /* Setup data protection.  */

    /* Reset SRD bitfield.  */
    srd_bits = 0;

    /* Pickup data starting address and actual size.  */
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

    /* Determine data block sizes. Minimize the alignment requirement.
       There are 4 MPU data entries available. The following is how the data size
       will be distributed:
       1. 1/4 of the largest power of two that is greater than or equal to data size.
       2. 1/4 of the largest power of two that is greater than or equal to data size.
       3. Largest power of 2 that fits in the remaining space.
       4. Smallest power of 2 that exceeds the remaining space, minimum 32. */

    /* Now loop through to setup MPU protection for the data area.  */
    for (i = 0; i < TXM_MODULE_MANAGER_MPU_DATA_ENTRIES; i++)
    {
        switch(i)
        {
            /* First two MPU blocks are 1/4 of the largest power of two
               that is greater than or equal to data size.  */
            case 0:
            {
                block_size = _txm_power_of_two_block_size(data_size) >> 2;
                break;
            }
            case 2:
            {
                /* Third MPU block is the largest power of 2 that fits within the remaining space. */
                /* Subtract (block_size*2) from data_size to calculate remaining space.  */
                data_size =  data_size - (block_size << 1);
                block_size = _txm_power_of_two_block_size(data_size);

                /* Minimum block size is 32.  */
                if(block_size > 32)
                {
                    /* POW2 function result is divided by two to fit in the remaining space. */
                    block_size = block_size >> 1;
                }
                break;
            }
            case 3:
            {
                /* Last MPU block is the smallest power of 2 that exceeds the remaining space, minimum 32. */
                /* Calculate remaining space.  */
                if(data_size - block_size > data_size)
                {
                    /* Case 2 covered the remaining data size. This region will not be used. */
                    block_size = 0;
                }
                else
                {
                    data_size =  data_size - block_size;
                    block_size = _txm_power_of_two_block_size(data_size);
                    srd_bits = _txm_module_manager_calculate_srd_bits(block_size, data_size);
                }
                break;
            }
            default:
            {
                /* Case 1 is the same as 0 - the block size was already calculated. */
                break;
            }
        }

        /* Build the base address register with address, MPU region, set Valid bit.  */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_address = (data_address & ~(block_size - 1)) | mpu_table_index | TXM_MPU_VALID_BIT;

        /* Only configure attribute register if the block is valid.  */
        if(block_size)
        {
            /* Calculate the region size information.  */
            region_size = _txm_module_manager_region_size_get(block_size);

            /* Build the attribute-size register with permissions, SRD, size, enable.  */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size = TXM_MODULE_MPU_DATA_ACCESS_CONTROL | srd_bits | (region_size << 1) | TXM_ENABLE_REGION;

            /* Adjust the data address.  */
            data_address =  data_address + block_size;
        }
        else
        {
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size = 0;
        }

        /* Increment MPU table index.  */
        mpu_table_index++;
    }
    /* End of data protection.  */

    /* Setup MPU for the shared regions.  */
    while (mpu_table_index < TXM_MODULE_MANAGER_MPU_USER_REGION_INDEX)
    {
        /* Build the base address register with address, MPU region, set Valid bit.  */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_address = mpu_table_index | TXM_MPU_VALID_BIT;

        /* Increment MPU table index.  */
        mpu_table_index++;
    }

    /* Setup user-defined regions (12-15).  */
    module_instance -> txm_module_instance_mpu_registers[12].txm_module_mpu_region_address          = TXM_MODULE_MPU_USER_DEFINED_RBAR_12;
    module_instance -> txm_module_instance_mpu_registers[12].txm_module_mpu_region_attribute_size   = TXM_MODULE_MPU_USER_DEFINED_RASR_12;
    module_instance -> txm_module_instance_mpu_registers[13].txm_module_mpu_region_address          = TXM_MODULE_MPU_USER_DEFINED_RBAR_13;
    module_instance -> txm_module_instance_mpu_registers[13].txm_module_mpu_region_attribute_size   = TXM_MODULE_MPU_USER_DEFINED_RASR_13;
    module_instance -> txm_module_instance_mpu_registers[14].txm_module_mpu_region_address          = TXM_MODULE_MPU_USER_DEFINED_RBAR_14;
    module_instance -> txm_module_instance_mpu_registers[14].txm_module_mpu_region_attribute_size   = TXM_MODULE_MPU_USER_DEFINED_RASR_14;
    module_instance -> txm_module_instance_mpu_registers[15].txm_module_mpu_region_address          = TXM_MODULE_MPU_USER_DEFINED_RBAR_15;
    module_instance -> txm_module_instance_mpu_registers[15].txm_module_mpu_region_attribute_size   = TXM_MODULE_MPU_USER_DEFINED_RASR_15;

#else   /* TXM_MODULE_MANAGER_16_MPU is not defined, only 8 MPU regions. */

ULONG   code_address;
ULONG   code_size;
ULONG   data_address;
ULONG   data_size;
ULONG   start_stop_stack_size;
ULONG   callback_stack_size;
ULONG   block_size;
ULONG   base_attribute_register;
ULONG   region_size;
ULONG   srd_bits = 0;
UINT    mpu_table_index = 1;
UINT    i;


    /* Setup the first MPU region for kernel mode entry.  */
    /* Set address register to user mode entry function address, which is guaranteed to be at least 32-byte aligned.
       Mask address to proper range, region 0, set Valid bit. */
    module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_MPU_KERNEL_ENTRY_INDEX].txm_module_mpu_region_address = ((ULONG) _txm_module_manager_user_mode_entry & 0xFFFFFFE0) | TXM_MPU_VALID_BIT;
    /* Set the attributes, region size (32 bytes), and enable bit.  */
    module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_MPU_KERNEL_ENTRY_INDEX].txm_module_mpu_region_attribute_size = TXM_MODULE_MPU_CODE_ACCESS_CONTROL | 0x08 | TXM_ENABLE_REGION;
    /* End of kernel mode entry setup.  */

    /* Setup code protection.  */

    /* Pickup code starting address and actual size.  */
    code_address =  (ULONG) module_instance -> txm_module_instance_code_start;
    code_size =     module_instance -> txm_module_instance_preamble_ptr -> txm_module_preamble_code_size;

    /* Check if shared memory was set up. If so, only 3 entries are available for
       code protection. If not set up, 4 code entries are available.  */
    if(module_instance -> txm_module_instance_mpu_registers[TXM_MODULE_MANAGER_SHARED_MPU_REGION].txm_module_mpu_region_address == 0)
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
            switch(i)
            {
                /* First two MPU blocks are 1/4 of the largest power of two
                   that is greater than or equal to code size.  */
                case 0:
                {
                    block_size = _txm_power_of_two_block_size(code_size) >> 2;
                    break;
                }
                case 2:
                {
                    /* Third MPU block is the largest power of 2 that fits within the remaining space. */
                    /* Subtract (block_size*2) from code_size to calculate remaining space.  */
                    code_size =  code_size - (block_size << 1);
                    block_size = _txm_power_of_two_block_size(code_size);

                    /* Minimum block size is 32.  */
                    if(block_size > 32)
                    {
                        /* POW2 function result is divided by two to fit in the remaining space. */
                        block_size = block_size >> 1;
                    }
                    break;
                }
                case 3:
                {
                    /* Last MPU block is the smallest power of 2 that exceeds the remaining space, minimum 32. */
                    /* Calculate remaining space.  */
                    if(code_size - block_size > code_size)
                    {
                        /* Case 2 covered the remaining code size. This region will not be used. */
                        block_size = 0;
                    }
                    else
                    {
                        code_size =  code_size - block_size;
                        block_size = _txm_power_of_two_block_size(code_size);
                        srd_bits = _txm_module_manager_calculate_srd_bits(block_size, code_size);
                    }
                    break;
                }
                default:
                {
                    /* Case 1 is the same as 0 - the block size was already calculated. */
                    break;
                }
            }

            /* Build the base address register with address, MPU region, set Valid bit.  */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_address = (code_address & ~(block_size - 1)) | mpu_table_index | TXM_MPU_VALID_BIT;

            /* Only configure attribute register if the block is valid.  */
            if(block_size)
            {
                /* Calculate the region size information.  */
                region_size = _txm_module_manager_region_size_get(block_size);

                /* Build the attribute-size register with permissions, SRD, size, enable.  */
                module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size = TXM_MODULE_MPU_CODE_ACCESS_CONTROL | srd_bits | (region_size << 1) | TXM_ENABLE_REGION;

                /* Adjust the code address.  */
                code_address =  code_address + block_size;
            }
            else
            {
                module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size = 0;
            }

            /* Increment MPU table index.  */
            mpu_table_index++;
        }
    }

    /* Only 3 code entries available.  */
    else
    {
        /* Calculate block size, one code entry taken up by shared memory.  */
        block_size = _txm_power_of_two_block_size(code_size / (TXM_MODULE_MANAGER_CODE_MPU_ENTRIES - 1));

        /* Calculate the region size and pre-shift it so we don't need to shift it multiple times in the for loop.  */
        region_size = _txm_module_manager_region_size_get(block_size) << 1;

        /* Now loop through to setup MPU protection for the code area.  */
        for (i = 0; i < TXM_MODULE_MANAGER_CODE_MPU_ENTRIES - 1; i++)
        {
            /* Build the base address register with address, MPU region, set Valid bit.  */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_address =  (code_address & ~(block_size - 1)) | mpu_table_index | TXM_MPU_VALID_BIT;

            /* Check if SRD bits need to be set.  */
            if (code_size < block_size)
            {
                srd_bits = _txm_module_manager_calculate_srd_bits(block_size, code_size);
            }

            /* Build the base attribute register. */
            base_attribute_register =  region_size | srd_bits | TXM_MODULE_MPU_CODE_ACCESS_CONTROL;

            /* Is there still some code?  If so set the region enable bit.  */
            if (code_size)
            {
                /* Set the region enable bit.  */
                base_attribute_register |= TXM_ENABLE_REGION;
            }

            /* Setup the MPU Base Attribute Register.   */
            module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size =  base_attribute_register;

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

            /* Increment MPU table index.  */
            mpu_table_index++;
        }

        /* Data protection is already set up so we can simply return here.  */
        return;
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

    /* Calculate the region size and pre-shift it so we don't need to shift it multiple times in the for loop.  */
    region_size = _txm_module_manager_region_size_get(block_size) << 1;

    /* Now loop through to setup MPU protection for the data area.  */
    for (i = 0; i < TXM_MODULE_MANAGER_DATA_MPU_ENTRIES; i++)
    {
        /* Build the base address register with address, MPU region, set Valid bit.  */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_address =  (data_address & ~(block_size - 1)) | mpu_table_index | TXM_MPU_VALID_BIT;

        /* Check if SRD bits need to be set.  */
        if (data_size < block_size)
        {
            srd_bits = _txm_module_manager_calculate_srd_bits(block_size, data_size);
        }

        /* Build the attribute-size register with permissions, SRD, size.  */
        base_attribute_register = region_size | srd_bits | TXM_MODULE_MPU_DATA_ACCESS_CONTROL;

        /* Is there still some data?  If so set the region enable bit.  */
        if (data_size)
        {
            /* Set the region enable bit.  */
            base_attribute_register |= TXM_ENABLE_REGION;
        }

        /* Setup the MPU Base Attribute Register.   */
        module_instance -> txm_module_instance_mpu_registers[mpu_table_index].txm_module_mpu_region_attribute_size =  base_attribute_register;

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

        /* Increment MPU table index.  */
        mpu_table_index++;
    }

#endif
}

#ifdef TXM_MODULE_MANAGER_16_MPU
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_manager_inside_data_check               Cortex-M3       */
/*                                                           6.1.9        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks if the specified object is inside shared       */
/*    memory.                                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    module_instance                   Pointer to module instance        */
/*    obj_ptr                           Pointer to the object             */
/*    obj_size                          Size of the object                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Whether the object is inside the shared memory region.              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Module dispatch check functions                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-15-2021      Scott Larson            Initial Version 6.1.9         */
/*                                                                        */
/**************************************************************************/
UINT _txm_module_manager_inside_data_check(TXM_MODULE_INSTANCE *module_instance, ALIGN_TYPE obj_ptr, UINT obj_size)
{

UINT shared_memory_index;
UINT num_shared_memory_mpu_entries;
ALIGN_TYPE shared_memory_address_start;
ALIGN_TYPE shared_memory_address_end;

    /* Check for overflow. */
    if ((obj_ptr) > ((obj_ptr) + (obj_size)))
    {
        return(TX_FALSE);
    }

    /* Check if the object is inside the module data.  */
    if ((obj_ptr >= (ALIGN_TYPE) module_instance -> txm_module_instance_data_start) &&
        ((obj_ptr + obj_size) <= ((ALIGN_TYPE) module_instance -> txm_module_instance_data_end + 1)))
    {
        return(TX_TRUE);
    }

    /* Check if the object is inside the shared memory.  */
    num_shared_memory_mpu_entries = module_instance -> txm_module_instance_shared_memory_count;
    for (shared_memory_index = 0; shared_memory_index < num_shared_memory_mpu_entries; shared_memory_index++)
    {

        shared_memory_address_start = (ALIGN_TYPE) module_instance -> txm_module_instance_shared_memory_address[shared_memory_index];
        shared_memory_address_end = shared_memory_address_start + module_instance -> txm_module_instance_shared_memory_length[shared_memory_index];

        if ((obj_ptr >= (ALIGN_TYPE) shared_memory_address_start) &&
            ((obj_ptr + obj_size) <= (ALIGN_TYPE) shared_memory_address_end))
        {
            return(TX_TRUE);
        }
    }

    return(TX_FALSE);
}
#endif
