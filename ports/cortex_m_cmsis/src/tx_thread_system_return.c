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


#ifdef TX_DISABLE_INLINE

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
#include "tx_timer.h"
#include "bsp_api.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_system_return                          Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is target processor specific.  It is used to transfer */
/*    control from a thread back to the ThreadX system.  Only a           */
/*    minimal context is saved since the compiler assumes temp registers  */
/*    are going to get slicked by a function call anyway.                 */
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
/*    _tx_thread_schedule                   Thread scheduling loop        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX components                                                  */
/*                                                                        */
/**************************************************************************/
VOID _tx_thread_system_return (VOID)
{
    /* Return to real scheduler via PendSV. Note that this routine is often
     * replaced with in-line assembly in tx_port.h to improved performance.  */

    /* Set PENDSVBIT in ICSR */
    SCB->ICSR = (uint32_t) SCB_ICSR_PENDSVSET_Msk;

    /* If a thread is returning, briefly enable and restore interrupts. */
    if (__get_IPSR() == 0)
    {
        UINT int_posture;
#ifdef TX_PORT_USE_BASEPRI
        int_posture = __get_BASEPRI();
        __set_BASEPRI(0U);
        __set_BASEPRI(int_posture);
#else
        int_posture = __get_PRIMASK();
        __enable_irq();
        __set_PRIMASK(int_posture);
#endif
    }
}

#endif                                 /* TX_DISABLE_INLINE */
