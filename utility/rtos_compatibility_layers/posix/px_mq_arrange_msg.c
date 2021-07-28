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
/*    posix_arrange_msg                                   PORTABLE C      */
/*                                                           6.1.7        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    arrange messages from the queue                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    Queue                         queue descriptor                      */
/*   *pMsgPrio                      If not NULL, priority of message      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    OK                            Always return successful              */
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
ULONG posix_arrange_msg( TX_QUEUE *Queue, ULONG *pMsgPrio )
{

    ULONG*          Qread;          /* to store read ptr of the queue        */
    ULONG*          temp_q = TX_NULL; /* temp storage for the message pointer  */
    ULONG           numMsgs;        /* no of messages queued                 */
    ULONG           msg;            /* temp variable for thr for loop        */
    ULONG           priority;       /* priority of the message               */
    ULONG           maxPrio;        /* max. priority of the messages in queue*/
    ULONG           number2;        /* messages                              */
    ULONG           minNo;          /* oldest message in the same priority   */
    ULONG           swap;           /* temp.variable for the swapping of the */
                                    /* messages                              */

    /* initialize the priority to the lowest priority.  */
    maxPrio = 0;
    minNo=0;

    /* Copy read pointer to the temporary variable.  */
    Qread = Queue -> tx_queue_read;

    /* Copy no. of messages in the queue to the temporary variable.  */
    numMsgs = Queue -> tx_queue_enqueued;

    if( numMsgs == 0 )    
        return(OK);

    for( msg = 0; msg < numMsgs; msg ++)
    {

    /* Advance Qread by two pointers to read the priority of the message.  */
        Qread = Qread + 2 ;

        /* Priority of the message queued.  */
        priority = *Qread;

        /* check with maxpriority.  */
        if( priority > maxPrio )
        {  
            /* copy read pointer to temporary buffer.  */
            temp_q = Qread-2;
            /* increment read pointer to point to order.  */
            Qread++;

            /* copy FIFO order to the message  */
            minNo = *Qread;
            /* Found higher priority message.  */
            maxPrio = priority;

            Qread++; 
        }
    
    /* if more than one same priority messages are in the queue 
           then check if this the oldest one.  */
        else if ( priority == maxPrio )
        {
            /* increment read pointer to point to read FIFO order */
            Qread++;

            /* copy number to the local varialble.  */
            number2 = *Qread;
           Qread++;
           /* find the oldest of the messages in this priority level.  */
           if( number2 < minNo )
           {
               /* founder older one  */
               minNo = number2;
               /* copy read pointer to temporary buffer.  */
               temp_q = Qread - 4;
           }
       
        }

    else
	    Qread = Qread + 2;

            /* Determine if we are at the end.  */
            if ( Qread >= Queue ->tx_queue_end)

                /* Yes, wrap around to the beginning.  */
                Qread =  Queue -> tx_queue_start;
    }

    /* All messages checked temp holds address of highest priority message and
       maxPrio holds the highest priority*/

    if( pMsgPrio != NULL )
    {
        /* copy message priority.  */
        *pMsgPrio = maxPrio;
    }

    /* Get the current queue read pointer */
    Qread = Queue -> tx_queue_read;
    
    /*  if(*pMsgPrio != *(Qread + 2) || minNo < *(Qread + 3))*/
    {
        /* Swap the messages.  */
        for ( msg = 0; msg < 4; msg++)
        {

            swap = *temp_q;
            *temp_q = *Qread;
            *Qread = swap;
            temp_q++;
            Qread++;
        }
    }
    
    return(OK);
}
