/* This test is designed to test thread terminate when the terminated thread is suspeded
   on a queue.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static TX_QUEUE        queue_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


static void    queue_notify(TX_QUEUE *queue_ptr)
{

}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_thread_terminate_application_define(void *first_unused_memory)
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

        printf("Running Queue Thread Terminate Test................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Thread Terminate Test................................. ERROR #2\n");
        test_control_return(1);
    }

    /* Create the queue.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Thread Terminate Test................................. ERROR #3\n");
        test_control_return(1);
    }
    
    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Thread Terminate Test................................. ERROR #4\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Thread Terminate Test................................. ERROR #5\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Queue Thread Terminate Test................................. ");

    /* Increment the thread counter.  */
    thread_0_counter++;

    /* Make sure thread 1 is suspended on the queue.  */
    if (thread_1.tx_thread_state != TX_QUEUE_SUSP)
    {

        /* Queue error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Terminate thread 1 which is suspended on queue 0.  */
    status =  tx_thread_terminate(&thread_1);

    /* Check status and make sure thread 1 is terminated.  */
    if ((status != TX_SUCCESS) || (thread_1.tx_thread_state != TX_TERMINATED) ||
        (thread_1_counter != 0) || (queue_0.tx_queue_suspended_count))
    {

        /* Queue error.  */
        printf("ERROR #7\n");
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
ULONG   dest_message[2];


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message[0], TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}

