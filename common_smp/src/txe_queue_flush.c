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
/**   Queue                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_queue.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _txe_queue_flush                                    PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the queue flush function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    queue_ptr                         Pointer to queue control block    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_QUEUE_ERROR                    Invalid queue pointer             */
/*    TX_CALLER_ERROR                   Invalid caller of this function   */
/*    status                            Actual completion status          */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_queue_flush                   Actual queue flush function       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txe_queue_flush(TX_QUEUE *queue_ptr)
{

UINT        status;


    /* Check for an invalid queue pointer.  */
    if (queue_ptr == TX_NULL)
    {

        /* Queue pointer is invalid, return appropriate error code.  */
        status =  TX_QUEUE_ERROR;
    }

    /* Now check for invalid queue ID.  */
    else if (queue_ptr -> tx_queue_id != TX_QUEUE_ID)
    {

        /* Queue pointer is invalid, return appropriate error code.  */
        status =  TX_QUEUE_ERROR;
    }
    else
    {

        /* Call actual queue flush function.  */
        status =  _tx_queue_flush(queue_ptr);
    }

    /* Return completion status.  */
    return(status);
}

