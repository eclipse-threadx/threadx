/* This test is designed to test timeouts on suspension on memory block pools.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;
static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;


static TX_BLOCK_POOL   pool_0;


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
void    threadx_block_memory_suspension_timeout_application_define(void *first_unused_memory)
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

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Suspension Timeout Test........................ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Suspension Timeout Test........................ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Suspension Timeout Test........................ ERROR #3\n");
        test_control_return(1);
    }

    /* Create block pool.  */
    status =  tx_block_pool_create(&pool_0, "pool 0", 100, pointer, 320);
    pointer = pointer + 320;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Suspension Timeout Test........................ ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer_1;
CHAR    *pointer_2;
CHAR    *pointer_3;


    /* Inform user.  */
    printf("Running Block Memory Suspension Timeout Test........................ ");

    /* Allocate all blocks from the pool.  */
    status =  tx_block_allocate(&pool_0, (VOID **) &pointer_1, TX_NO_WAIT);
    status += tx_block_allocate(&pool_0, (VOID **) &pointer_2, TX_NO_WAIT);
    status += tx_block_allocate(&pool_0, (VOID **) &pointer_3, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Block memory error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_1, (CHAR) 0xEF, 100);

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_2, (CHAR) 0xEF, 100);

    /* Set all the memory of the blocks.  */
    TX_MEMSET(pointer_3, (CHAR) 0xEF, 100);

    /* Sleep for 64 ticks to allow the other thread 6 timeouts on the block
       pool.  */
    tx_thread_sleep(64);

    /* Incrment the run counter.  */
    thread_0_counter++;

    /* Check the counter of the other thread.  */
    if ((thread_1_counter != 6) || (thread_2_counter != 3))
    {

        /* Block memory error.  */
        printf("ERROR #6\n");
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
CHAR    *pointer_1;


    /* Attempt to get a block from the pool.  */
    while(1)
    {

        /* Allocate a block from the pool.  */
        status =  tx_block_allocate(&pool_0, (VOID **) &pointer_1, 10);

        /* Determine if the allocate was successful.  */
        if (status != TX_NO_MEMORY)
            return;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer_1;


    /* Delay so we get some single suspension timeouts as well.  */
    tx_thread_sleep(32);

    /* Attempt to get a block from the pool.  */
    while(1)
    {

        /* Allocate a block from the pool.  */
        status =  tx_block_allocate(&pool_0, (VOID **) &pointer_1, 10);

        /* Determine if the allocate was successful.  */
        if (status != TX_NO_MEMORY)
            return;

        /* Increment the thread counter.  */
        thread_2_counter++;
    }
}

