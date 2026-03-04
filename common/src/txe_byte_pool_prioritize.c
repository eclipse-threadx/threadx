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
/**   Byte Memory                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_byte_pool.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_byte_pool_prioritize                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the byte pool prioritize call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pool_ptr                          Pointer to pool control block     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_byte_pool_prioritize         Actual byte pool prioritize        */
/*                                        function                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txe_byte_pool_prioritize(TX_BYTE_POOL *pool_ptr)
{

UINT        status;


    /* Check for an invalid byte memory pool pointer.  */
    if (pool_ptr == TX_NULL)
    {

        /* Byte pool pointer is invalid, return appropriate error code.  */
        status =  TX_POOL_ERROR;
    }

    /* Now check for invalid pool ID.  */
    else if (pool_ptr -> tx_byte_pool_id != TX_BYTE_POOL_ID)
    {

        /* Byte pool pointer is invalid, return appropriate error code.  */
        status =  TX_POOL_ERROR;
    }
    else
    {

        /* Call actual byte pool prioritize function.  */
        status =  _tx_byte_pool_prioritize(pool_ptr);
    }

    /* Return completion status.  */
    return(status);
}

