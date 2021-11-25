/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
   threads of different priorities, using a message queue, semaphore, mutex, event flags group, 
   byte pool, and block pool.  */

#include "tx_api.h"

#define TX_TEST_STACK_SIZE         (1024 * 2)
#define TX_TEST_QUEUE_SIZE         (16)
#define TX_TEST_BYTE_POOL_SIZE     (1024 * 4)
#define TX_TEST_BLOCK_POOL_SIZE    (1024 * 4)


/* Define the ThreadX object control blocks...  */

TX_THREAD               tx_test_thread_0;
TX_THREAD               tx_test_thread_1;
TX_THREAD               tx_test_thread_2;
TX_QUEUE                tx_test_queue_0;
TX_SEMAPHORE            tx_test_semaphore_0;
TX_MUTEX                tx_test_mutex_0;
TX_EVENT_FLAGS_GROUP    tx_test_event_flags_0;
TX_BYTE_POOL            tx_test_byte_pool_0;
TX_BLOCK_POOL           tx_test_block_pool_0;


/* Define the counters used in the demo application...  */

ULONG                   tx_test_thread_0_counter;
ULONG                   tx_test_thread_1_counter;
ULONG                   tx_test_thread_2_counter;


/* Define thread prototypes.  */

void    tx_test_thread_0_entry(ULONG thread_input);
void    tx_test_thread_1_entry(ULONG thread_input);
void    tx_test_thread_2_entry(ULONG thread_input);


/* Define the thread stacks.  */

ULONG	tx_test_thread_0_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_1_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];
ULONG	tx_test_thread_2_stack[TX_TEST_STACK_SIZE / sizeof(ULONG)];


/* Define other buffers used by the test code.  */

ULONG 	tx_test_queue_0_buffer[TX_TEST_QUEUE_SIZE];
ULONG 	tx_test_byte_pool_0_buffer[TX_TEST_BYTE_POOL_SIZE / sizeof(ULONG)];
ULONG 	tx_test_block_pool_0_buffer[TX_TEST_BLOCK_POOL_SIZE / sizeof(ULONG)];


/* Define test function prototypes.  */

void    tx_test_sleep();
void    tx_test_busy_loop();

int     tx_test_sum_many_params(
			void (*f)(),
			int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8,
			int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16,
			int p17, int p18, int p19, int p20, int p21, int p22, int p23, int p24,
			int p25, int p26, int p27, int p28, int p29, int p30, int p31, int p32,
			int p33, int p34, int p35, int p36, int p37, int p38, int p39, int p40,
			int p41, int p42, int p43, int p44, int p45, int p46, int p47, int p48,
			int p49, int p50, int p51, int p52, int p53, int p54, int p55, int p56,
			int p57, int p58, int p59, int p60, int p61, int p62, int p63, int p64,
			int p65, int p66, int p67, int p68, int p69, int p70, int p71, int p72);

int     tx_test_multiply_many_params(
			void (*f)(),
			int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8,
			int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16,
			int p17, int p18, int p19, int p20, int p21, int p22, int p23, int p24,
			int p25, int p26, int p27, int p28, int p29, int p30, int p31, int p32,
			int p33, int p34, int p35, int p36, int p37, int p38, int p39, int p40,
			int p41, int p42, int p43, int p44, int p45, int p46, int p47, int p48,
			int p49, int p50, int p51, int p52, int p53, int p54, int p55, int p56,
			int p57, int p58, int p59, int p60, int p61, int p62, int p63, int p64,
			int p65, int p66, int p67, int p68, int p69, int p70, int p71, int p72);


/**/

void tx_test_error_handler()
{
	for (;;)
	{
		/* Stay here forever */
	}
}

/* Define main entry point.  */

int main()
{

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();

    return(0);
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

UINT	status = TX_SUCCESS;


    /* Create the ThreadX test thread 0.  */
    status = tx_thread_create(
    		&tx_test_thread_0, "ThreadX test thread 0",
			tx_test_thread_0_entry, 0,
			tx_test_thread_0_stack,
			TX_TEST_STACK_SIZE,
            5, 5,
			TX_NO_TIME_SLICE,
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 1.  */
    status = tx_thread_create(
    		&tx_test_thread_1, "ThreadX test thread 1",
			tx_test_thread_1_entry, 1,
    		tx_test_thread_1_stack,
			TX_TEST_STACK_SIZE,
            7, 7,
			3, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the ThreadX test thread 2.  */
    status = tx_thread_create(
    		&tx_test_thread_2, "ThreadX test thread 2",
			tx_test_thread_2_entry, 2,
    		tx_test_thread_2_stack,
			TX_TEST_STACK_SIZE,
            5, 7,
			4, /* time slice */
			TX_AUTO_START);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the message queue shared by threads 1 and 2.  */
    status = tx_queue_create(&tx_test_queue_0, "ThreadX test queue 0", TX_1_ULONG, tx_test_queue_0_buffer, TX_TEST_QUEUE_SIZE*sizeof(ULONG));
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the semaphore used for testing.  */
    status = tx_semaphore_create(&tx_test_semaphore_0, "ThreadX test semaphore 0", 1);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the event flags group used by threads 1 and 5.  */
    status = tx_event_flags_create(&tx_test_event_flags_0, "ThreadX test event flags 0");
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create the mutex used for testing without priority inheritance.  */
    status = tx_mutex_create(&tx_test_mutex_0, "ThreadX test mutex 0", TX_NO_INHERIT);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Create a byte memory pool for testing.  */
    status = tx_byte_pool_create(&tx_test_byte_pool_0, "ThreadX test byte pool 0", tx_test_byte_pool_0_buffer, TX_TEST_BYTE_POOL_SIZE);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Quick test of the byte pool during initialization.  */
    {
		CHAR *pointer = TX_NULL;

		/* Allocate a block and release the block memory.  */
		status = tx_byte_allocate(&tx_test_byte_pool_0, (VOID **) &pointer, 123, TX_NO_WAIT);
	    if (status != TX_SUCCESS) tx_test_error_handler();

		/* Release the block back to the pool.  */
		status = tx_block_release(pointer);
	    if (status != TX_SUCCESS) tx_test_error_handler();
    }

    /* Create a block memory pool to allocate a message buffer from.  */
    status = tx_block_pool_create(&tx_test_block_pool_0, "block pool 0", sizeof(ULONG), tx_test_block_pool_0_buffer, TX_TEST_BLOCK_POOL_SIZE);
    if (status != TX_SUCCESS) tx_test_error_handler();

    /* Quick test of the block pool during initialization.  */
    {
		CHAR *pointer = TX_NULL;

		/* Allocate a block and release the block memory.  */
		status = tx_block_allocate(&tx_test_block_pool_0, (VOID **) &pointer, TX_NO_WAIT);
	    if (status != TX_SUCCESS) tx_test_error_handler();

		/* Release the block back to the pool.  */
		status = tx_block_release(pointer);
	    if (status != TX_SUCCESS) tx_test_error_handler();
    }
}



/* Define the test threads.  */

void    tx_test_thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        tx_test_thread_0_counter++;

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(10);

        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(&tx_test_event_flags_0, 0x1, TX_OR);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    tx_test_thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
    	tx_test_thread_1_counter++;

        /* Send message to queue 0.  */
        status =  tx_queue_send(&tx_test_queue_0, &tx_test_thread_1_counter, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    tx_test_thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {

        /* Increment the thread counter.  */
    	tx_test_thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(&tx_test_queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we 
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != tx_test_thread_1_counter))
            break;
    }
}


void    thread_3_and_4_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Get the semaphore with suspension.  */
        status =  tx_semaphore_get(&tx_test_semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the semaphore.  */
        tx_thread_sleep(2);

        /* Release the semaphore.  */
        status =  tx_semaphore_put(&tx_test_semaphore_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_5_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual_flags;


    /* This thread simply waits for an event in a forever loop.  */
    while(1)
    {

        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(&tx_test_event_flags_0, 0x1, TX_OR_CLEAR,
                                                &actual_flags, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) || (actual_flags != 0x1))
            break;
    }
}


void    thread_6_and_7_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 6 and thread 7.  As the loop
       below shows, these function compete for ownership of mutex_0.  */
    while(1)
    {

        /* Get the mutex with suspension.  */
        status =  tx_mutex_get(&tx_test_mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Get the mutex again with suspension.  This shows
           that an owning thread may retrieve the mutex it
           owns multiple times.  */
        status =  tx_mutex_get(&tx_test_mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the mutex.  */
        tx_thread_sleep(2);

        /* Release the mutex.  */
        status =  tx_mutex_put(&tx_test_mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Release the mutex again.  This will actually 
           release ownership since it was obtained twice.  */
        status =  tx_mutex_put(&tx_test_mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}
