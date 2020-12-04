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
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


#include "tx_api.h"
#include "tx_secure_interface.h"

#ifdef TX_PORT_TRUSTZONE_NSC_ENABLE

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_secure_stack_free                      Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function enters the SVC handler to free a secure stack.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Thread control block pointer  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Actual completion status      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    SVC 2                                                               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _tx_thread_secure_stack_free (TX_THREAD * thread_ptr)
{
    (void) thread_ptr;

    UINT return_code;

    /* thread_ptr is used in SVC.  Use inline assembly to ensure it is not clobbered. */
    __asm volatile (
        TX_ENABLE_ASM                  // Enable interrupts for SVC call
        "SVC     %[svc_num]                  \n"
        TX_RESTORE_ASM                 // Restore interrupt state
        "MOV  %[ret], r0                     \n"
        :[ret] "=r" (return_code) :[svc_num] "i" (TX_SVC_NUM_SECURE_FREE) : "memory");

    return return_code;
}

#endif
