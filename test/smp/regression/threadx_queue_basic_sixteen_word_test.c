/* This test is designed to test immediate response queue services including create
   and delete.  This test is for queue sizes of 16 ULONG.  Two queues are used one with 
   a capacity of 1 message and another with a capacity of 3 messages.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static TX_QUEUE        queue_0;
static TX_QUEUE        queue_1;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_basic_sixteen_word_application_define(void *first_unused_memory)
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

        printf("Running Queue Sixteen Word Test..................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create the queues.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_16_ULONG, pointer, 16*sizeof(ULONG));
    pointer = pointer + (16*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Sixteen Word Test..................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_queue_create(&queue_1, "queue 1", TX_16_ULONG, pointer, 16*3*sizeof(ULONG));
    pointer = pointer + 16*3*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Sixteen Word Test..................................... ERROR #3\n");
        test_control_return(1);
    }
}





/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   source_message[16];
ULONG   dest_message[16];
ULONG   expected_message[16];


    /* Inform user.  */
    printf("Running Queue Sixteen Word Test..................................... ");

    source_message[0] = 0x01234567;
    source_message[15] = 0x89ABCDEF;

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message[0] != dest_message[0]) ||
                                  (source_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */
    source_message[0]++;
    source_message[15]++;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message[0] != dest_message[0]) ||
                                  (source_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Now we need to do the same thing with the queue with three entries.  */

    source_message[0]++;
    source_message[15]++;
    expected_message[0] =  source_message[0];
    expected_message[15] =  source_message[15];

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[15]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[15]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[15]++;
        

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[15]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[15]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[15]++;

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[15]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[15]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[15]++;

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[15]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[15]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[15] != dest_message[15]))
    {

        /* Queue error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[15]++;

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Delete the queues.  */
    status =  tx_queue_delete(&queue_1);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    status =  tx_queue_delete(&queue_0);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


