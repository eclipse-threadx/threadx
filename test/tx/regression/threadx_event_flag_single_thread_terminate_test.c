/* This test is designed to test event flag suspension with a single suspended thread
   being terminated at the same priority level.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;
static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;


extern UINT     _tx_thread_preempt_disable;

static TX_EVENT_FLAGS_GROUP group_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


static void event_set_notify(TX_EVENT_FLAGS_GROUP *group)
{

    /* Not necessary to do anything in this function.  */
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_event_flag_single_thread_terminate_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Single Thread Terminate Test..................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Single Thread Terminate Test..................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Single Thread Terminate Test..................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create event flag group 0.  */
    status =  tx_event_flags_create(&group_0, "group 0");

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Single Thread Terminate Test..................... ERROR #4\n");
        test_control_return(1);
    }

    /* Register the event set notify function.  */
    status =  tx_event_flags_set_notify(&group_0, event_set_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Single Thread Terminate Test..................... ERROR #5\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Event Flag Single Thread Terminate Test..................... ERROR #6\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Event Flag Single Thread Terminate Test..................... ");

    /* Increment run counter.  */
    thread_0_counter++;

    /* Sleep to allow lower-priority thread 1 to run.  */
    tx_thread_sleep(5);

    /* Resume Thread 2.  */
    status =  tx_thread_resume(&thread_2);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
 
    /* Now terminate thread 1.  */
    status =  tx_thread_terminate(&thread_1);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Now sleep to allow Thread 2 to run.  */
    tx_thread_sleep(5);

    /* Set only one of the event flags needed.  */
    status =  tx_event_flags_set(&group_0, 0x00080000, TX_OR);

    /* Now sleep to allow thread 2 to run.  */
    tx_thread_sleep(5);

    /* Check status.  */
    if ((status != TX_SUCCESS) && (thread_2_counter != 1))
    {

        /* Event flag error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Set the other event flag needed.  */
    status =  tx_event_flags_set(&group_0, 0x00800000, TX_OR);

    /* Now sleep to allow thread 2 to run.  */
    tx_thread_sleep(5);

    /* Check status.  */
    if ((status != TX_SUCCESS) && (thread_2_counter != 2))
    {

        /* Event flag error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* At this point, thread 2 is suspended on the flags again. Or some flags that are 
       not needed.  */
       
    /* Set an event flag that is not needed.  */
    status =  tx_event_flags_set(&group_0, 0x00000001, TX_OR);

    /* Now sleep to allow thread 2 to run.  */
    tx_thread_sleep(5);

    /* Check status.  */
    if ((status != TX_SUCCESS) && (thread_2_counter != 2))
    {

        /* Event flag error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Set an event flag that is needed.  */
    status =  tx_event_flags_set(&group_0, 0x00080001, TX_OR);

    /* Now sleep to allow thread 2 to run.  */
    tx_thread_sleep(5);

    /* Check status and run counters.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 3) || 
        (_tx_thread_preempt_disable))
    {

        /* Event flag error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Terminate thread 2.  */
    status =  tx_thread_terminate(&thread_2);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (thread_2_counter != 3))
    {

        /* Event flag error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* Wait for event flags.  */
    while(1)
    {

        /* Increment run counter.  */
        thread_1_counter++;

        /* Self suspend thread.  */
        status =  tx_thread_suspend(&thread_1);

        /* Check status.  */
        if (status != TX_SUCCESS)
        {
            thread_1_counter =  0;  /* Make an error!  */ 
            return;
        }
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual_events;

    /* Wait for event flags.  */
    while(1)
    {

        /* Increment run counter.  */
        thread_2_counter++;

        /* Attempt to get events from event flag group.  AND option.  */
        status =  tx_event_flags_get(&group_0, 0x000880000, TX_AND_CLEAR, &actual_events, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            return;

        /* Increment run counter.  */
        thread_2_counter++;

        /* Attempt to get events from event flag group.  OR option.  */
        status =  tx_event_flags_get(&group_0, 0x000880000, TX_OR_CLEAR, &actual_events, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            return;

        /* Increment run counter.  */
        thread_2_counter++;

        /* Attempt to get events from event flag group.  AND option.  */
        status =  tx_event_flags_get(&group_0, 0x000880000, TX_AND_CLEAR, &actual_events, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_NO_EVENTS)
            return;
    }
}

