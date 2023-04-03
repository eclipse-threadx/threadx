/* This test is designed to test queue full suspension of queue that supports 3 messages
   that are each 2 ULONG in size.  */

#include   <stdio.h>
#include   "tx_api.h"

static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static TX_QUEUE        queue_0;


static unsigned long   thread_1a_counter =  0;
static TX_THREAD       thread_1a;

static unsigned long   thread_2a_counter =  0;
static TX_THREAD       thread_2a;

static TX_QUEUE        queue_0a;


static ULONG           queue_area[3];


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_1a_entry(ULONG thread_input);
static void    thread_2a_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void    queue_notify(TX_QUEUE *queue_ptr)
{

}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_full_suspension_application_define(void *first_unused_memory)
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

        printf("Running Queue Full Suspension Test.................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_1a, "thread 1a", thread_1a_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Full Suspension Test.................................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_2a, "thread 2a", thread_2a_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Full Suspension Test.................................. ERROR #3\n");
        test_control_return(1);
    }

    /* Create the queue.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);
    status +=  tx_queue_create(&queue_0a, "queue 0a", TX_1_ULONG, pointer, 3*1*sizeof(ULONG));
    pointer = pointer + 3*1*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Full Suspension Test.................................. ERROR #4\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Full Suspension Test.................................. ERROR #5\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Full Suspension Test.................................. ERROR #6\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   source_message[2] = {0x12345678, 0};
ULONG   dest_message[2];


    /* Inform user.  */
    printf("Running Queue Full Suspension Test.................................. ");
    
    /* Perform the one word queue version.  */

    /* Suspend to get thread 1a to pend on the queue.  */
    tx_thread_suspend(&thread_0);
    status =  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);
    tx_thread_suspend(&thread_0);
    tx_thread_resume(&thread_1a);
    tx_thread_resume(&thread_2a);
    tx_queue_delete(&queue_0a);
    status +=  tx_queue_create(&queue_0a, "queue 0a", TX_1_ULONG, queue_area, sizeof(queue_area));

    
    /* Fill the queue with an initial 3 messages!  */
    status +=  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);
    status +=  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);
    status +=  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);
    source_message[0]++;

    /* Receive two of the messages back to put the first received message at the end 
       of the queue.  */
    status += tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);
    status += tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);
    status +=  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);
    source_message[0]++;
    status +=  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);
    source_message[0]++;

    /* Check status and run count of other thread.  */
    if ((status != TX_SUCCESS) || (thread_1a_counter != 0))
    {

        /* Queue error.  */
        printf("ERROR #6a\n");
        test_control_return(1);
    }

    /* Send message that should cause this thread to suspend, until the
       lower priority thread receives a message.  */
    status =  tx_queue_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);

    /* Check status and run count of other thread - it should have got the
       message already even though its counter is still 0 (it was preempted
       in the queue receive call.  */
    if ((status != TX_SUCCESS) || (thread_1a_counter != 5) || (thread_2a_counter != 1))
    {

        /* Queue error.  */
        printf("ERROR #7a\n");
        test_control_return(1);
    }

    /* Perform the two word queue version.  */ 
    
    /* Reset the source message.  */
    source_message[0] = 0x12345678;
    source_message[1] = 0;   
    
    /* Resume threads 1 and 2.  */
    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);

    /* Fill the queue with an initial 3 messages!  */
    status =  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    status += tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    status +=  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    source_message[0]++;

    /* Receive two of the messages back to put the first received message at the end 
       of the queue.  */
    status += tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);
    status += tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);
    status +=  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    source_message[0]++;
    status +=  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);
    source_message[0]++;

    /* Check status and run count of other thread.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0))
    {

        /* Queue error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Send message that should cause this thread to suspend, until the
       lower priority thread receives a message.  */
    status =  tx_queue_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);

    /* Check status and run count of other thread - it should have got the
       message already even though its counter is still 0 (it was preempted
       in the queue receive call.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 5) || (thread_2_counter != 1))
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
ULONG   expected_message[2] = {0x12345678, 0};
ULONG   dest_message[2];
UINT    old_priority;


    /* Loop forever!  */
    while(1)
    {

        /* Receive messages and suspend.  */
        status =  tx_queue_receive(&queue_0, &dest_message[0], TX_WAIT_FOREVER);

        if ((status != TX_SUCCESS) || (dest_message[0] != expected_message[0]++))
            break;

        /* Change thread 2 priority.  */
        tx_thread_priority_change(&thread_2, 15, &old_priority);

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{

ULONG   source_message[2] = {0x1234567C, 0};


    /* Send one message to the queue.  */
    tx_queue_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);

    /* Increment the thread counter.  */
    thread_2_counter++;
}


static void    thread_1a_entry(ULONG thread_input)
{
UINT    status;
ULONG   expected_message[2] = {0x12345678, 0};
ULONG   dest_message[2];
UINT    old_priority;


   /* Receive message and suspend.  */
   status = tx_thread_resume(&thread_0);
   status += tx_queue_receive(&queue_0a, &dest_message[0], TX_WAIT_FOREVER);
   status += tx_thread_resume(&thread_0);
   status += tx_thread_suspend(&thread_1a);

   if ((status != TX_SUCCESS) || (dest_message[0] != expected_message[0]))
       return;

    /* Loop forever!  */
    while(1)
    {

        /* Receive messages and suspend.  */
        status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_WAIT_FOREVER);

        if ((status != TX_SUCCESS) || (dest_message[0] != expected_message[0]++))
            break;

        /* Change thread 2a priority.  */
        tx_thread_priority_change(&thread_2a, 15, &old_priority);

        /* Increment the thread counter.  */
        thread_1a_counter++;
    }
}


static void    thread_2a_entry(ULONG thread_input)
{

ULONG   source_message[2] = {0x1234567C, 0};


    /* Send one message to the queue.  */
    tx_queue_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);

    /* Increment the thread counter.  */
    thread_2a_counter++;
}
