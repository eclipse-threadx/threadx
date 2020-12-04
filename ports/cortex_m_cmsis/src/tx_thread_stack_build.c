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

#define TX_SOURCE_CODE

/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"

#define TX_ALIGN_8         (0xFFFFFFF8)

#define TX_INITIAL_IPSR    (0x01000000)

#ifdef TX_SINGLE_MODE_SECURE

/* Build initial LR value for secure mode. */
 #define TX_INITIAL_LR     (0xFFFFFFFD)
#else

/* Build initial LR value to return to non-secure PSP. */
 #define TX_INITIAL_LR     (0xFFFFFFBC)
#endif

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_stack_build                            Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function builds a stack frame on the supplied thread's stack.  */
/*    The stack frame results in a fake interrupt return to the supplied  */
/*    function pointer.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread control blk */
/*    function_ptr                          Pointer to return function    */
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
/*    _tx_thread_create                     Create thread service         */
/*                                                                        */
/**************************************************************************/

VOID _tx_thread_stack_build (TX_THREAD * thread_ptr, VOID (* function_ptr)(VOID))
{
    /* Build a fake interrupt frame.  The form of the fake interrupt stack
     * on the Cortex-M look like the following after it is built:
     *
     * Stack Top:
     *             #if __FPU_USED
     *                 LR          Interrupted LR (LR at time of PENDSV)
     *             #endif
     *                 r4          Initial value for r4
     *                 r5          Initial value for r5
     *                 r6          Initial value for r6
     *                 r7          Initial value for r7
     *                 r8          Initial value for r8
     *                 r9          Initial value for r9
     *                 r10 (sl)    Initial value for r10 (sl)
     *                 r11         Initial value for r11
     *                 r0          Initial value for r0    (Hardware stack starts here!!)
     *                 r1          Initial value for r1
     *                 r2          Initial value for r2
     *                 r3          Initial value for r3
     *                 r12         Initial value for r12
     *                 lr          Initial value for lr
     *                 pc          Initial value for pc
     *                 xPSR        Initial value for xPSR
     *
     * Stack Bottom: (higher memory address) */

    /* First, subtract stack frame size (16 4-byte registers). */
    UINT * tx_thread_stack_ptr = (UINT *) thread_ptr->tx_thread_stack_end - 16;

#if __FPU_USED

    /* Allocate another word for interrupted LR. */
    tx_thread_stack_ptr--;
#endif

    /* Align stack frame to an 8 byte address. */
    tx_thread_stack_ptr             = (UINT *) ((UINT) tx_thread_stack_ptr & TX_ALIGN_8);
    thread_ptr->tx_thread_stack_ptr = tx_thread_stack_ptr;

#if __FPU_USED

    /* Default value that will be restored into LR first time thread is restored
     * from context. */
    tx_thread_stack_ptr[0] = TX_INITIAL_LR;
    tx_thread_stack_ptr++;
#endif

    /* Clear the first 13 registers on the exception stack frame (r4-r11, r0-r3, r12). */
    memset(tx_thread_stack_ptr, 0U, 13 * sizeof(UINT));

    /* Poison EXC_RETURN value. Set LR to 0xFFFFFFFF. */
    tx_thread_stack_ptr[13] = UINT32_MAX;

    /* Store initial PC. */
    tx_thread_stack_ptr[14] = (UINT) function_ptr;

    /* Only T-bit needs be set. */
    tx_thread_stack_ptr[15] = TX_INITIAL_IPSR;
}
