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
;/**   Initialize                                                          */
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
;#include "tx_initialize.h"
;#include "tx_thread.h"
;#include "tx_timer.h"
;
;
;
;
    IMPORT      _tx_thread_system_stack_ptr
    IMPORT      _tx_initialize_unused_memory
    IMPORT      _tx_version_id
    IMPORT      _tx_build_options
    IMPORT      ||Image$$SHARED_DATA$$ZI$$Limit||
;
;
        AREA ||.text||, CODE, READONLY
;/**************************************************************************/ 
;/*                                                                        */ 
;/*  FUNCTION                                               RELEASE        */ 
;/*                                                                        */ 
;/*    _tx_initialize_low_level                         SMP/Cortex-A9/AC5  */ 
;/*                                                            6.1         */ 
;/*  AUTHOR                                                                */ 
;/*                                                                        */ 
;/*    William E. Lamie, Microsoft Corporation                             */ 
;/*                                                                        */ 
;/*  DESCRIPTION                                                           */ 
;/*                                                                        */ 
;/*    This function is responsible for any low-level processor            */ 
;/*    initialization, including setting up interrupt vectors, setting     */ 
;/*    up a periodic timer interrupt source, saving the system stack       */ 
;/*    pointer for use in ISR processing later, and finding the first      */ 
;/*    available RAM memory address for tx_application_define.             */ 
;/*                                                                        */ 
;/*  INPUT                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  OUTPUT                                                                */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  CALLS                                                                 */ 
;/*                                                                        */ 
;/*    None                                                                */ 
;/*                                                                        */ 
;/*  CALLED BY                                                             */ 
;/*                                                                        */ 
;/*    _tx_initialize_kernel_enter           ThreadX entry function        */ 
;/*                                                                        */ 
;/*  RELEASE HISTORY                                                       */ 
;/*                                                                        */ 
;/*    DATE              NAME                      DESCRIPTION             */ 
;/*                                                                        */ 
;/*  09-30-2020     William E. Lamie         Initial Version 6.1           */ 
;/*                                                                        */ 
;/**************************************************************************/ 
;VOID   _tx_initialize_low_level(VOID)
;{
    EXPORT  _tx_initialize_low_level
_tx_initialize_low_level
;
;    /* Save the first available memory address.  */
;    _tx_initialize_unused_memory =  (VOID_PTR) (||Image$$SHARED_DATA$$ZI$$Limit||);
;
    LDR     r0, =||Image$$SHARED_DATA$$ZI$$Limit||  ; Get end of non-initialized RAM area
    LDR     r2, =_tx_initialize_unused_memory       ; Pickup unused memory ptr address
    STR     r0, [r2, #0]                            ; Save first free memory address
;
;

;    /* Done, return to caller.  */
;
    IF  {INTER} = {TRUE}
    BX      lr                                  ; Return to caller
    ELSE
    MOV     pc, lr                              ; Return to caller
    ENDIF
;}
;
;    /* Reference build options and version ID to ensure they come in.  */
;
    LDR     r2, =_tx_build_options              ; Pickup build options variable address
    LDR     r0, [r2, #0]                        ; Pickup build options content
    LDR     r2, =_tx_version_id                 ; Pickup version ID variable address
    LDR     r0, [r2, #0]                        ; Pickup version ID content
;
;

    END

