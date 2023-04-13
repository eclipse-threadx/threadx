/* This test is designed to test immediate response queue services including create
   and delete.  This test is for queue sizes of 2 ULONG.  Two queues are used one with 
   a capacity of 1 message and another with a capacity of 3 messages.  */

#include   <stdio.h>
#include   "tx_api.h"

typedef struct QUEUE_MEMORY_TEST_STRUCT
{
    ULONG       first;
    ULONG       second;
    TX_QUEUE    queue;
    ULONG       first_middle;
    ULONG       second_middle;
    ULONG       queue_area[2048/sizeof(ULONG)];
    ULONG       next_to_last;
    ULONG       last;

} QUEUE_MEMORY_TEST;

static  QUEUE_MEMORY_TEST   queue_memory;

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
void    threadx_queue_basic_two_word_application_define(void *first_unused_memory)
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

        printf("Running Queue Two Word Test......................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create the queues.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 2*sizeof(ULONG));
    pointer = pointer + (2*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Two Word Test......................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_queue_create(&queue_1, "queue 1", TX_2_ULONG, pointer, 2*3*sizeof(ULONG));
    pointer = pointer + 2*3*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Two Word Test......................................... ERROR #3\n");
        test_control_return(1);
    }
}





/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   source_message[2];
ULONG   dest_message[2];
ULONG   expected_message[2];


    /* Inform user.  */
    printf("Running Queue Two Word Test......................................... ");

    /* Perform queue memory test.  */
    queue_memory.first =        0x11223344;
    queue_memory.second =       0x55667788;
    queue_memory.first_middle = 0x21314151;
    queue_memory.second_middle= 0x61718191;
    queue_memory.next_to_last = 0x99aabbcc;
    queue_memory.last =         0xddeeff00;
    
    /* Create the queue.  */
    status =  tx_queue_create(&queue_memory.queue, "queue memory", TX_2_ULONG, &queue_memory.queue_area[0], (2048*sizeof(ULONG))/sizeof(ULONG));
    tx_queue_delete(&queue_memory.queue);

    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (queue_memory.first != 0x11223344) ||
        (queue_memory.second != 0x55667788) ||
        (queue_memory.first_middle != 0x21314151) ||
        (queue_memory.second_middle != 0x61718191) ||
        (queue_memory.next_to_last != 0x99aabbcc) ||
        (queue_memory.last != 0xddeeff00))
    {
        /* Queue error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    source_message[0] = 0x01234567;
    source_message[1] = 0x89ABCDEF;

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message[0] != dest_message[0]) ||
                                  (source_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */
    source_message[0]++;
    source_message[1]++;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message[0] != dest_message[0]) ||
                                  (source_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Now we need to do the same thing with the queue with three entries.  */

    source_message[0]++;
    source_message[1]++;
    expected_message[0] =  source_message[0];
    expected_message[1] =  source_message[1];

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[1]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[1]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[1]++;
        

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[1]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[1]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[1]++;

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[1]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[1]++;
    status +=  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);
    source_message[0]++;
    source_message[1]++;

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[1]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[1]++;

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message[0] != dest_message[0]) ||
                                  (expected_message[1] != dest_message[1]))
    {

        /* Queue error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    expected_message[0]++;
    expected_message[1]++;

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Delete the queues.  */
    status =  tx_queue_delete(&queue_1);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    status =  tx_queue_delete(&queue_0);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


