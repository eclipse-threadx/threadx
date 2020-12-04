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

/* #define TX_SOURCE_CODE  */

/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_cmsis.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_control                      Cortex-M/CMSIS    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for changing the interrupt lockout     */
/*    posture of the system.                                              */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_posture                           New interrupt lockout posture */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    old_posture                           Old interrupt lockout posture */
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
UINT _tx_thread_interrupt_control (UINT new_posture)
{
    UINT old_posture;

#ifdef TX_PORT_USE_BASEPRI

    /* Pickup current interrupt lockout posture.  */
    old_posture = __get_BASEPRI();

    /* Apply the new interrupt posture.  */
    UINT new_basepri = 0U;
    if (new_posture > 0)
    {
        new_basepri = TX_PORT_MAX_IPL << (8U - __NVIC_PRIO_BITS);
    }
    __set_BASEPRI(new_basepri);

#else

    /* Pickup current interrupt lockout posture.  */
    old_posture = __get_PRIMASK();

    /* Apply the new interrupt posture.  */
    __set_PRIMASK(new_posture);

#endif

    /* Return the original value */
    return old_posture;
}
