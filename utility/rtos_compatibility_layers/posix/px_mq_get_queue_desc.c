/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
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
/** POSIX wrapper for THREADX                                             */ 
/**                                                                       */
/**                                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#include "tx_api.h"     /* Threadx API */
#include "pthread.h"    /* Posix API */
#include "px_int.h"     /* Posix helper functions */


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    posix_get_queue_desc                                PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns a message queue descriptor pointer of valid   */
/*    message queue                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    q_ptr                         message queue pointer                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    q_des                         message queue descriptor              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    posix_internal_error          Returns a Generic error               */
/*    tx_byte_allocate              Allocates bytes                       */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    POSIX internal Code                                                 */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  06-02-2021     William E. Lamie         Initial Version 6.1.7         */
/*                                                                        */
/**************************************************************************/
struct mq_des * posix_get_queue_des(POSIX_MSG_QUEUE * q_ptr)
{

struct mq_des    *q_des = NULL;
VOID             *bp;

    /* allocate the system resource allocated by open call.  */
    if( tx_byte_allocate((TX_BYTE_POOL *)&posix_heap_byte_pool, &bp,
                              sizeof(struct mq_des), TX_NO_WAIT))
    {
        /* return generic error.  */
        posix_internal_error(100);
        /* Return error.  */
        return((struct mq_des *)ERROR);
    }
    q_des = (struct mq_des *)bp;
    /* Get message queue data to mq_des structure.  */
    q_des->f_data = q_ptr;
    return(q_des);
}
