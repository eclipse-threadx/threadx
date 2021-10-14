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
/**   Initialize                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*
#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
*/

    EXTERN      _tx_thread_system_stack_ptr
    EXTERN      _tx_initialize_unused_memory
    EXTERN      _tx_build_options
    EXTERN      _tx_version_id


/* Define the FREE_MEM segment that will specify where free memory is
   defined.  This must also be located in at the end of other RAM segments
   in the linker control file.  The value of this segment is what is passed
   to tx_application_define.  */

    RSEG    FREE_MEM:DATA
    PUBLIC  __tx_free_memory_start
__tx_free_memory_start
    DS8     8


    SECTION `.text`:CODE:NOROOT(3)
    CODE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_initialize_low_level                         Cortex-A5x-SMP/IAR */
/*                                                            6.1.9       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for any low-level processor            */
/*    initialization, including setting up interrupt vectors, setting     */
/*    up a periodic timer interrupt source, saving the system stack       */
/*    pointer for use in ISR processing later, and finding the first      */
/*    available RAM memory address for tx_application_define.             */
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
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter           ThreadX entry function        */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-15-2021      William E. Lamie        Initial Version 6.1.9         */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_initialize_low_level(VOID)
{ */
    PUBLIC  _tx_initialize_low_level
_tx_initialize_low_level:

    MSR     DAIFSet, 0x3                        // Lockout interrupts


    /* Save the system stack pointer.  */
    /* _tx_thread_system_stack_ptr = (VOID_PTR) (sp);  */

    LDR     x0, =_tx_thread_system_stack_ptr    // Pickup address of system stack ptr
    MOV     x1, sp                              // Pickup SP
    SUB     x1, x1, #15                         //
    AND     x1, x1, #~0xF                       // Get 16-bit alignment
    STR     x1, [x0]                            // Store system stack

    /* Save the first available memory address.  */
    /* _tx_initialize_unused_memory =  (VOID_PTR) Image$$HEAP$$ZI$$Limit;  */

    LDR     x0, =_tx_initialize_unused_memory   // Pickup address of unused memory ptr
    LDR     x1, =__tx_free_memory_start         // Pickup unused memory address
    STR     x1, [x0]                            // Store unused memory address

    /* Done, return to caller.  */

    RET                                         // Return to caller
/* } */
    END
