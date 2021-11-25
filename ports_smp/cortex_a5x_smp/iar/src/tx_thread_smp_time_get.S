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
/**   Thread - Low Level SMP Support                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/*
#define TX_SOURCE_CODE
#define TX_THREAD_SMP_SOURCE_CODE
*/

/* Include necessary system files.  */

/*
#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
*/

    SECTION `.text`:CODE:NOROOT(3)
    CODE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_smp_time_get                          Cortex-A5x-SMP/IAR */
/*                                                            6.1.9       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function gets the global time value that is used for debug     */
/*    information and event tracing.                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    32-bit time stamp                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX Source                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-15-2021      William E. Lamie        Initial Version 6.1.9         */
/*                                                                        */
/**************************************************************************/
    PUBLIC   _tx_thread_smp_time_get
_tx_thread_smp_time_get:
    MOV     x0, #0          // Add logic to get timer value here for diagnostics
    RET

    END
