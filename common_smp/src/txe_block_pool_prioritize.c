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
/**   Block Pool                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_block_pool.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_block_pool_prioritize                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the block pool prioritize call.  */
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
/*    _tx_block_pool_prioritize         Actual block pool prioritize      */
/*                                        function                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/**************************************************************************/
UINT  _txe_block_pool_prioritize(TX_BLOCK_POOL *pool_ptr)
{

UINT    status;


    /* Check for an invalid block memory pool pointer.  */
    if (pool_ptr == TX_NULL)
    {

        /* Block memory pointer is invalid, return appropriate error code.  */
        status =  TX_POOL_ERROR;
    }

    /* Now check for invalid pool ID.  */
    else if (pool_ptr -> tx_block_pool_id != TX_BLOCK_POOL_ID)
    {

        /* Block memory pointer is invalid, return appropriate error code.  */
        status =  TX_POOL_ERROR;
    }
    else
    {

        /* Call actual block pool prioritize function.  */
        status =  _tx_block_pool_prioritize(pool_ptr);
    }

    /* Return completion status.  */
    return(status);
}

