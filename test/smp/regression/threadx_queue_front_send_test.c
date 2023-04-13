/* This test is designed to test immediate response queue services including queue front send.
   This test is for queue sizes of 2 ULONG.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static TX_THREAD       thread_2;

static TX_QUEUE        queue_0;


static unsigned long   thread_1a_counter =  0;
static TX_THREAD       thread_1a;

static TX_THREAD       thread_2a;

static TX_QUEUE        queue_0a;

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
void    threadx_queue_front_send_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Front Test............................................ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status =  tx_thread_create(&thread_1a, "thread 1a", thread_1a_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Front Test............................................ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status =  tx_thread_create(&thread_2a, "thread 2a", thread_2a_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Front Test............................................ ERROR #3\n");
        test_control_return(1);
    }

    /* Create the queues.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_2_ULONG, pointer, 2*2*sizeof(ULONG));
    pointer = pointer + 2*2*sizeof(ULONG);
    status =  tx_queue_create(&queue_0a, "queue 0a", TX_1_ULONG, pointer, 2*1*sizeof(ULONG));
    pointer = pointer + 2*1*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Front Test............................................ ERROR #4\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Front Test............................................ ERROR #5\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Front Test............................................ ERROR #6\n");
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
ULONG   temp[2];


    /* Inform user.  */
    printf("Running Queue Front Test............................................ ");

    /* Perform the 1 word queue front send test.  */
    
    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Place something on queue 0a.  */
    status =  tx_queue_send(&queue_0a, &source_message[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #7a\n");
        test_control_return(1);
    }
    
    /* Place a new message on the front of the queue.  */
    temp[0] =  0xF000001;
    status =  tx_queue_front_send(&queue_0a, &temp[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #8a\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0a.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (dest_message[0] != temp[0]))
    {

        /* Queue error.  */
        printf("ERROR #9a\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0a.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (dest_message[0] != source_message[0]))
    {

        /* Queue error.  */
        printf("ERROR #10a\n");
        test_control_return(1);
    }

    /* Attempt to receive another message from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #11a\n");
        test_control_return(1);
    }
    
    /* At this point the queue is empty. Resume another thread to 
       suspend on an empty queue.  */
    tx_thread_resume(&thread_1a);
    
    /* Relinquish to get this thread suspended on the empty queue.  */
    tx_thread_relinquish();
    
    /* Resume thread 2a to get another thread suspended on the empty queue.  */
    tx_thread_resume(&thread_2a);
    
    /* Now send something to the front of the queue, which will resume
       the first waiting thread.  */
    temp[0] =  0xFF00002;
    status =  tx_queue_front_send(&queue_0a, &temp[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #12a\n");
        test_control_return(1);
    }

    /* Now send something to the front of the queue, which will resume
       the second waiting thread.  */
    temp[0] =  0xFF00002;
    status =  tx_queue_front_send(&queue_0a, &temp[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #13a\n");
        test_control_return(1);
    }
    
    /* Now relinquish again to let the other thread process the message.  */
    tx_thread_relinquish();
    
    /* At this point, the other thread should have placed 2 messages on the queue
       so we will now send to the front, but without suspension.  */
    temp[0] =  0xFF00003;
    status =  tx_queue_front_send(&queue_0a, &temp[0], TX_NO_WAIT);

    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #14a\n");
        test_control_return(1);
    }

    /* Now, we will now send to the front, but with suspension.  */
    temp[0] =  0xFF00003;
    status =  tx_queue_front_send(&queue_0a, &temp[0], TX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #15a\n");
        test_control_return(1);
    }

    /* Now resume thread 2a to get another thread suspended on the queue.  */
    tx_thread_resume(&thread_2a);
    
    temp[0] =  0xFF00004;
    status =  tx_queue_front_send(&queue_0a, &temp[0], TX_WAIT_FOREVER);
    
    /* When we get back, the other thread has received all the messages and 
       verified they are in order AND relinquished.  */
    if ((status != TX_SUCCESS) || (thread_1a_counter != 1))
    {

        /* Queue error.  */
        printf("ERROR #16a\n");
        test_control_return(1);
    }

   
    /* Perform the multiword queue front send test.  */


    /* Increment thread 0 counter.  */
    thread_0_counter =  1;

    /* Reset the source message.  */
    source_message[0] =  0x12345678;
    source_message[1] =  0;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, &source_message[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
    
    /* Place a new message on the front of the queue.  */
    temp[0] =  0xF000001;
    status =  tx_queue_front_send(&queue_0, &temp[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (dest_message[0] != temp[0]))
    {

        /* Queue error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (dest_message[0] != source_message[0]))
    {

        /* Queue error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to receive another message from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }
    
    /* At this point the queue is empty. Resume another thread to 
       suspend on an empty queue.  */
    tx_thread_resume(&thread_1);
    
    /* Relinquish to get this thread suspended on the empty queue.  */
    tx_thread_relinquish();
    
    /* Resume thread 2 to get another thread suspended on the empty queue.  */
    tx_thread_resume(&thread_2);
    
    /* Now send something to the front of the queue, which will resume
       the first waiting thread.  */
    temp[0] =  0xFF00002;
    status =  tx_queue_front_send(&queue_0, &temp[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Now send something to the front of the queue, which will resume
       the second waiting thread.  */
    temp[0] =  0xFF00002;
    status =  tx_queue_front_send(&queue_0, &temp[0], TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }
    
    /* Now relinquish again to let the other thread process the message.  */
    tx_thread_relinquish();
    
    /* At this point, the other thread should have placed 2 messages on the queue
       so we will now send to the front, but without suspension.  */
    temp[0] =  0xFF00003;
    status =  tx_queue_front_send(&queue_0, &temp[0], TX_NO_WAIT);

    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Now, we will now send to the front, but with suspension.  */
    temp[0] =  0xFF00003;
    status =  tx_queue_front_send(&queue_0, &temp[0], TX_WAIT_FOREVER);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Now resume thread 2 to get another thread suspended on the queue.  */
    tx_thread_resume(&thread_2);
    
    temp[0] =  0xFF00004;
    status =  tx_queue_front_send(&queue_0, &temp[0], TX_WAIT_FOREVER);
    
    /* When we get back, the other thread has received all the messages and 
       verified they are in order AND relinquished.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1))
    {

        /* Queue error.  */
        printf("ERROR #16\n");
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
ULONG   source_message[2] = {0xEE000001, 0};
ULONG   dest_message[2];


    /* First, suspend on an empty queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_WAIT_FOREVER);

    /* Determine if the message is good.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xFF00002))
        return;
        
    /* Now fill the queue with two messages.   */
    status =  tx_queue_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);
    source_message[0]++;
    status +=  tx_queue_front_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);
    
    if (status != TX_SUCCESS)
        return;

    /* Now let thread 0 send to the front of the queue with suspension.  */
    tx_thread_relinquish();
    
    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xFF00003))
        return;

    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000002))
        return;

    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000001))
        return;

    /* At this point, we are going to fill up the queue again.  */
    source_message[0]++;
    status =  tx_queue_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);
    source_message[0]++;
    status +=  tx_queue_front_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);
    
    if (status != TX_SUCCESS)
        return;
    
    /* Now let thread 0 send to the front of the queue with suspension.  */
    tx_thread_relinquish();

    /* Attempt to receive four messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xFF00004))
        return;

    /* Attempt to receive four messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xDD000001))
        return;

    /* Attempt to receive four messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000004))
        return;

    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000003))
        return;
        
    /* Increment this threads counter.  */
    thread_1_counter++;
}
    

static void    thread_2_entry(ULONG thread_input)
{

ULONG   source_message[2] = {0xDD000001, 0};
ULONG   destination_message[2];


    /* Receive message.  */
    tx_queue_receive(&queue_0, &destination_message[0], TX_WAIT_FOREVER);
    
    /* Self suspend.  */
    tx_thread_suspend(&thread_2);

    /* Send another message to the front of the queue.  */
    tx_queue_front_send(&queue_0, &source_message[0], TX_WAIT_FOREVER);
}


static void    thread_1a_entry(ULONG thread_input)
{

UINT    status;
ULONG   source_message[2] = {0xEE000001, 0};
ULONG   dest_message[2];


    /* First, suspend on an empty queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_WAIT_FOREVER);

    /* Determine if the message is good.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xFF00002))
        return;
        
    /* Now fill the queue with two messages.   */
    status =  tx_queue_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);
    source_message[0]++;
    status +=  tx_queue_front_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);
    
    if (status != TX_SUCCESS)
        return;

    /* Now let thread 0 send to the front of the queue with suspension.  */
    tx_thread_relinquish();
    
    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xFF00003))
        return;

    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000002))
        return;

    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000001))
        return;

    /* At this point, we are going to fill up the queue again.  */
    source_message[0]++;
    status =  tx_queue_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);
    source_message[0]++;
    status +=  tx_queue_front_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);
    
    if (status != TX_SUCCESS)
        return;
    
    /* Now let thread 0 send to the front of the queue with suspension.  */
    tx_thread_relinquish();

    /* Attempt to receive four messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xFF00004))
        return;

    /* Attempt to receive four messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xDD000001))
        return;

    /* Attempt to receive four messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000004))
        return;

    /* Attempt to receive three messages from the queue.  */
    status =  tx_queue_receive(&queue_0a, &dest_message[0], TX_NO_WAIT);

    /* Should be an error.  */
    if ((status != TX_SUCCESS) || (dest_message[0] != 0xEE000003))
        return;
        
    /* Increment this threads counter.  */
    thread_1a_counter++;
}
    

static void    thread_2a_entry(ULONG thread_input)
{

ULONG   source_message[2] = {0xDD000001, 0};
ULONG   destination_message[2];


    /* Receive message.  */
    tx_queue_receive(&queue_0a, &destination_message[0], TX_WAIT_FOREVER);
    
    /* Self suspend.  */
    tx_thread_suspend(&thread_2a);

    /* Send another message to the front of the queue.  */
    tx_queue_front_send(&queue_0a, &source_message[0], TX_WAIT_FOREVER);
}
