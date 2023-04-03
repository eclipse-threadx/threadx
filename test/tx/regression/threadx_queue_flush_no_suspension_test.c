/* This test is designed to test the queue flush operation on a queue that has no threads
   suspended on it.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static TX_QUEUE        queue_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void    queue_notify(TX_QUEUE *queue_ptr)
{

}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_flush_no_suspension_application_define(void *first_unused_memory)
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

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush No Suspension Test.............................. ERROR #1\n");
        test_control_return(1);
    }

    /* Create the queue.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush No Suspension Test.............................. ERROR #2\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush No Suspension Test.............................. ERROR #3\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Flush No Suspension Test.............................. ERROR #4\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

ULONG   message[2] =  {0x12345678, 0};
UINT    status;


    /* Inform user.  */
    printf("Running Queue Flush No Suspension Test.............................. ");

    /* Fill up the queue.  */
    status =  tx_queue_send(&queue_0, &message[0], TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_queue_send(&queue_0, &message[0], TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_queue_send(&queue_0, &message[0], TX_NO_WAIT);


    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Flush queue 0 to make more room.  */
    status =  tx_queue_flush(&queue_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #8\n");
        test_control_return(1);
    }


    /* Fill up the queue.  */
    status =  tx_queue_send(&queue_0, &message[2], TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #9\n");
        test_control_return(1);
    }

    status =  tx_queue_send(&queue_0, &message[2], TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #10\n");
        test_control_return(1);
    }

    status =  tx_queue_send(&queue_0, &message[2], TX_NO_WAIT);


    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Flush queue again to empty and then flush to test on an empty queue.  */
    status =  tx_queue_flush(&queue_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #12\n");
        test_control_return(1);
    }

    status =  tx_queue_flush(&queue_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Success if we get here.  */
    printf("SUCCESS!\n");
    test_control_return(0);

    /* Increment the thread counter.  */
    thread_0_counter++;
}

