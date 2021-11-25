/* This is a small demo of the high-performance ThreadX kernel running as a module. It includes
   examples of eight threads of different priorities, using a message queue, semaphore, mutex,
   event flags group, byte pool, and block pool.  */

/* Specify that this is a module!  */

#define TXM_MODULE


/* Include the ThreadX module definitions.  */

#include "txm_module.h"


/* Define constants.  */

#define DEMO_BYTE_POOL_SIZE     (1024 * 48)
#define DEMO_STACK_SIZE         (1024 * 4)
#define DEMO_BLOCK_POOL_SIZE    100
#define DEMO_QUEUE_SIZE         100


/* Define the pool space in the bss section of the module. ULONG is used to
   get the word alignment.  */

ULONG                   demo_module_pool_space[DEMO_BYTE_POOL_SIZE / sizeof(ULONG)];


/* Define the ThreadX object control blocks...  */

TX_THREAD               *thread_0;
TX_THREAD               *thread_1;
TX_THREAD               *thread_2;
TX_THREAD               *thread_3;
TX_THREAD               *thread_4;
TX_THREAD               *thread_5;
TX_THREAD               *thread_6;
TX_THREAD               *thread_7;
TX_QUEUE                *queue_0;
TX_SEMAPHORE            *semaphore_0;
TX_MUTEX                *mutex_0;
TX_EVENT_FLAGS_GROUP    *event_flags_0;
TX_BYTE_POOL            *byte_pool_0;
TX_BLOCK_POOL           *block_pool_0;


/* Define the counters used in the demo application...  */

ULONG           thread_0_counter = 0;
ULONG           thread_1_counter = 0;
ULONG           thread_1_messages_sent = 0;
ULONG           thread_2_counter = 0;
ULONG           thread_2_messages_received = 0;
ULONG           thread_3_counter = 0;
ULONG           thread_4_counter = 0;
ULONG           thread_5_counter = 0;
ULONG           thread_6_counter = 0;
ULONG           thread_7_counter = 0;
ULONG           semaphore_0_puts = 0;
ULONG           event_0_sets = 0;
ULONG           queue_0_sends = 0;

/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_and_4_entry(ULONG thread_input);
void    thread_5_entry(ULONG thread_input);
void    thread_6_and_7_entry(ULONG thread_input);

void  semaphore_0_notify(TX_SEMAPHORE *semaphore_ptr)
{

    if (semaphore_ptr == semaphore_0)
        semaphore_0_puts++;
}


void  event_0_notify(TX_EVENT_FLAGS_GROUP *event_flag_group_ptr)
{

    if (event_flag_group_ptr == event_flags_0)
        event_0_sets++;
}


void  queue_0_notify(TX_QUEUE *queue_ptr)
{

    if (queue_ptr == queue_0)
        queue_0_sends++;
}


/* Define the module start function.  */

void    demo_module_start(ULONG id)
{

UINT    status;
CHAR    *pointer;


    /* Allocate all the objects. In MMU mode, modules cannot allocate control blocks within
       their own memory area so they cannot corrupt the resident portion of ThreadX by overwriting
       the control block(s).  */
    status = txm_module_object_allocate((void*)&thread_0, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_1, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_2, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_3, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_4, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_5, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_6, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&thread_7, sizeof(TX_THREAD));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&queue_0, sizeof(TX_QUEUE));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&semaphore_0, sizeof(TX_SEMAPHORE));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&mutex_0, sizeof(TX_MUTEX));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&event_flags_0, sizeof(TX_EVENT_FLAGS_GROUP));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&byte_pool_0, sizeof(TX_BYTE_POOL));
    while (status != TX_SUCCESS);
    status = txm_module_object_allocate((void*)&block_pool_0, sizeof(TX_BLOCK_POOL));
    while (status != TX_SUCCESS);


    /* Create a byte memory pool from which to allocate the thread stacks.  */
    status = tx_byte_pool_create(byte_pool_0, "module byte pool 0", demo_module_pool_space, DEMO_BYTE_POOL_SIZE);
    while (status != TX_SUCCESS);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the message queue.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create the message queue shared by threads 1 and 2.  */
    status = tx_queue_create(queue_0, "module queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));
    while (status != TX_SUCCESS);

    status = tx_queue_send_notify(queue_0, queue_0_notify);
    while (status != TX_SUCCESS);

    /* Create the semaphore used by threads 3 and 4.  */
    status = tx_semaphore_create(semaphore_0, "module semaphore 0", 1);
    while (status != TX_SUCCESS);

    status = tx_semaphore_put_notify(semaphore_0, semaphore_0_notify);
    while (status != TX_SUCCESS);

    /* Create the event flags group used by threads 1 and 5.  */
    status = tx_event_flags_create(event_flags_0, "module event flags 0");
    while (status != TX_SUCCESS);

    status = tx_event_flags_set_notify(event_flags_0, event_0_notify);
    while (status != TX_SUCCESS);

    /* Create the mutex used by thread 6 and 7 without priority inheritance.  */
    status = tx_mutex_create(mutex_0, "module mutex 0", TX_NO_INHERIT);
    while (status != TX_SUCCESS);

    /* Allocate the memory for a small block pool.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create a block memory pool to allocate a message buffer from.  */
    status = tx_block_pool_create(block_pool_0, "module block pool 0", sizeof(ULONG), pointer, DEMO_BLOCK_POOL_SIZE);
    while (status != TX_SUCCESS);

    /* Allocate a block and release the block memory.  */
    status = tx_block_allocate(block_pool_0, (VOID **) &pointer, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Release the block back to the pool.  */
    status = tx_block_release(pointer);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 0.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create the main thread.  */
    status = tx_thread_create(thread_0, "module thread 0", thread_0_entry, 0,
            pointer, DEMO_STACK_SIZE,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 1.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create threads 1 and 2. These threads pass information through a ThreadX
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    status = tx_thread_create(thread_1, "module thread 1", thread_1_entry, 1,
            pointer, DEMO_STACK_SIZE,
            16, 16, 4, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 2.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    status = tx_thread_create(thread_2, "module thread 2", thread_2_entry, 2,
            pointer, DEMO_STACK_SIZE,
            16, 16, 4, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 3.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.
       An interesting thing here is that both threads share the same instruction area.  */
    status = tx_thread_create(thread_3, "module thread 3", thread_3_and_4_entry, 3,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 4.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    status = tx_thread_create(thread_4, "module thread 4", thread_3_and_4_entry, 4,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 5.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create thread 5.  This thread simply pends on an event flag which will be set
       by thread_0.  */
    status = tx_thread_create(thread_5, "module thread 5", thread_5_entry, 5,
            pointer, DEMO_STACK_SIZE,
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 6.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    /* Create threads 6 and 7.  These threads compete for a ThreadX mutex.  */
    status = tx_thread_create(thread_6, "module thread 6", thread_6_and_7_entry, 6,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);
    while (status != TX_SUCCESS);

    /* Allocate the stack for thread 7.  */
    status = tx_byte_allocate(byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);
    while (status != TX_SUCCESS);

    status = tx_thread_create(thread_7, "module thread 7", thread_6_and_7_entry, 7,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);
    while (status != TX_SUCCESS);
}


/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{

UINT    status;

    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(10);

        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(event_flags_0, 0x1, TX_OR);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Send message to queue 0.  */
        status =  tx_queue_send(queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        /* Increment the message sent.  */
        thread_1_messages_sent++;
    }
}


void    thread_2_entry(ULONG thread_input)
{

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {
        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;

        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
    }
}


void    thread_3_and_4_entry(ULONG thread_input)
{

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 3)
            thread_3_counter++;
        else
            thread_4_counter++;

        /* Get the semaphore with suspension.  */
        status =  tx_semaphore_get(semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the semaphore.  */
        tx_thread_sleep(2);

        /* Release the semaphore.  */
        status =  tx_semaphore_put(semaphore_0);

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

        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Wait for event flag 0.  */
        status =  tx_event_flags_get(event_flags_0, 0x1, TX_OR_CLEAR,
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

        /* Increment the thread counter.  */
        if (thread_input == 6)
            thread_6_counter++;
        else
            thread_7_counter++;

        /* Get the mutex with suspension.  */
        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Get the mutex again with suspension.  This shows
           that an owning thread may retrieve the mutex it
           owns multiple times.  */
        status =  tx_mutex_get(mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Sleep for 2 ticks to hold the mutex.  */
        tx_thread_sleep(2);

        /* Release the mutex.  */
        status =  tx_mutex_put(mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;

        /* Release the mutex again.  This will actually
           release ownership since it was obtained twice.  */
        status =  tx_mutex_put(mutex_0);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


