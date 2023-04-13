/* This test is designed to test contention of two threads on a single
   memory byte pool.  */
   
#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;
static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;
static unsigned long   initial_pool_size;


static TX_BYTE_POOL   pool_0;


static int test_done;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_byte_memory_thread_contention_application_define(void *first_unused_memory)
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
            17, 17, 1, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Thread Contention Test.......................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 1, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Thread Contention Test.......................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 1, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Thread Contention Test.......................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create byte pool 0.  */
    status =  tx_byte_pool_create(&pool_0, "pool 0", pointer, 108);
    pointer = pointer + 108;
    
    /* Save off the intial pool size.  */
    initial_pool_size =  pool_0.tx_byte_pool_available;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Thread Contention Test.......................... ERROR #4\n");
        test_control_return(1);
    }

    /* Set the test done flag to false.  */
    test_done =  TX_FALSE;
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;


    /* Inform user.  */
    printf("Running Byte Memory Thread Contention Test.......................... ");

    /* Set time to 0.  */
    tx_time_set(0);

    while(1)
    {

        /* Allocate memory from the pool.  This size will cause merge activity
           because the search pointer will sit in this large block about half 
           the time.  */
        status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
        {

            /* Byte memory error.  */
            printf("ERROR #5\n");
            test_control_return(1);
        }

        /* Fill the memory.  */
        TX_MEMSET(pointer, (CHAR) 0xEF, 60);

        /* Now release the block. */
        status =  tx_byte_release(pointer);

        /* Check for status.  */
        if (status != TX_SUCCESS)
        {

            /* Byte memory error.  */
            printf("ERROR #6\n");
            test_control_return(1);
        }

        /* Check the time.  */
        if (tx_time_get() > 128)
            break;    

        /* Increment the thread counter.  */
        thread_0_counter++;
    }

    /* Set the done flag.  */
    test_done =  TX_TRUE;
    
    /* Sleep to let the other threads finish up!  */
    tx_thread_sleep(2);

    /* Determine if we all all the original memory and that thread 1
       is in the proper place.  */
    if (pool_0.tx_byte_pool_available != initial_pool_size)
    {

        /* Byte memory error.  */
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
CHAR    *pointer;


    while(test_done == TX_FALSE)
    {

        /* Allocate memory from the pool.  */
        status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 30, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            return;

        /* Fill the memory.  */
        TX_MEMSET(pointer, (CHAR) 0xEF, 30);

        /* Now release the block. */
        status =  tx_byte_release(pointer);

        /* Check for status.  */
        if (status != TX_SUCCESS)
            return;
        
        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;


    while(test_done == TX_FALSE)
    {

        /* Allocate memory from the pool.  */
        status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 12, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            return;

        /* Fill the memory.  */
        TX_MEMSET(pointer, (CHAR) 0xEF, 12);

        /* Now release the block. */
        status =  tx_byte_release(pointer);

        /* Check for status.  */
        if (status != TX_SUCCESS)
            return;
        
        /* Increment the thread counter.  */
        thread_2_counter++;
    }
}
