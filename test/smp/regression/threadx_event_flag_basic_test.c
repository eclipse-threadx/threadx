/* This test is designed to test simple event flag group creation, deletion, gets and
   sets.  */

#include   <stdio.h>
#include   "tx_api.h"


typedef struct EVENT_FLAG_MEMORY_TEST_STRUCT
{
    ULONG                   first;
    ULONG                   second;
    TX_EVENT_FLAGS_GROUP    event_flags;
    ULONG                   next_to_last;
    ULONG                   last;

} EVENT_FLAG_MEMORY_TEST;

static  EVENT_FLAG_MEMORY_TEST   event_flag_memory;


/* Define external reference to the event flag create call from initialization.   */

extern UINT            test_event_flags_from_init;


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_TIMER        timer_0;

static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;

static TX_EVENT_FLAGS_GROUP group_0;
static TX_EVENT_FLAGS_GROUP group_1;
static TX_EVENT_FLAGS_GROUP group_2;
static TX_EVENT_FLAGS_GROUP group_3;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
UINT        _txe_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT event_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void event_set_notify(TX_EVENT_FLAGS_GROUP *group)
{

    /* Not necessary to do anything in this function.  */
}


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;
ULONG   actual_events;


    /* Determine if calling event flag create from initialization was successful.  */
    if (test_event_flags_from_init != TX_SUCCESS)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create an event flag group from a timer.  */
    status =  tx_event_flags_create(&group_2, "group 2");
    
        /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Try to delete an event flags group from a timer.  */
    status =  tx_event_flags_delete(&group_0);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Get events with suspendsion from a timer.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, 14, &actual_events, 100);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    timer_executed =  1;
#endif
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;
ULONG   actual_events;


    /* Attempt to create an event flag group from an ISR.  */
    status =  tx_event_flags_create(&group_2, "group 2");

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete an event flags group from an ISR.  */
    status =  tx_event_flags_delete(&group_0);

    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Get events with suspendsion from an ISR.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, 14, &actual_events, 100);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }


    isr_executed =  1;
#endif
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_event_flag_basic_application_define(void *first_unused_memory)
#endif
{

INT     status;
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

        printf("Running Event Flag Basic Test....................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #2\n");
        test_control_return(1);
    }

     /* Create event flag group 0 and 1.  */
    status =  tx_event_flags_create(&group_0, "group 0");
 
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_event_flags_create(&group_1, "group 1");

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #4\n");
        test_control_return(1);
    }
    
    /* Register the event set notify function.  */
    status =  tx_event_flags_set_notify(&group_0, event_set_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #5\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #6\n");
        test_control_return(1);
    }

#endif

    /* Un-register the event set notify function.  */
    status =  tx_event_flags_set_notify(&group_0, TX_NULL);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #5a\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Event Flag Basic Test....................................... ERROR #6a\n");
        test_control_return(1);
    }
#endif
 
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual_events;


    /* Inform user.  */
    printf("Running Event Flag Basic Test....................................... ");

    /* Perform event flag memory test.  */
    event_flag_memory.first =        0x11223344;
    event_flag_memory.second =       0x55667788;
    event_flag_memory.next_to_last = 0x99aabbcc;
    event_flag_memory.last =         0xddeeff00;
    
    /* Create the event flag group.  */
    status =  tx_event_flags_create(&event_flag_memory.event_flags, "group memory");
    tx_event_flags_delete(&event_flag_memory.event_flags);
    
    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (event_flag_memory.first != 0x11223344) ||
        (event_flag_memory.second != 0x55667788) ||
        (event_flag_memory.next_to_last != 0x99aabbcc) ||
        (event_flag_memory.last != 0xddeeff00))
    {
    
        /* Event flag error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Event flags should be created now.  */

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Try to create with a NULL pointer.  */
    status =  tx_event_flags_create(TX_NULL, "group 0");
 
    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
    
    /* Try to create with a bad size.  */
    status =  _txe_event_flags_create(&group_3, "group 3", (sizeof(TX_EVENT_FLAGS_GROUP)+1));
 
    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }
    
    /* Try to create an already created group.  */
    status =  tx_event_flags_create(&group_0, "group 0");
 
    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Delete with a NULL pointer.  */
    status =  tx_event_flags_delete(TX_NULL);

    /* Check the status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }
    
    /* Delete with a non-created pointer.  */
    group_2.tx_event_flags_group_id =  0;
    status =  tx_event_flags_delete(&group_2);

    /* Check the status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Get events with a null group pointer.  */
    status =  tx_event_flags_get(TX_NULL, 0x80008000, TX_AND, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Get events with a non-created group pointer.  */
    group_2.tx_event_flags_group_id =  0;
    status =  tx_event_flags_get(&group_2, 0x80008000, TX_AND, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Get events with a null flags retun pointer.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND, TX_NULL, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Get events with a bad option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, 14, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_OPTION_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Set evetns with a NULL group pointer.   */
    status =  tx_event_flags_set(TX_NULL, 0x80008000, TX_OR);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Set events with a non-created group pointer.   */
    group_2.tx_event_flags_group_id =  0;
    status =  tx_event_flags_set(&group_2, 0x80008000, TX_OR);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Set events with a bad option.   */
    status =  tx_event_flags_set(&group_0, 0x80008000, 14);

    /* Check status.  */
    if (status != TX_OPTION_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Attempt to register the event set notify function with a NULL group pointer.  */
    status =  tx_event_flags_set_notify(TX_NULL, event_set_notify);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to register the event set notify function with a non-created group pointer.  */
    group_2.tx_event_flags_group_id =  0;
    status =  tx_event_flags_set_notify(&group_2, event_set_notify);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }
#endif

    /* Attempt to get events from an empty event flag group.  AND option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Attempt to get events from an empty event flag group.  OR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }
    
    /* Attempt to get events from an empty event flag group.  AND CLEAR option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Attempt to get events from an empty event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Set the necessary events.  */
    status =  tx_event_flags_set(&group_0, 0x80008000, TX_OR);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Just for fun, clear bit 15.  */
    status =  tx_event_flags_set(&group_0, 0x80000000, TX_AND);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Set bit 15 again.  */
    status =  tx_event_flags_set(&group_0, 0x00008000, TX_OR);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Now attemp to retrieve events...  */

    /* Attempt to get events from event flag group.  AND option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Attempt to get events from event flag group.  OR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }
    
    /* Attempt to get events from event flag group.  AND CLEAR option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Attempt to get events from an empty event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Put event flags back in the group.  */

    /* Set the necessary events.  */
    status =  tx_event_flags_set(&group_0, 0x80008000, TX_OR);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Attempt to get events from event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x00008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Attempt to get events from event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80000000UL))
    {

        /* Event flag error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }


#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Now resume thread 1.  */
    tx_thread_resume(&thread_1);

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Block memory error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }

#endif

    /* Delete both event flag groups.  */
    status =  tx_event_flags_delete(&group_0);

    /* Check the status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }

    status =  tx_event_flags_delete(&group_1);

    /* Check the status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #38\n");
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

    while(1)
    {

        tx_thread_relinquish();    
    }
}

