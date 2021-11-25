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

/*
#include "tx_api.h"
#include "tx_thread.h"
*/

    SECTION `.text`:CODE:NOROOT(3)
    CODE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_interrupt_restore                     Cortex-A5x-SMP/IAR */
/*                                                            6.1.9       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is responsible for restoring interrupts to the state  */
/*    returned by a previous _tx_thread_interrupt_disable call.           */
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
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-15-2021      William E. Lamie        Initial Version 6.1.9         */
/*                                                                        */
/**************************************************************************/
/* UINT   _tx_thread_interrupt_restore(UINT old_posture)
{ */
    PUBLIC  _tx_thread_interrupt_restore
_tx_thread_interrupt_restore:

    /* Restore the old interrupt posture.  */

    MSR     DAIF, x0                            // Setup the old posture
    RET                                         // Return to caller

/* }  */
    END
