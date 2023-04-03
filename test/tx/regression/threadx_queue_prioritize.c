/* This test is designed to test queue prioritize.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Define the external reference for the preempt disable flag.  */

extern volatile UINT   _tx_thread_preempt_disable;


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static TX_THREAD       thread_2;

static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_3;

static unsigned long   thread_4_counter =  0;
static TX_THREAD       thread_4;

static unsigned long   thread_5_counter =  0;
static TX_THREAD       thread_5;

static unsigned long   thread_6_counter =  0;
static TX_THREAD       thread_6;

static TX_QUEUE        queue_0;
static TX_QUEUE        queue_1;


static int             test_status;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void    queue_notify(TX_QUEUE *queue_ptr)
{

}


/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

    /* Determine if the test case we are looking for is present.  */
    if ((_tx_thread_preempt_disable) && (test_status == 1))
    {

        /* Determine if thread 3 is at the front of the suspension list.  */
        if (queue_0.tx_queue_suspension_list == &thread_3)
        {

            /* Abort the wait of thread 3.  */
            tx_thread_wait_abort(&thread_3);
        }
        else
        {
        
            /* Abort the wait of thread 5.  */
            tx_thread_wait_abort(&thread_5);

            /* End the ISR processing.  */
            test_status =  2;    
            test_isr_dispatch =  TX_NULL;
        }
    }
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_prioritize_application_define(void *first_unused_memory)
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

        printf("Running Queue Prioritize Test....................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #3\n");
        test_control_return(1);
    }


    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 4, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #7\n");
        test_control_return(1);
    }

    /* Create the queue.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, 3*2*sizeof(ULONG));
    pointer = pointer + 3*2*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #8\n");
        test_control_return(1);
    }

    /* Setup queue send notification.  */
    status =  tx_queue_send_notify(&queue_0, queue_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #9\n");
        test_control_return(1);
    }
#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Queue Prioritize Test....................................... ERROR #10\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Queue Prioritize Test....................................... ");


#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to prioritize a non-queue.  */
    status =  tx_queue_prioritize(TX_NULL);

    /* Check for an error condition.   */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to prioritize a non-created queue.  */
    queue_1.tx_queue_id =  0;
    status =  tx_queue_prioritize(&queue_1);

    /* Check for an error condition.   */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }
#endif

    /* Nothing to do here, but check prioritization with no suspended threads.  */
    status =  tx_queue_prioritize(&queue_0);

    /* Check for an error condition.   */
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);

    /* Increment the thread counter.  */
    thread_0_counter++;

    /* Make sure thread 1 and 2 are suspended on the queue.  */
    if ((thread_1.tx_thread_state != TX_QUEUE_SUSP) || (thread_2.tx_thread_state != TX_QUEUE_SUSP) ||
        (queue_0.tx_queue_suspension_list != &thread_1))
    {

        /* Queue error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Prioritize the queue suspension list.  */
    status =  tx_queue_prioritize(&queue_0);

    /* Check status and make sure thread 2 is not at the head of the list.  */
    if ((status != TX_SUCCESS) || (queue_0.tx_queue_suspension_list != &thread_2))
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Prioritize the queue suspension list again, but slightly different path because it was previously prioritized.  */
    status =  tx_queue_prioritize(&queue_0);

    /* Check status and make sure thread 2 not at the head of the list.  */
    if ((status != TX_SUCCESS) || (queue_0.tx_queue_suspension_list != &thread_2))
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }
    
    /* At this point we are going to get more than 2 threads suspended.  */
    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);
    tx_thread_resume(&thread_3);
    tx_thread_resume(&thread_4);
    tx_thread_resume(&thread_5);
    tx_thread_resume(&thread_6);
    
    /* Prioritize the queue suspension list.  */
    status =  tx_queue_prioritize(&queue_0);

    /* Check status and make sure thread 3 is at the front of the suspension list.  */
    if ((status != TX_SUCCESS) || (queue_0.tx_queue_suspension_list != &thread_3))
    {

        /* Queue error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
    
    /* Now loop to test the interrupt of the prioritize loop logic.  */
    test_status =  1;
    test_isr_dispatch =  test_isr;  
    do
    {

        /* Prioritize the queue suspension list.  */
        status =  tx_queue_prioritize(&queue_0);

        /* Check status and make sure thread 1 is terminated.  */
        if (status != TX_SUCCESS)
        {

            /* Block Pool error.  */
            printf("ERROR #17\n");
            test_control_return(1);
        }

    } while (test_status == 1);
    
    /* Now determine if thread 4 is at the front of the list... It should be!  */
    if (queue_0.tx_queue_suspension_list != &thread_4)
    {    

        /* Queue error.  */
        printf("ERROR #18\n");
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
ULONG   dest_message;


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message, TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{
UINT    status;
ULONG   dest_message;


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message, TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_3_entry(ULONG thread_input)
{
UINT    status;
ULONG   dest_message;


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message, TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_3_counter++;
    }
}


static void    thread_4_entry(ULONG thread_input)
{
UINT    status;
ULONG   dest_message;


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message, TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_4_counter++;
    }
}


static void    thread_5_entry(ULONG thread_input)
{
UINT    status;
ULONG   dest_message;


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message, TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_5_counter++;
    }
}


static void    thread_6_entry(ULONG thread_input)
{
UINT    status;
ULONG   dest_message;


    /* Loop forever!  */
    while(1)
    {


        /* Receive message from empty queue.  */
        status =  tx_queue_receive(&queue_0, &dest_message, TX_WAIT_FOREVER);

        if (status != TX_QUEUE_EMPTY)
            break;

        /* Increment the thread counter.  */
        thread_6_counter++;
    }
}


