/* This test is designed to test termination on thread suspended on memory byte pool.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;


static TX_BYTE_POOL   pool_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_byte_memory_thread_terminate_application_define(void *first_unused_memory)
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

        printf("Running Byte Memory Thread Terminate Test........................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Thread Terminate Test........................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create byte pools 0 and 1.  */
    status =  tx_byte_pool_create(&pool_0, "pool 0", pointer, 108);
    pointer = pointer + 108;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Thread Terminate Test........................... ERROR #3\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;


    /* Inform user.  */
    printf("Running Byte Memory Thread Terminate Test........................... ");

    /* Increment the thread counter.  */
    thread_0_counter++;
        
    /* Allocate memory from the pool.  Only one block of this size will fit.   */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Let other thread suspend on byte pool.  */
    tx_thread_relinquish();

    /* Terminate the other thread.  */
    status =  tx_thread_terminate(&thread_1);
 
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /*  Release block back.  */
    status =  tx_byte_release(pointer);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Allocate memory from the pool.  Only one block of this size will fit.   */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_NO_WAIT);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 0))
    {

        /* Byte memory error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
    
    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void    thread_1_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;

    while(1)
    {

        /* Allocate memory from the pool.  */
        status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_WAIT_FOREVER);

        /* Should never get here!  */
        if (status != TX_NO_MEMORY)
            return;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}

