/* This test is designed to test the queue flush operation on a queue that has two threads
   suspended on it.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static TX_QUEUE        queue_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void    queue_notify(TX_QUEUE *queue_ptr)
{

}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_flush_application_define(void *first_unused_memory)
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

        printf("Running Queue Flush w/Suspended Threads Test........................ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush w/Suspended Threads Test........................ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush w/Suspended Threads Test........................ ERROR #3\n");
        test_control_return(1);
    }


    /* Create the queue.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush w/Suspended Threads Test........................ ERROR #4\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Flush w/Suspended Threads Test........................ ERROR #5\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Flush w/Suspended Threads Test........................ ERROR #6\n");
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
    printf("Running Queue Flush w/Suspended Threads Test........................ ");

    /* Fill up the queue.  */
    status =  tx_queue_send(&queue_0, &message[0], TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #7\n");
        test_control_return(1);
    }

    status =  tx_queue_send(&queue_0, &message[0], TX_NO_WAIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #8\n");
        test_control_return(1);
    }

    status =  tx_queue_send(&queue_0, &message[0], TX_NO_WAIT);


    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Relinquish to get other threads suspended on the queue full.  */
    tx_thread_relinquish();

    /* Flush queue 0 which has the threads suspended on it.  */
    status =  tx_queue_flush(&queue_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Relinquish to let other threads run and finish!  */
    tx_thread_relinquish();

    /* Determine if the queue flush test was successful.  */
    if ((thread_1_counter == 1) && (thread_2_counter == 1))
    {

        /* Successful queue flush test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
    else
    {
    
        /* Queue Flush error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Increment the thread counter.  */
    thread_0_counter++;
}


static void    thread_1_entry(ULONG thread_input)
{
UINT    status;
ULONG   message[2] = {0x1, 0};


    /* Receive message from empty queue.  */
    status =  tx_queue_send(&queue_0, &message[0], TX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
        return;

    /* Increment the thread counter.  */
    thread_1_counter++;
}


static void    thread_2_entry(ULONG thread_input)
{
UINT    status;
ULONG   message[2] = {0x2, 0};


    /* Receive message from empty queue.  */
    status =  tx_queue_send(&queue_0, &message[0], TX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
        return;

    /* Increment the thread counter.  */
    thread_2_counter++;
}

