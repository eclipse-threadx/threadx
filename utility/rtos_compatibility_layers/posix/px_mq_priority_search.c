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
/*    posix_priority_search                               PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This routine returns the no. of messages of the same priority       */
/*    in the message queue.                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    msgQId                 message queue ID                             */
/*    priority               priority of the message                      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    order                  Returns the number of same priority messages.*/
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
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
ULONG posix_priority_search(mqd_t msgQId ,ULONG priority)
{

TX_QUEUE            *queue;
POSIX_MSG_QUEUE     *q_ptr;
ULONG                order;
ULONG                numMsgs;
UINT                 index;
ULONG               *source;
ULONG                msgp;

    queue   = &(msgQId->f_data->queue);
    q_ptr   = (POSIX_MSG_QUEUE * )queue;

    /* No. of messages in the queue.  */
    numMsgs = q_ptr -> queue.tx_queue_enqueued;

    /* retrieving the message pointer.  */
    source  =  q_ptr->queue.tx_queue_read;

    /* check for same priority.  */
    for(index = 0,order = 1;index <= numMsgs ;index++)
    {
        source += 2;
        msgp = *source;
        source += 2;

        if(source == q_ptr->queue.tx_queue_end)
            source = q_ptr->queue.tx_queue_start;
    
        if(priority == msgp)
            order += 1;
    }
    /* Returns the number of same priority messages. */
    return(order);
}
