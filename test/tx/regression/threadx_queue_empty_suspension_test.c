/* This test is designed to test empty queue suspension of queue that supports 3 messages
   that are each 2 ULONG in size.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static TX_QUEUE        queue_0;
static TX_QUEUE        queue_1;


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
void    threadx_queue_empty_suspension_application_define(void *first_unused_memory)
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

        printf("Running Queue Empty Suspension Test................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Empty Suspension Test................................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Empty Suspension Test................................. ERROR #3\n");
        test_control_return(1);
    }

    /* Create the queue.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Empty Suspension Test................................. ERROR #4\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);
    
#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Empty Suspension Test................................. ERROR #5\n");
        test_control_return(1);
    }
    
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Empty Suspension Test................................. ERROR #6\n");
        test_control_return(1);
    }

#endif 
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   source_message[2] = {0x12345678, 0};


    /* Inform user.  */
    printf("Running Queue Empty Suspension Test................................. ");

    /* Increment the thread counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to setup notify on a non-queue.  */
    status =  tx_queue_send_notify(TX_NULL, queue_notify);

    /* Check status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to setup notify on a non-created queue.  */
    queue_1.tx_queue_id =  0;
    status =  tx_queue_send_notify(&queue_1, queue_notify);

    /* Check status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
#endif

    /* Send message that should go directly into the the other thread's
       destination area!  We should also preempt and the queue should
       be empty by the next time we get around!  */
    status =  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);

    /* Check status and run count of other thread - it should have got the
       message already.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1))
    {

        /* Queue error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Now resume thread 2 to get another thread suspended on an empty queue.  */
    tx_thread_resume(&thread_2);
    
    /* Now send 2 messages to wakeup both threads!  */
    source_message[0]++;
    status =  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    status += tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    
    /* Check status and run count of other thread - it should have got the
       message already.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 2) || (thread_2_counter != 1))
    {

        /* Queue error.  */
        printf("ERROR #10\n");
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
ULONG   expected_message[2] = {0x12345678, 0};
ULONG   dest_message[2];


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue with suspension.  We should always
           suspend until thread 0 executes.  */
        status =  tx_queue_receive(&queue_0, &dest_message[0], TX_WAIT_FOREVER);

        if ((status != TX_SUCCESS) || (dest_message[0] != expected_message[0]++))
            break;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;
ULONG   expected_message[2] = {0x12345679, 0};
ULONG   dest_message[2];


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue with suspension.  We should always
           suspend until thread 0 executes.  */
        status =  tx_queue_receive(&queue_0, &dest_message[0], TX_WAIT_FOREVER);

        if ((status != TX_SUCCESS) || (dest_message[0] != expected_message[0]++))
            break;

        /* Increment the thread counter.  */
        thread_2_counter++;
    }
}

