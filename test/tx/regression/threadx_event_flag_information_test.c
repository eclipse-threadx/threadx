/* This test is designed to test the event flag group information gathering services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_event_flags.h"


static TX_THREAD       thread_0;


static TX_EVENT_FLAGS_GROUP group_0;
static TX_EVENT_FLAGS_GROUP group_1;
static TX_EVENT_FLAGS_GROUP group_2;



/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


UINT  _tx_event_flags_performance_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG *sets, ULONG *gets,
                    ULONG *suspensions, ULONG *timeouts);


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
void    threadx_event_flag_information_application_define(void *first_unused_memory)
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

        printf("Running Event Flag Information Test................................. ERROR #1\n");
        test_control_return(1);
    }

     /* Create event flag group 0 and 1.  */
    status =  tx_event_flags_create(&group_0, "group 0");
 
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Information Test................................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_event_flags_create(&group_1, "group 1");

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Information Test................................. ERROR #3\n");
        test_control_return(1);
    }
    
    /* Register the event set notify function.  */
    status =  tx_event_flags_set_notify(&group_0, event_set_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Information Test................................. ERROR #4\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Event Flag Information Test................................. ERROR #5\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT                    status;
ULONG                   actual_events;
CHAR                    *name;
ULONG                   current_flags;
TX_THREAD               *first_suspended;
ULONG                   suspended_count;
TX_EVENT_FLAGS_GROUP    *next_group;
ULONG                   sets;
ULONG                   gets;
ULONG                   suspensions;
ULONG                   timeouts;


    /* Inform user.  */
    printf("Running Event Flag Information Test................................. ");

    /* Event flags should be created now.  */

    /* Attempt to get events from an empty event flag group.  AND option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to get events from an empty event flag group.  OR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
    
    /* Attempt to get events from an empty event flag group.  AND CLEAR option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to get events from an empty event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Set the necessary events.  */
    status =  tx_event_flags_set(&group_0, 0x80008000, TX_OR);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Just for fun, clear bit 15.  */
    status =  tx_event_flags_set(&group_0, 0x80000000, TX_AND);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Set bit 15 again.  */
    status =  tx_event_flags_set(&group_0, 0x00008000, TX_OR);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Now attemp to retrieve events...  */

    /* Attempt to get events from event flag group.  AND option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to get events from event flag group.  OR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }
    
    /* Attempt to get events from event flag group.  AND CLEAR option.  */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_AND_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to get events from an empty event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_NO_EVENTS)
    {

        /* Event flag error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Put event flags back in the group.  */

    /* Set the necessary events.  */
    status =  tx_event_flags_set(&group_0, 0x80008000, TX_OR);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Event flag error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to get events from event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x00008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80008000UL))
    {

        /* Event flag error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Attempt to get events from event flag group.  OR CLEAR option. */
    status =  tx_event_flags_get(&group_0, 0x80008000, TX_OR_CLEAR, &actual_events, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (actual_events != 0x80000000UL))
    {

        /* Event flag error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Get information with a NULL pointer.  */
    status =  tx_event_flags_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Get information from a non-created group.  */
    group_2.tx_event_flags_group_id =  0;
    status =  tx_event_flags_info_get(&group_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check status.  */
    if (status != TX_GROUP_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

#endif

    /* Get information about the event flag group.  */
    status =  tx_event_flags_info_get(&group_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_event_flags_info_get(&group_0, &name, &current_flags, &first_suspended, &suspended_count, &next_group);
    
    /* Check the status.  */
    if ((status != TX_SUCCESS) || (current_flags != group_0.tx_event_flags_group_current) || (first_suspended != TX_NULL) || (suspended_count != 0) || (next_group != &group_1))
    {

        /* Event flag error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

#ifdef TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO

    /* Get performance information with NULL pointer.  */
    status =  _tx_event_flags_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check the status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Event flag error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(&group_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_event_flags_performance_info_get(&group_0, &sets, &gets, &suspensions, &timeouts);
    
    /* Check the status.  */
    if ((status != TX_SUCCESS) || (sets != group_0.tx_event_flags_group_performance_set_count) || (gets != group_0.tx_event_flags_group__performance_get_count) || 
        (suspensions != group_0.tx_event_flags_group___performance_suspension_count) || (timeouts != group_0.tx_event_flags_group____performance_timeout_count))
    {

        /* Event flag error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Get system performance information on all event flags groups.  */
    status =  tx_event_flags_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_event_flags_performance_system_info_get(&sets, &gets, &suspensions, &timeouts);
    
    /* Check the status.  */
    if ((status != TX_SUCCESS) || (sets != _tx_event_flags_performance_set_count) || (gets != _tx_event_flags_performance_get_count) || 
        (suspensions != _tx_event_flags_performance_suspension_count) || (timeouts != _tx_event_flags_performance_timeout_count))
    {

        /* Event flag error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

#else

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(&group_0, &sets, &gets, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(TX_NULL, &sets, &gets, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(TX_NULL, TX_NULL, &gets, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Get performance information on the event flag group.  */
    status =  tx_event_flags_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Get system performance information on all event flags groups.  */
    status =  tx_event_flags_performance_system_info_get(&sets, &gets, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Get system performance information on all event flags groups.  */
    status =  tx_event_flags_performance_system_info_get(TX_NULL, &gets, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Get system performance information on all event flags groups.  */
    status =  tx_event_flags_performance_system_info_get(TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Get system performance information on all event flags groups.  */
    status =  tx_event_flags_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Get system performance information on all event flags groups.  */
    status =  tx_event_flags_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Event flag error.  */
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
