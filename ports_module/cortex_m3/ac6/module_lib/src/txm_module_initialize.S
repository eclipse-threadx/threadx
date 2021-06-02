/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
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


    .global     __use_two_region_memory
    .global     __scatterload


    .eabi_attribute Tag_ABI_PCS_RO_data, 1
    .eabi_attribute Tag_ABI_PCS_R9_use,  1
    .eabi_attribute Tag_ABI_PCS_RW_data, 2

    .text
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txm_module_initialize                          Cortex-M4/MPU/AC6   */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Scott Larson, Microsoft Corporation                                 */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the module c runtime.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    __scatterload                         Initialize C runtime          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _txm_module_thread_shell_entry        Start module thread           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Andres Mlinar            Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
// VOID   _txm_module_initialize(VOID)
    .global  _txm_module_initialize
    .thumb_func
_txm_module_initialize:
    PUSH    {r4-r12,lr}                         // Save dregs and LR
    //B       __scatterload                       // Call ARM func to initialize variables

// Override the __rt_exit function.
    .global  __rt_exit
    .thumb_func
__rt_exit:
    POP     {r4-r12,lr}                         // Restore dregs and LR
    BX      lr                                  // Return to caller

#define TXM_MODULE_HEAP_SIZE 512

// returns heap start address in  R0
// returns heap end address in    R2
// does not touch SP, it is already set up before the module runs
    .global __user_setup_stackheap
    .thumb_func
__user_setup_stackheap:
    LDR     r1, _txm_heap                       // load heap offset
    MOV     r2, TXM_MODULE_HEAP_SIZE            // load heap size
    ADD     r2, r2, r0                          // calculate heap end address
    BX      lr

// dummy main function
    .global main
    .thumb_func
main:
    BX      lr

    .align 8
_txm_heap:
    .zero   TXM_MODULE_HEAP_SIZE
