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
/**   Queue                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_queue.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_queue_info_get                                  PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function retrieves information from the specified queue.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    queue_ptr                         Pointer to queue control block    */
/*    name                              Destination for the queue name    */
/*    enqueued                          Destination for enqueued count    */
/*    available_storage                 Destination for available storage */
/*    first_suspended                   Destination for pointer of first  */
/*                                        thread suspended on this queue  */
/*    suspended_count                   Destination for suspended count   */
/*    next_queue                        Destination for pointer to next   */
/*                                        queue on the created list       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
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
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     Yuxin Zhou               Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _tx_queue_info_get(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage,
                    TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue)
{

TX_INTERRUPT_SAVE_AREA


    /* Disable interrupts.  */
    TX_DISABLE

    /* If trace is enabled, insert this event into the trace buffer.  */
    TX_TRACE_IN_LINE_INSERT(TX_TRACE_QUEUE_INFO_GET, queue_ptr, 0, 0, 0, TX_TRACE_QUEUE_EVENTS)

    /* Log this kernel call.  */
    TX_EL_QUEUE_INFO_GET_INSERT

    /* Retrieve all the pertinent information and return it in the supplied
       destinations.  */

    /* Retrieve the name of the queue.  */
    if (name != TX_NULL)
    {
    
        *name =  queue_ptr -> tx_queue_name;
    }
    
    /* Retrieve the number of messages currently in the queue.  */
    if (enqueued != TX_NULL)
    {
    
        *enqueued =  (ULONG) queue_ptr -> tx_queue_enqueued;
    }
    
    /* Retrieve the number of messages that will still fit in the queue.  */
    if (available_storage != TX_NULL)
    {
    
        *available_storage =  (ULONG) queue_ptr -> tx_queue_available_storage;
    }
    
    /* Retrieve the first thread suspended on this queue.  */
    if (first_suspended != TX_NULL)
    {
    
        *first_suspended =  queue_ptr -> tx_queue_suspension_list;
    }
    
    /* Retrieve the number of threads suspended on this queue.  */
    if (suspended_count != TX_NULL)
    {
    
        *suspended_count =  (ULONG) queue_ptr -> tx_queue_suspended_count;
    }
    
    /* Retrieve the pointer to the next queue created.  */
    if (next_queue != TX_NULL)
    {
    
        *next_queue =  queue_ptr -> tx_queue_created_next;
    }
    
    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(TX_SUCCESS);
}

