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
/** ThreadX Component                                                     */
/**                                                                       */
/**   ThreadX/GHS Event Log (EL)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE
#define TX_EL_SOURCE_CODE


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_el.h"
#include "string.h"


/* Define global variables used to manage the event pool.  */

UCHAR   *_tx_el_tni_start;
UCHAR   **_tx_el_current_event;
UCHAR   *_tx_el_event_area_start;
UCHAR   *_tx_el_event_area_end;
UINT    _tx_el_maximum_events;
ULONG   _tx_el_total_events;
UINT    _tx_el_event_filter;
ULONG   _tx_el_time_base_upper;
ULONG   _tx_el_time_base_lower;

extern char __ghsbegin_eventlog[];
extern char __ghsend_eventlog[];

extern  TX_THREAD   *_tx_thread_current_ptr;
UINT                _tx_thread_interrupt_control(UINT new_posture);


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_initialize                                   PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function creates the Event Log (in the format dictated by the  */
/*    GHS Event Analyzer) and sets up various information for subsequent  */
/*    operation.  The start and end of the Event Log is determined by the */
/*    .eventlog section in the linker control file.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
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
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_initialize(VOID)
{

UCHAR   *work_ptr;
UCHAR   *read_ptr;
ULONG   event_log_size;
UCHAR   *end_ptr;
UINT    i;


    /* Clear total event counter.  */
    _tx_el_total_events =  0;

    /* Clear event filter.  */
    _tx_el_event_filter =  0;

    /* First, pickup the starting and ending address of the Event Log memory.  */
    work_ptr =  (unsigned char *) __ghsbegin_eventlog;
    end_ptr =   (unsigned char *) __ghsend_eventlog;

    /* Calculate the event log size.  */
    event_log_size =  end_ptr - work_ptr;

    /* Subtract off the number of bytes in the header and the TNI area.  */
    event_log_size =  event_log_size - (TX_EL_HEADER_SIZE +
                                       (TX_EL_TNI_ENTRY_SIZE * TX_EL_TNIS));

    /* Make sure the event log is evenly divisible by the event size.  */
    event_log_size =  (event_log_size/TX_EL_EVENT_SIZE) * TX_EL_EVENT_SIZE;

    /* Build the Event Log header.  */

    /* Setup the Event Log Version ID.  */
    *((unsigned short *) work_ptr) =  (unsigned short) TX_EL_VERSION_ID;
    work_ptr =  work_ptr + sizeof(unsigned short);

    /* Setup the TNIS (number of thread names) field.  */
    *((unsigned short *) work_ptr) =  (unsigned short) TX_EL_TNIS;
    work_ptr =  work_ptr + sizeof(unsigned short);

    /* Setup the EVPS (event pool size) field.  */
    *((ULONG *) work_ptr) =  event_log_size;
    work_ptr =  work_ptr + sizeof(ULONG);

    /* Remember the maximum number of events.  */
    _tx_el_maximum_events =  event_log_size/TX_EL_EVENT_SIZE;

    /* Setup max_events field.  */
    *((ULONG *) work_ptr) =  _tx_el_maximum_events;
    work_ptr =  work_ptr + sizeof(ULONG);

    /* Setup the evploc (location of event pool).  */
    *((ULONG *) work_ptr) =  (ULONG) (((ULONG) __ghsbegin_eventlog) + TX_EL_HEADER_SIZE +
                                        (TX_EL_TNIS * TX_EL_TNI_ENTRY_SIZE));
    work_ptr =  work_ptr + sizeof(ULONG);

    /* Save the current event pointer.  */
    _tx_el_current_event =  (UCHAR **) work_ptr;

    /* Setup event_ptr (pointer to oldest event) field to the start
       of the event pool.  */
    *_tx_el_current_event =  (UCHAR *) (((ULONG) __ghsbegin_eventlog) + TX_EL_HEADER_SIZE +
                                        (TX_EL_TNIS * TX_EL_TNI_ENTRY_SIZE));
    work_ptr =  work_ptr + sizeof(ULONG);

    /* Setup tbfreq (the number of ticks in a second) field.  */
    *((ULONG *) work_ptr) =  TX_EL_TICKS_PER_SECOND;
    work_ptr =  work_ptr + sizeof(ULONG);

    /* At this point we are pointing at the Thread Name Information (TNI) array.  */

    /* Remember the start of this for future updates.  */
    _tx_el_tni_start =  work_ptr;

    /* Clear the entire TNI array, this is the initial setting.  */
    end_ptr =  work_ptr + (TX_EL_TNIS * TX_EL_TNI_ENTRY_SIZE);
    memset((void *)work_ptr, 0, (TX_EL_TNIS * TX_EL_TNI_ENTRY_SIZE));
    work_ptr = end_ptr;

    /* At this point, we are pointing at the actual Event Entry area.  */

    /* Remember the start of the actual event log area.  */
    _tx_el_event_area_start =  work_ptr;

    /* Clear the entire Event area.  */
    end_ptr =  work_ptr + event_log_size;
    memset((void *)work_ptr, 0, event_log_size);
    work_ptr = end_ptr;

    /* Save the end pointer for later use.  */
    _tx_el_event_area_end =  work_ptr;

    /* Setup an entry to resolve all activities from initialization and from
       an idle system.  */
    work_ptr =  _tx_el_tni_start;
    read_ptr =  (UCHAR *) "Initialization/System Idle";
    i =         0;
    while ((i < TX_EL_TNI_NAME_SIZE) && (*read_ptr))
    {

        /* Copy a character of thread's name into TNI area of log.  */
        *work_ptr++ =  *read_ptr++;

        /* Increment the character count.  */
        i++;
    }

    /* Determine if a NULL needs to be inserted.  */
    if (i < TX_EL_TNI_NAME_SIZE)
    {

        /* Yes, insert a NULL into the event log string.  */
        *work_ptr =  (unsigned char) 0;
    }

    /* Setup the thread ID to NULL.  */
    *((ULONG *) (_tx_el_tni_start + TX_EL_TNI_THREAD_ID_OFFSET)) =  (ULONG) TX_NULL;

    /* Set the valid field to indicate the entry is complete.  */
    *((UCHAR *) (_tx_el_tni_start + TX_EL_TNI_VALID_OFFSET)) =  (ULONG) TX_EL_VALID_ENTRY;

    /* Clear the time base global variables.  */
    _tx_el_time_base_upper =  0;
    _tx_el_time_base_lower =  0;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_thread_register                              PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function registers a thread in the event log for future        */
/*    display purposes.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Pointer to thread control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_SUCCESS                        Thread was placed in TNI area     */
/*    TX_ERROR                          No more room in the TNI area      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_create                 ThreadX thread create function    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _tx_el_thread_register(TX_THREAD *thread_ptr)
{

UCHAR   *entry_ptr;
UCHAR   *work_ptr;
UCHAR   *read_ptr;
UINT    i;


    /* First of all, search for a free slot in the TNI area.  */
    entry_ptr =  _tx_el_tni_start;
    i =         0;
    while (i < TX_EL_TNIS)
    {

        /* Determine if this entry is available.  */
        if (*(entry_ptr + TX_EL_TNI_VALID_OFFSET) == TX_EL_INVALID_ENTRY)
            break;

        /* Otherwise, increment the associated pointers and indices.  */
        i++;
        entry_ptr =  entry_ptr + TX_EL_TNI_ENTRY_SIZE;
    }

    /* Check to see if there were no more valid entries.  */
    if (i >= TX_EL_TNIS)
        return(TX_EL_NO_MORE_TNI_ROOM);

    /* Otherwise, we have room in the TNI and a valid record.  */

    /* Setup the thread's name.  */
    work_ptr =  entry_ptr;
    read_ptr =  (UCHAR *) thread_ptr -> tx_thread_name;
    i =         0;
    while ((i < TX_EL_TNI_NAME_SIZE) && (*read_ptr))
    {

        /* Copy a character of thread's name into TNI area of log.  */
        *work_ptr++ =  *read_ptr++;

        /* Increment the character count.  */
        i++;
    }

    /* Determine if a NULL needs to be inserted.  */
    if (i < TX_EL_TNI_NAME_SIZE)
    {

        /* Yes, insert a NULL into the event log string.  */
        *work_ptr =  (unsigned char) 0;
    }

    /* Setup the thread ID.  */
    *((ULONG *) (entry_ptr + TX_EL_TNI_THREAD_ID_OFFSET)) =  (ULONG) thread_ptr;

    /* Setup the thread priority.  */
    *((ULONG *) (entry_ptr + TX_EL_TNI_THREAD_PRIORITY_OFF)) =  (ULONG) thread_ptr -> tx_thread_priority;

    /* Set the valid field to indicate the entry is complete.  */
    *((UCHAR *) (entry_ptr + TX_EL_TNI_VALID_OFFSET)) =  (ULONG) TX_EL_VALID_ENTRY;

    /* Thread name has been registered.  */
    return(TX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_thread_unregister                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function unregisters a thread in the event log for future      */
/*    display purposes.                                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                        Pointer to thread control block   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    TX_SUCCESS                        Thread was placed in TNI area     */
/*    TX_ERROR                          No more room in the TNI area      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_thread_create                 ThreadX thread create function    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _tx_el_thread_unregister(TX_THREAD *thread_ptr)
{

UCHAR   *entry_ptr;
UCHAR   *work_ptr;
UCHAR   *read_ptr;
UINT    found;
UINT    i, j;


    /* First of all, search for a match in the TNI area.  */
    entry_ptr =  _tx_el_tni_start;
    i =         0;
    while (i < TX_EL_TNIS)
    {

        /* Determine if this entry is a match.  */
        work_ptr =  entry_ptr;
        read_ptr =  (UCHAR *) thread_ptr -> tx_thread_name;
        found =     TX_TRUE;
        j =         0;
        do
        {

            /* Determine if this character is the same.  */
            if (*work_ptr != *read_ptr)
            {

                /* Set found to false and fall out of the loop.  */
                found =  TX_FALSE;
                break;
            }
            else if (*work_ptr == 0)
            {

                /* Null terminated, just break the loop.  */
                break;
            }
            else
            {

                /* Copy a character of thread's name into TNI area of log.  */
                *work_ptr++ =  *read_ptr++;
            }

            /* Increment the character count.  */
            j++;

        } while(j < TX_EL_TNIS);


        /* Was a match found?  */
        if (found)
        {

            /* Yes, mark the entry as available now.  */
            *(entry_ptr + TX_EL_TNI_VALID_OFFSET) = TX_EL_INVALID_ENTRY;

            /* Get out of the loop!  */
            break;
        }

        /* Otherwise, increment the associated pointers and indices.  */
        i++;
        entry_ptr =  entry_ptr + TX_EL_TNI_ENTRY_SIZE;
    }

    /* Determine status to return.  */
    if (found)
        return(TX_SUCCESS);
    else
        return(TX_EL_NAME_NOT_FOUND);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_user_event_insert                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts a user event into the event log.              */
/*    If the event log is full, the oldest event is overwritten.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    sub_type                              Event subtype for kernel call */
/*    info_1                                First information field       */
/*    info_2                                Second information field      */
/*    info_3                                Third information field       */
/*    info_4                                Fourth information field      */
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
/*    ThreadX services                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_user_event_insert(UINT sub_type, ULONG info_1, ULONG info_2,
                                                    ULONG info_3, ULONG info_4)
{

TX_INTERRUPT_SAVE_AREA

UINT    upper_tb;
UCHAR   *entry_ptr;

    /* Disable interrupts.  */
    TX_DISABLE

    /* Increment total event counter.  */
    _tx_el_total_events++;

    /* Setup working entry pointer first.  */
    entry_ptr =  *_tx_el_current_event;

    /* Store the event type.  */
    *((unsigned short *) entry_ptr) =  (unsigned short) TX_EL_USER_EVENT;

    /* Store the event subtype.  */
    *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) =
                                (unsigned short) sub_type;

    /* Get time stamp.  */
    do
    {

        /* Pickup the upper tb.  */
        upper_tb =  (ULONG) read_tbu();

        /* Store the upper time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) =
                                (ULONG) upper_tb;

        /* Store the lower time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =
                                (ULONG) read_tbl();
    } while (upper_tb != (ULONG) read_tbu());

    /* Store the current thread.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =
                                (ULONG) _tx_thread_current_ptr;

    /* Store the first info field.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =
                                (ULONG) info_1;

    /* Store the second info field.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_2_OFFSET)) =
                                (ULONG) info_2;

    /* Store the third info field.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_3_OFFSET)) =
                                (ULONG) info_3;

    /* Store the fourth info field.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_4_OFFSET)) =
                                (ULONG) info_4;

    /* Now move the current event log pointer.  */
    entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;

    /* Check for a wraparound condition.  */
    if (entry_ptr >= _tx_el_event_area_end)
    {

        /* Yes, we have wrapped around to the end of the event area.
           Start back at the top!  */
        entry_ptr =  _tx_el_event_area_start;
    }

    /* Write the entry pointer back into the header.  */
    *_tx_el_current_event =  entry_ptr;

    /* Restore interrupts.  */
    TX_RESTORE
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_thread_running                               PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts a thread change event into the event          */
/*    log, which indicates that a context switch is taking place.         */
/*    If the event log is full, the oldest event is overwritten.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread being       */
/*                                            scheduled                   */
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
/*    _tx_thread_schedule                   ThreadX scheduler             */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_thread_running(TX_THREAD *thread_ptr)
{

UINT    upper_tb;
UCHAR   *entry_ptr;

    TX_EL_NO_STATUS_EVENTS

    /* Increment total event counter.  */
    _tx_el_total_events++;

    /* Setup working entry pointer first.  */
    entry_ptr =  *_tx_el_current_event;

    /* Store the event type.  */
    *((unsigned short *) entry_ptr) =  (unsigned short) TX_EL_THREAD_CHANGE;

    /* Store the event subtype.  */
    *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) =
                                (unsigned short) 0;

    /* Get time stamp.  */
    do
    {

        /* Pickup the upper tb.  */
        upper_tb =  (ULONG) read_tbu();

        /* Store the upper time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) =
                                (ULONG) upper_tb;

        /* Store the lower time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =
                                (ULONG) read_tbl();
    } while (upper_tb != (ULONG) read_tbu());

    /* Store the current thread.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =
                                (ULONG) thread_ptr;

    /* Now move the current event log pointer.  */
    entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;

    /* Check for a wraparound condition.  */
    if (entry_ptr >= _tx_el_event_area_end)
    {

        /* Yes, we have wrapped around to the end of the event area.
           Start back at the top!  */
        entry_ptr =  _tx_el_event_area_start;
    }

    /* Write the entry pointer back into the header.  */
    *_tx_el_current_event =  entry_ptr;

    TX_EL_END_FILTER
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_thread_preempted                             PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts a thread preempted event into the event       */
/*    log, which indicates that an interrupt occurred that made a higher  */
/*    priority thread ready for execution.  In this case, the previously  */
/*    executing thread has an event entered to indicate it is no longer   */
/*    running.                                                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    thread_ptr                            Pointer to thread being       */
/*                                            scheduled                   */
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
/*    _tx_thread_context_restore            ThreadX context restore       */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_thread_preempted(TX_THREAD *thread_ptr)
{

UINT    upper_tb;
UCHAR   *entry_ptr;


    TX_EL_NO_STATUS_EVENTS

    /* Increment total event counter.  */
    _tx_el_total_events++;

    /* Setup working entry pointer first.  */
    entry_ptr =  *_tx_el_current_event;

    /* Store the event type.  */
    *((unsigned short *) entry_ptr) =  (unsigned short) TX_EL_THREAD_STATUS_CHANGE;

    /* Store the event subtype.  */
    *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) =
                                (unsigned short) TX_READY;

    /* Get time stamp.  */
    do
    {

        /* Pickup the upper tb.  */
        upper_tb =  (ULONG) read_tbu();

        /* Store the upper time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) =
                                (ULONG) upper_tb;

        /* Store the lower time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =
                                (ULONG) read_tbl();
    } while (upper_tb != (ULONG) read_tbu());

    /* Store the current thread.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =
                                (ULONG) _tx_thread_current_ptr;

    /* Now move the current event log pointer.  */
    entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;

    /* Check for a wraparound condition.  */
    if (entry_ptr >= _tx_el_event_area_end)
    {

        /* Yes, we have wrapped around to the end of the event area.
           Start back at the top!  */
        entry_ptr =  _tx_el_event_area_start;
    }

    /* Write the entry pointer back into the header.  */
    *_tx_el_current_event =  entry_ptr;

    TX_EL_END_FILTER
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_interrupt                                    PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts an interrupt event into the log, which        */
/*    indicates the start of interrupt processing for the specific        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    interrupt_number                      Interrupt number supplied by  */
/*                                            ISR                         */
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
/*    ISR processing                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_interrupt(UINT interrupt_number)
{

UINT    upper_tb;
UCHAR   *entry_ptr;


    TX_EL_NO_INTERRUPT_EVENTS

    /* Increment total event counter.  */
    _tx_el_total_events++;

    /* Setup working entry pointer first.  */
    entry_ptr =  *_tx_el_current_event;

    /* Store the event type.  */
    *((unsigned short *) entry_ptr) =  (unsigned short) TX_EL_INTERRUPT;

    /* Store the event subtype.  */
    *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) =
                                (unsigned short) TX_EL_INTERRUPT_SUB_TYPE;

    /* Get time stamp.  */
    do
    {

        /* Pickup the upper tb.  */
        upper_tb =  (ULONG) read_tbu();

        /* Store the upper time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) =
                                (ULONG) upper_tb;

        /* Store the lower time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =
                                (ULONG) read_tbl();
    } while (upper_tb != (ULONG) read_tbu());

    /* Store the current thread.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =
                                (ULONG) _tx_thread_current_ptr;

    /* Store the first info word.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =
                                (ULONG) interrupt_number;

    /* Now move the current event log pointer.  */
    entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;

    /* Check for a wraparound condition.  */
    if (entry_ptr >= _tx_el_event_area_end)
    {

        /* Yes, we have wrapped around to the end of the event area.
           Start back at the top!  */
        entry_ptr =  _tx_el_event_area_start;
    }

    /* Write the entry pointer back into the header.  */
    *_tx_el_current_event =  entry_ptr;

    TX_EL_END_FILTER
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_interrupt_end                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function inserts an interrupt end event into the log, which    */
/*    indicates the end of interrupt processing for the specific          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    interrupt_number                      Interrupt number supplied by  */
/*                                            ISR                         */
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
/*    ISR processing                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_interrupt_end(UINT interrupt_number)
{

UINT    upper_tb;
UCHAR   *entry_ptr;


    TX_EL_NO_INTERRUPT_EVENTS

    /* Increment total event counter.  */
    _tx_el_total_events++;

    /* Setup working entry pointer first.  */
    entry_ptr =  *_tx_el_current_event;

    /* Store the event type.  */
    *((unsigned short *) entry_ptr) =  (unsigned short) TX_EL_INTERRUPT;

    /* Store the event subtype.  */
    *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) =
                                (unsigned short) TX_EL_END_OF_INTERRUPT;

    /* Get time stamp.  */
    do
    {

        /* Pickup the upper tb.  */
        upper_tb =  (ULONG) read_tbu();

        /* Store the upper time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) =
                                (ULONG) upper_tb;

        /* Store the lower time stamp.  */
        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =
                                (ULONG) read_tbl();
    } while (upper_tb != (ULONG) read_tbu());

    /* Store the current thread.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =
                                (ULONG) _tx_thread_current_ptr;

    /* Store the first info word.  */
    *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =
                                (ULONG) interrupt_number;

    /* Now move the current event log pointer.  */
    entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;

    /* Check for a wraparound condition.  */
    if (entry_ptr >= _tx_el_event_area_end)
    {

        /* Yes, we have wrapped around to the end of the event area.
           Start back at the top!  */
        entry_ptr =  _tx_el_event_area_start;
    }

    /* Write the entry pointer back into the header.  */
    *_tx_el_current_event =  entry_ptr;

    TX_EL_END_FILTER
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_interrupt_control                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function remaps the tx_interrupt_control service call so that  */
/*    it can be tracked in the event log.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    new_posture                           New interrupt posture         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    old_posture                           Old interrupt posture         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _tx_thread_interrupt_control          Interrupt control service     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    ThreadX services                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT  _tx_el_interrupt_control(UINT new_posture)
{

TX_INTERRUPT_SAVE_AREA
UINT    old_posture;


    TX_EL_NO_INTERRUPT_EVENTS

    TX_DISABLE
    TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_INTERRUPT_CONTROL, _tx_thread_current_ptr, new_posture)
    TX_RESTORE

    TX_EL_END_FILTER

    old_posture =  _tx_thread_interrupt_control(new_posture);
    return(old_posture);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_event_log_on                                 PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function disables all event filters.                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
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
/*    Application code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_event_log_on(void)
{

    /* Disable all event filters.  */
    _tx_el_event_filter =  TX_EL_ENABLE_ALL_EVENTS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_event_log_off                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets all event filters, thereby turning event         */
/*    logging off.                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
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
/*    Application code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_event_log_off(void)
{

    /* Set all event filters.  */
    _tx_el_event_filter =  TX_EL_FILTER_ALL_EVENTS;
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_el_event_log_set                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function sets the events filters specified by the user.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    filter                            Events to filter                  */
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
/*    Application code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
VOID  _tx_el_event_filter_set(UINT filter)
{

    /* Apply the user event filter.  */
    _tx_el_event_filter =  filter;
}

