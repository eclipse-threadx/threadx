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

UINT _tx_thread_interrupt_disable(VOID);

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_restore                      Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for disabling interrupts and returning */
/*    the previous interrupt lockout posture.                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    old_posture                           Old interrupt lockout posture */
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
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT _tx_thread_interrupt_disable (VOID)
{
    UINT old_posture;

#ifdef TX_PORT_USE_BASEPRI

    /* Pickup current interrupt lockout posture.  */
    old_posture = __get_BASEPRI();

    /* Disable interrupts */
    __set_BASEPRI(TX_PORT_MAX_IPL << (8U - __NVIC_PRIO_BITS));

#else

    /* Pickup current interrupt lockout posture.  */
    old_posture = __get_PRIMASK();

    /* Disable interrupts */
    __disable_irq();

#endif

    /* Return old interrupt lockout posture.  */
    return old_posture;
}
