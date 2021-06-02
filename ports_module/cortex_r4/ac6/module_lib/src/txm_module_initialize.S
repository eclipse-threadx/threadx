;/**************************************************************************/
;/*                                                                        */
;/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
;/*                                                                        */
;/*       This software is licensed under the Microsoft Software License   */
;/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
;/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
;/*       and in the root directory of this software.                      */
;/*                                                                        */
;/**************************************************************************/
;
;
;/**************************************************************************/ 
;/**************************************************************************/ 
;/**                                                                       */ 
;/** ThreadX Component                                                     */ 
;/**                                                                       */ 
;/**   Module                                                              */ 
;/**                                                                       */ 
;/**************************************************************************/ 
;/**************************************************************************/ 
;
;
;#define TX_SOURCE_CODE
;
;
;/* Include necessary system files.  */
;
;#include "tx_api.h"
;#include "tx_thread.h"



    .global     __scatterload

#ifdef TX_THUMB_MODE
    .thumb
#else
    .arm
#endif
    .text
    .eabi_attribute Tag_ABI_align_preserved, 1
	.eabi_attribute Tag_ABI_PCS_RO_data, 1
	.eabi_attribute Tag_ABI_PCS_R9_use,  1
	.eabi_attribute Tag_ABI_PCS_RW_data, 2
    .arm
/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _txm_module_initialize                          Cortex-R4/MPU/ARM   */ 
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
/*    heap_base                             Pointer to bottom of heap     */
/*    heap_top                              Pointer to top of heap        */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    __scatterload                         Initialize C runtime          */
/*    __rt_lib_init                         Initialize heap               */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _txm_module_thread_shell_entry        Start module thread           */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Scott Larson             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
/* VOID   _txm_module_initialize(VOID *heap_base, VOID *heap_top) */
    .global _txm_module_initialize
    .type   _txm_module_initialize, %function

_txm_module_initialize:
    PUSH    {r0-r12,lr}                         // Save dregs and LR
    
    B       __scatterload                       // Call ARM func to initialize variables


    .global __rt_entry
    .type   __rt_entry, %function
__rt_entry:
    POP     {r0-r1}
    BL      __rt_lib_init
    POP     {r2-r12,lr}                         // Restore dregs and LR
    BX      lr                                  // Return to caller


