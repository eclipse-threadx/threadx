/* This test is designed to test immediate response queue services including create
   and delete.  This test is for queue sizes of 1 ULONG.  Two queues are used one with 
   a capacity of 1 message and another with a capacity of 3 messages.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;


static TX_TIMER        timer_0;


static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;

static TX_QUEUE        queue_0;
static TX_QUEUE        queue_1;
static TX_QUEUE        queue_2;
static TX_QUEUE        queue_3;


/* Define the external reference to the status for queue create from initialization.  */

extern UINT            test_queue_from_init;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


UINT        _txe_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, 
                        VOID *queue_start, ULONG queue_size, UINT queue_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;
CHAR    *pointer;
ULONG   source = 1234;
ULONG   destination;


    /* Determine if calling queue create from initialization was successful.  */
    if (test_queue_from_init != TX_SUCCESS)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create a queue from a timer.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_queue_create(&queue_2, "queue 2", TX_1_ULONG, pointer, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a queue from a timer.  */
    status = tx_queue_delete(&queue_0);
    
    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to send something with suspension from a timer.  */
    status =  tx_queue_front_send(&queue_0, &source, 100);
    
    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to send something with suspension from a timer.  */
    status =  tx_queue_send(&queue_0, &source, 100);
    
    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to receive something with suspension from a timer.  */
    status =  tx_queue_receive(&queue_0, &destination, 100);
    
    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }


    timer_executed =  1;
#endif
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

CHAR    *pointer;
UINT    status;
ULONG   source = 1234;
ULONG   destination;


    /* Attempt to create a queue from an ISR.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_queue_create(&queue_2, "queue 2", TX_1_ULONG, pointer, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a queue from an ISR.  */
    status = tx_queue_delete(&queue_0);
    
    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to send something with suspension from an ISR.  */
    status =  tx_queue_front_send(&queue_0, &source, 100);
    
    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to send something with suspension from an ISR.  */
    status =  tx_queue_send(&queue_0, &source, 100);
    
    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to receive something with suspension from an ISR.  */
    status =  tx_queue_receive(&queue_0, &destination, 100);
    
    /* Check for status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    isr_executed =  1;
#endif
}



/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_queue_basic_application_define(void *first_unused_memory)
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

    status += tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;


    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue One Word Queue Test................................... ERROR #1\n");
        test_control_return(1);
    }

    /* Create the queues.  */
    status =  tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, sizeof(ULONG));
    pointer = pointer + sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue One Word Queue Test................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_queue_create(&queue_1, "queue 1", TX_1_ULONG, pointer, 3*sizeof(ULONG));
    pointer = pointer + 3*sizeof(ULONG);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Queue One Word Queue Test................................... ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   source_message = 0x12345678UL;
ULONG   dest_message;
ULONG   expected_message;
#ifndef TX_DISABLE_ERROR_CHECKING
CHAR    *pointer;
#endif

    /* Inform user.  */
    printf("Running Queue One Word Queue Test................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to create with NULL queue.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_queue_create(TX_NULL, "queue 1", TX_1_ULONG, pointer, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Attempt to create with bad sized queue control block.  */
    status =  _txe_queue_create(&queue_3, "queue 3", TX_1_ULONG, pointer, 3*sizeof(ULONG), (sizeof(TX_QUEUE)+1));

    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to create with bad sized message.  */
    status =  tx_queue_create(&queue_3, "queue 3", 0, pointer, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_SIZE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to create a queue that has already been created.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_queue_create(&queue_0, "queue 1", TX_1_ULONG, pointer, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to create a queue with an invalid message size.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_queue_create(&queue_2, "queue 2", 47, pointer, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_SIZE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to create a queue with a NULL queue area.  */
    status =  tx_queue_create(&queue_2, "queue 2", TX_1_ULONG, TX_NULL, 3*sizeof(ULONG));

    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to create a queue with a too small queue area.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_queue_create(&queue_2, "queue 2", TX_1_ULONG, pointer, 1);

    /* Check for status.  */
    if (status != TX_SIZE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to delete a NULL pointer.  */
    status = tx_queue_delete(TX_NULL);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-created queue.  */
    queue_2.tx_queue_id =  0;
    status = tx_queue_delete(&queue_2);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Attempt to flush a NULL pointer.  */
    status =  tx_queue_flush(TX_NULL);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to flush a non-created queue.  */
    queue_2.tx_queue_id =  0;
    status =  tx_queue_flush(&queue_2);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to send something to the front of a non-queue.  */
    status =  tx_queue_front_send(TX_NULL, &source_message, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }
    
    /* Attempt to send something to the front of a non-created queue.  */
    queue_2.tx_queue_id =  0;
    status =  tx_queue_front_send(&queue_2, &source_message, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Attempt to send something with a NULL source pointer.  */
    status =  tx_queue_front_send(&queue_0, TX_NULL, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to send something to a non-queue.  */
    status =  tx_queue_send(TX_NULL, &source_message, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }
    
    /* Attempt to send something to a non-created queue.  */
    queue_2.tx_queue_id =  0;
    status =  tx_queue_send(&queue_2, &source_message, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Attempt to send something with a NULL source pointer.  */
    status =  tx_queue_send(&queue_0, TX_NULL, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to receive something from a non-queue.  */
    status =  tx_queue_receive(TX_NULL, &dest_message, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Attempt to receive something from a non-created queue.  */
    queue_2.tx_queue_id =  0;
    status =  tx_queue_receive(&queue_2, &dest_message, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_QUEUE_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Attempt to receive something to a NULL destination.  */
    status =  tx_queue_receive(&queue_0, TX_NULL, TX_NO_WAIT);
    
    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Queue error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }
#endif

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */
    source_message++;

    /* Place something on queue 0.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_0, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 0.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (source_message != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_0, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Now we need to do the same thing with the queue with three entries.  */

    source_message++;
    expected_message =  source_message;

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
        
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }

    /* Make sure we can do the same thing again!  */

    /* Place something on queue 1.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;
    status +=  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);
    source_message++;

    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #38\n");
        test_control_return(1);
    }
    
    /* Attempt to place something on a full queue.  */
    status =  tx_queue_send(&queue_1, &source_message, TX_NO_WAIT);    

    /* Should be an error.  */
    if (status != TX_QUEUE_FULL)
    {

        /* Queue error.  */
        printf("ERROR #39\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #40\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #41\n");
        test_control_return(1);
    }

    /* Attempt to receive something from queue 1.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be successful and dest_message should equal source. */
    if ((status != TX_SUCCESS) || (expected_message++ != dest_message))
    {

        /* Queue error.  */
        printf("ERROR #42\n");
        test_control_return(1);
    }

    /* Attempt to receive something from an empty queue.  */
    status =  tx_queue_receive(&queue_1, &dest_message, TX_NO_WAIT);

    /* Should be an error.  */
    if (status != TX_QUEUE_EMPTY)
    {

        /* Queue error.  */
        printf("ERROR #43\n");
        test_control_return(1);
    }


#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Resume thread 1 so that we can take an interrupt on top of it.  */
    tx_thread_resume(&thread_1);
    
    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Queue error.  */
        printf("ERROR #44\n");
        test_control_return(1);
    }

#endif

    /* Delete the queues.  */
    status =  tx_queue_delete(&queue_1);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #45\n");
        test_control_return(1);
    }

    status =  tx_queue_delete(&queue_0);
    if (status != TX_SUCCESS)
    {

        /* Queue error.  */
        printf("ERROR #46\n");
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

    while(1)
    {

        tx_thread_relinquish();
    }
}

