/* This test is designed to test queue full and empty suspension with timeouts on queues
   that supports 3 messages that are each 2 ULONG in size.  */

#include   <stdio.h>
#include   "tx_api.h"

//static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;

static TX_THREAD       thread_1;

static TX_THREAD       thread_2;
static unsigned long   thread_2_counter =  0;

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
void    threadx_queue_suspension_timeout_application_define(void *first_unused_memory)
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

        printf("Running Queue Suspension Timeout Test............................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Suspension Timeout Test............................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Suspension Timeout Test............................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create the queues.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Suspension Timeout Test............................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_queue_create(&queue_1, "queue 1", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Suspension Timeout Test............................... ERROR #5\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Suspension Timeout Test............................... ERROR #6\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Suspension Timeout Test............................... ERROR #7\n");
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
    printf("Running Queue Suspension Timeout Test............................... ");

    /* Fill the queue with an initial 3 messages!  */
    status =  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    source_message[0]++;
    status +=  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    source_message[0]++;
    status +=  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    source_message[0]++;

    /* Check status and run count of other thread.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0))
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Send message to the front of the queue that should cause this thread to suspend.  The timeout
       should cause it to resume with a TX_QUEUE_FULL error code.  */
    status =  tx_queue_front_send(&queue_0, &source_message[0], 3);

    if ((status != TX_QUEUE_FULL) || (thread_1_counter != 0) || (thread_2_counter != 0))
    {

        /* Queue error.  */
        printf("ERROR #9a\n");
        test_control_return(1);
    }
    
    /* Send message that should cause this thread to suspend.  The timeout
       should cause it to resume with a TX_QUEUE_FULL error code.  */
    status =  tx_queue_send(&queue_0, &source_message[0], 32);

    if ((status != TX_QUEUE_FULL) || (thread_1_counter != 1) || (thread_2_counter != 1))
    {

        /* Queue error.  */
        printf("ERROR #9\n");
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


        /* Receive message from empty queue with suspension and timeout.  
           We should wakeup after the timeout expires with an empty status.  */
        status =  tx_queue_receive(&queue_1, &dest_message[0], 20);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{
UINT    status;
ULONG   dest_message[2];


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue with suspension and timeout.  
           We should wakeup after the timeout expires with an empty status.  */
        status =  tx_queue_receive(&queue_1, &dest_message[0], 20);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_2_counter++;
    }
}
