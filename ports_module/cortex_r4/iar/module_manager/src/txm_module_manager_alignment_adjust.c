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
/*    _txm_power_of_two_block_size                    Cortex-R4/MPU/IAR   */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function calculates a power of two size at or immediately above*/ 
/*    the input size and returns it to the caller.                        */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    size                              Block size                        */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    calculated size                   Rounded up to power of two        */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _txm_module_manager_alignment_adjust  Adjust alignment for Cortex-R */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
ULONG  _txm_power_of_two_block_size(ULONG size)
{
    /* Check for 0 size. */
    if(size == 0)
        return 0;
    
    /* Minimum MPU block size is 32. */
    if(size <= 32)
        return 32;
    
    /* Bit twiddling trick to round to next high power of 2
       (if original size is power of 2, it will return original size. Perfect!) */
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;
    
    /* Return a power of 2 size at or above the input size.  */
    return(size);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txm_module_manager_alignment_adjust            Cortex-R4/MPU/IAR   */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function adjusts the alignment and size of the code and data   */ 
/*    section for a given module implementation.                          */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    module_preamble                   Pointer to module preamble        */ 
/*    code_size                         Size of the code area (updated)   */ 
/*    code_alignment                    Code area alignment (updated)     */ 
/*    data_size                         Size of data area (updated)       */ 
/*    data_alignment                    Data area alignment (updated)     */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _txm_power_of_two_block_size      Calculate power of two size       */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Initial thread stack frame                                          */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _txm_module_manager_alignment_adjust(TXM_MODULE_PREAMBLE *module_preamble, 
                                           ULONG *code_size, 
                                           ULONG *code_alignment, 
                                           ULONG *data_size, 
                                           ULONG *data_alignment)
{

ULONG   local_code_size;
ULONG   local_code_alignment;
ULONG   local_data_size;
ULONG   local_data_alignment;
ULONG   code_size_accum;
ULONG   data_size_accum;

    /* Copy the input parameters into local variables for ease of use.  */
    local_code_size =       *code_size;
    local_code_alignment =  *code_alignment;
    local_data_size =       *data_size;
    local_data_alignment =  *data_alignment;

    /* Determine code block sizes. Minimize the alignment requirement.
       There are 4 MPU code entries available. The following is how the code size
       will be distributed:
       1. 1/4 of the largest power of two that is greater than or equal to code size.
       2. 1/4 of the largest power of two that is greater than or equal to code size.
       3. Largest power of 2 that fits in the remaining space.
       4. Smallest power of 2 that exceeds the remaining space, minimum 32.  */
    local_code_alignment =  _txm_power_of_two_block_size(local_code_size) >> 2;
    code_size_accum =  local_code_alignment + local_code_alignment;
    code_size_accum =  code_size_accum + (_txm_power_of_two_block_size(local_code_size - code_size_accum) >> 1);
    code_size_accum =  code_size_accum + _txm_power_of_two_block_size(local_code_size - code_size_accum);
    local_code_size =  code_size_accum;
    
    /* Determine data block sizes. Minimize the alignment requirement.
       There are 4 MPU data entries available. The following is how the data size
       will be distributed:
       1. 1/4 of the largest power of two that is greater than or equal to data size.
       2. 1/4 of the largest power of two that is greater than or equal to data size.
       3. Largest power of 2 that fits in the remaining space.
       4. Smallest power of 2 that exceeds the remaining space, minimum 32.  */
    local_data_alignment =  _txm_power_of_two_block_size(local_data_size) >> 2;
    data_size_accum =  local_data_alignment + local_data_alignment;
    data_size_accum =  data_size_accum + (_txm_power_of_two_block_size(local_data_size - data_size_accum) >> 1);
    data_size_accum =  data_size_accum + _txm_power_of_two_block_size(local_data_size - data_size_accum);
    local_data_size =  data_size_accum;
    
    /* Return all the information to the caller.  */
    *code_size =        local_code_size; 
    *code_alignment =   local_code_alignment;
    *data_size =        local_data_size;
    *data_alignment =   local_data_alignment;
}



