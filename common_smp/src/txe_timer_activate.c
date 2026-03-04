/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation
 * Copyright (c) 2026-present Eclipse ThreadX contributors
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 *
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Timer                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_timer.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txe_timer_activate                                 PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the activate application timer   */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    timer_ptr                         Pointer to timer control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_TIMER_ERROR                    Invalid application timer         */
/*    TX_ACTIVATE_ERROR                 Application timer already active  */
/*    status                            Actual completion status          */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_timer_activate                Actual application timer activate */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txe_timer_activate(TX_TIMER *timer_ptr)
{

UINT    status;


    /* Check for an invalid timer pointer.  */
    if (timer_ptr == TX_NULL)
    {
        /* Timer pointer is invalid, return appropriate error code.  */
        status =  TX_TIMER_ERROR;
    }

    /* Now check for invalid timer ID.  */
    else if (timer_ptr -> tx_timer_id != TX_TIMER_ID)
    {
        /* Timer pointer is invalid, return appropriate error code.  */
        status =  TX_TIMER_ERROR;
    }
    else
    {

        /* Call actual application timer activate function.  */
        status =  _tx_timer_activate(timer_ptr);
    }

    /* Return completion status.  */
    return(status);
}

