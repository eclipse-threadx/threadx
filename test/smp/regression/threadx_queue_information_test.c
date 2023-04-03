/* This test is designed to test the queue information services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_queue.h"


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static TX_QUEUE        queue_0;
static TX_QUEUE        queue_1;
static TX_QUEUE        queue_2;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


UINT  _tx_queue_performance_info_get(TX_QUEUE *queue_ptr, ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts);

/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_information_application_define(void *first_unused_memory)
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
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Information Test...................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create the queues.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, sizeof(ULONG));
    pointer = pointer + sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Information Test...................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_queue_create(&queue_1, "queue 1", TX_1_ULONG, pointer, 3*sizeof(ULONG));
    pointer = pointer + 3*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Information Test...................................... ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT        status;
ULONG       source_message = 0x12345678UL;
ULONG       dest_message;
ULONG       expected_message;
CHAR        *name;
ULONG       enqueued;
ULONG       available_storage;
TX_THREAD   *first_suspended;
ULONG       suspended_count;
TX_QUEUE    *next_queue;
ULONG       messages_sent;
ULONG       messages_received;
ULONG       empty_suspensions;
ULONG       full_suspensions;
ULONG       full_errors;
ULONG       timeouts;


    /* Inform user.  */
    printf("Running Queue Information Test...................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to get info from a non-queue.  */
    status =  tx_queue_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #\n");
        test_control_return(1);
    }

    /* Attempt to get info from a non-created queue.  */
    queue_2.tx_queue_id =  0;
    status =  tx_queue_info_get(&queue_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }
#endif

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */
    source_message++;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Now we need to do the same thing with the queue with three entries.  */

    source_message++;
    expected_message =  source_message;

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
        
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Get queue information.  */
    status =  tx_queue_info_get(&queue_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_queue_info_get(&queue_0, &name, &enqueued, &available_storage, &first_suspended, &suspended_count, &next_queue);

    /* Check for errors.  */    
    if ((status != TX_SUCCESS) || (enqueued != queue_0.tx_queue_enqueued) || (available_storage != queue_0.tx_queue_available_storage) || 
        (first_suspended != queue_0.tx_queue_suspension_list) || (suspended_count != queue_0.tx_queue_suspended_count) || 
        (next_queue != queue_0.tx_queue_created_next))
    {

        /* Queue error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

#ifdef TX_QUEUE_ENABLE_PERFORMANCE_INFO

    /* Test null pointer for queue performance info get.  */
    status =  _tx_queue_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Should be an error!  */ 
    if (status != TX_PTR_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(&queue_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_queue_performance_info_get(&queue_0, &messages_sent, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Check for errors.  */    
    if ((status != TX_SUCCESS) || (messages_sent != queue_0.tx_queue_performance_messages_sent_count) || (messages_received != queue_0.tx_queue_performance_messages_received_count) || 
        (empty_suspensions != queue_0.tx_queue_performance_empty_suspension_count) || (full_suspensions != queue_0.tx_queue_performance_full_suspension_count) || 
        (full_errors != queue_0.tx_queue_performance_full_error_count) || (timeouts != queue_0.tx_queue_performance_timeout_count))
    {

        /* Queue error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_queue_performance_system_info_get(&messages_sent, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Check for errors.  */    
    if ((status != TX_SUCCESS) || (messages_sent != _tx_queue_performance_messages_sent_count) || (messages_received != _tx_queue_performance__messages_received_count) || 
        (empty_suspensions != _tx_queue_performance_empty_suspension_count) || (full_suspensions != _tx_queue_performance_full_suspension_count) || 
        (full_errors != _tx_queue_performance_full_error_count) || (timeouts != _tx_queue_performance_timeout_count))
    {

        /* Queue error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

#else

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(&queue_0, &messages_sent, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, &messages_sent, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, TX_NULL, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Get performance information.  */
    status =  tx_queue_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(&messages_sent, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, &messages_received, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #38\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, TX_NULL, &empty_suspensions, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #39\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &full_suspensions, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #40\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &full_errors, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #41\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #42\n");
        test_control_return(1);
    }

    /* Get system performance information.  */
    status =  tx_queue_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Should be an error!  */ 
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Queue error.  */
        printf("ERROR #43\n");
        test_control_return(1);
    }

#endif

    /* Delete the queues.  */
    status =  tx_queue_delete(&queue_1);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #44\n");
        test_control_return(1);
    }

    status =  tx_queue_delete(&queue_0);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #45\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}
