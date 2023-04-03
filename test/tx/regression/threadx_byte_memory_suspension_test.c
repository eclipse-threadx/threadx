/* This test is designed to test suspension on a memory byte pool.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;
static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;
static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_3;


static TX_BYTE_POOL   pool_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);



#ifndef TX_MANUAL_TEST

/* Define test flags for automated test.  */

extern TEST_FLAG    threadx_byte_allocate_loop_test;
extern TEST_FLAG    threadx_byte_release_loop_test;

#endif


void abort_and_resume_byte_allocating_thread(void)
{

UCHAR   *search_ptr;

    /* Adjust the search pointer to avoid the search pointer change for this test.  */
    search_ptr =  pool_0.tx_byte_pool_search;
    while (search_ptr >= pool_0.tx_byte_pool_search)
    
    {
        search_ptr =  *((UCHAR **) ((VOID *) search_ptr));
    }
    pool_0.tx_byte_pool_search =  search_ptr;
   
    tx_thread_wait_abort(&thread_3);
    tx_thread_resume(&thread_3);
}


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_byte_memory_suspension_application_define(void *first_unused_memory)
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

        printf("Running Byte Memory Suspension Test................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Suspension Test................................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Suspension Test................................. ERROR #3\n");
        test_control_return(1);
    }

    /* Create byte pool 0.  */
    status =  tx_byte_pool_create(&pool_0, "pool 0", pointer, 108);
    pointer = pointer + 108;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Suspension Test................................. ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;


    /* Inform user.  */
    printf("Running Byte Memory Suspension Test................................. ");
        
    /* Increment the thread counter.  */
    thread_0_counter++;

    /* Allocate memory from the pool.  Only one block of this size will fit.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Let other thread suspend on byte pool.  */
    tx_thread_relinquish();

    /* Now release the blocks. */
    status =  tx_byte_release(pointer);

    /* Check status.  */
    if ((status != TX_SUCCESS) && (thread_1_counter == 0))
    {

        /* Byte memory error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Let other thread run again.  */
    tx_thread_relinquish();

    /* Check to make sure thread 1 has now run.  */
    if (thread_1_counter != 1)
    {

        /* Byte memory error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }
    
    /* Now allocate the memory again. Only one block of this size will fit.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
    
    /* Resume the second thread.  */
    tx_thread_resume(&thread_2);
    
    /* Now relinquish to let both thread 1 and 2 suspend.  */
    tx_thread_relinquish();
    
    /* At this point both threads should be suspended on the byte pool.  */
    
    /* Release the memory again.  */
    status =  tx_byte_release(pointer);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }
    
    /* Now relinquish to get the other threads to run once.  */
    tx_thread_relinquish();
    tx_thread_relinquish();
    
    /* At this point both threads 1 and 2 are suspended on the byte pool again.  */
    if ((thread_1_counter != 3) && (thread_2_counter != 1))
    {

        /* Byte memory error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }
    
    /* Now allocate the memory again. Only one block of this size will fit.  */
    status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #10a\n");
        test_control_return(1);
    }
    
    /* Resume thread 3 to get it suspended on the the pool.  */
    tx_thread_resume(&thread_3);

#ifdef TX_MANUAL_TEST

    /* Set BP hear. Now release the memory and step into the code. After byte search issue IRQ2 mannually, which will
       make thread 3 abort the first request and make another request of a different size. This is the path we are trying 
       to generate in the test.  */
    status =  tx_byte_release(pointer);
#else

    /* Set the flag that will make thread 3 abort the first request and make another request of a different size. This tests the memory size change path 
       in the byte release loop logic.  */
    threadx_byte_release_loop_test =  1;
    status =  tx_byte_release(pointer);

#endif
 
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Byte memory error.  */
        printf("ERROR #10b\n");
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

    while(1)
    {

        /* Allocate memory from the pool.  */
        status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            return;

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Now release each of the blocks. */
        status =  tx_byte_release(pointer);

        /* Check for status.  */
        if (status != TX_SUCCESS)
            return;
        
        /* Let thread 0 run again.  */
        tx_thread_relinquish();
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;

    while(1)
    {

        /* Allocate memory from the pool.  */
        status = tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_WAIT_FOREVER);

        /* Check status.  */
        if (status != TX_SUCCESS)
            return;

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Now release each of the blocks. */
        status =  tx_byte_release(pointer);

        /* Check for status.  */
        if (status != TX_SUCCESS)
            return;
        
        /* Let thread 0 run again.  */
        tx_thread_relinquish();
    }
}


static void    thread_3_entry(ULONG thread_input)
{

UINT    status;
CHAR    *pointer;

    while (1)
    {

        /* Allocate memory from the pool.  */
        tx_byte_allocate(&pool_0, (VOID **) &pointer, 60, TX_WAIT_FOREVER);

#ifdef TX_MANUAL_TEST

        /* Set BP here and manually clear the owner after one failed byte search just to test the loop
           construct in tx_byte_allocate.c.  */
        status =  tx_byte_allocate(&pool_0, (VOID **) &pointer, 90, TX_WAIT_FOREVER);
#else

        /* Set the flag that clears the pool owner after one failed byte search to test the loop
           construct processing in tx_byte_allocate.c.  */
        threadx_byte_allocate_loop_test =  1;
        status =  tx_byte_allocate(&pool_0, (VOID **) &pointer, 90, TX_WAIT_FOREVER);
#endif
     
        /* Check for status.  */
        if (status != TX_SUCCESS)
            return;

        /* Increment the thread counter.  */
        thread_3_counter++;

        /* Now release the block. */
        status =  tx_byte_release(pointer);

        /* Check for status.  */
        if (status != TX_SUCCESS)
            return;
        
        /* suspend this thread.  */
        tx_thread_suspend(&thread_3);
    }
}
