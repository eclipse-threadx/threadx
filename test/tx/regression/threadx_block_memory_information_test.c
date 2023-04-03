/* This test is designed to test block memory information services.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_block_pool.h"


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Define the external reference for the preempt disable flag.  */

extern volatile UINT   _tx_thread_preempt_disable;



static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_3;

static unsigned long   thread_4_counter =  0;
static TX_THREAD       thread_4;

static unsigned long   thread_5_counter =  0;
static TX_THREAD       thread_5;

static unsigned long   thread_6_counter =  0;
static TX_THREAD       thread_6;

static TX_BLOCK_POOL   block_pool_0;
static TX_BLOCK_POOL   block_pool_2;
#ifdef TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO  
static TX_BLOCK_POOL   block_pool_1;
#endif

static int             test_status;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);

/* Direct core function to bypass the error checking shell.  */
UINT  _tx_block_pool_info_get(TX_BLOCK_POOL *pool_ptr, CHAR **name, ULONG *available_blocks, 
                    ULONG *total_blocks, TX_THREAD **first_suspended, 
                    ULONG *suspended_count, TX_BLOCK_POOL **next_pool);

/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

    /* Determine if the test case we are looking for is present.  */
    if ((_tx_thread_preempt_disable) && (test_status == 1))
    {

        /* Abort the wait of thread 3.  */
        tx_thread_wait_abort(&thread_3);

        /* End the ISR processing.  */
        test_status =  2;    
        test_isr_dispatch =  TX_NULL;
    }
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_block_memory_information_application_define(void *first_unused_memory)
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

        printf("Running Block Memory Information Test............................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 4, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #7\n");
        test_control_return(1);
    }

    /* Create the block_pool with one block.  */
    status =  tx_block_pool_create(&block_pool_0, "block_pool 0", 30, pointer, 40);
    pointer = pointer + 40;
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Block Memory Information Test............................... ERROR #8\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT            status;
VOID            *pointer;
CHAR            *name;
ULONG           available;
ULONG           total_blocks;
TX_THREAD       *first_suspended;
ULONG           suspended_count;
TX_BLOCK_POOL   *next_pool;
ULONG           allocates;
ULONG           releases;
ULONG           suspensions;
ULONG           timeouts;


    /* Inform user.  */
    printf("Running Block Memory Information Test............................... ");

    /* Allocate the one block.  */
    tx_block_allocate(&block_pool_0, &pointer, TX_NO_WAIT);

    /* Nothing to do here, but check prioritization with no suspended threads.  */
    status =  tx_block_pool_prioritize(&block_pool_0);

    /* Check for an error condition.   */
    if (status != TX_SUCCESS)
    {

        /* Block Pool error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Get both threads suspended.  */
    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);

    /* Increment the thread counter.  */
    thread_0_counter++;

    /* Make sure thread 1 and 2 are suspended on the block_pool.  */
    if ((thread_1.tx_thread_state != TX_BLOCK_MEMORY) || (thread_2.tx_thread_state != TX_BLOCK_MEMORY) ||
        (block_pool_0.tx_block_pool_suspension_list != &thread_1))
    {

        /* Block Pool error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Prioritize the block pool suspension list.  */
    status =  tx_block_pool_prioritize(&block_pool_0);

    /* Check status and make sure thread 1 is terminated.  */
    if ((status != TX_SUCCESS) || (block_pool_0.tx_block_pool_suspension_list != &thread_2))
    {

        /* Block Pool error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* At this point we are going to get more than 2 threads suspended.  */
    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);
    tx_thread_resume(&thread_3);
    tx_thread_resume(&thread_4);
    tx_thread_resume(&thread_5);
    tx_thread_resume(&thread_6);
    
    /* Prioritize the block pool suspension list.  */
    status =  tx_block_pool_prioritize(&block_pool_0);

    /* Check status and make sure thread 3 is at the front of the suspension list.  */
    if ((status != TX_SUCCESS) || (block_pool_0.tx_block_pool_suspension_list != &thread_3))
    {

        /* Block Pool error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }
    
    /* Now loop to test the interrupt of the prioritize loop logic.  */
    test_status =  1;
    test_isr_dispatch =  test_isr;  
    do
    {

        /* Prioritize the block pool suspension list.  */
        status =  tx_block_pool_prioritize(&block_pool_0);

        /* Check status and make sure thread 1 is terminated.  */
        if (status != TX_SUCCESS)
        {

            /* Block Pool error.  */
            printf("ERROR #13\n");
            test_control_return(1);
        }

    } while (test_status == 1);
    
    /* Now determine if thread 4 is at the front of the list... It should be!  */
    if (block_pool_0.tx_block_pool_suspension_list != &thread_4)
    {    

        /* Block Pool error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Test the NULL pointer on the info get call.  */
    status =  tx_block_pool_info_get(TX_NULL, TX_NULL, TX_NULL,  TX_NULL,       TX_NULL,          TX_NULL,          TX_NULL);

    /* Check for the proper error code.  */
    if (status != TX_POOL_ERROR)
    {

        /* Block Pool error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Test the non-thread pointer on the info get call.  */
    block_pool_2.tx_block_pool_id =  0;
    status =  tx_block_pool_info_get(&block_pool_2, TX_NULL, TX_NULL,  TX_NULL,       TX_NULL,          TX_NULL,          TX_NULL);

    /* Check for the proper error code.  */
    if (status != TX_POOL_ERROR)
    {

        /* Block Pool error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

#endif
    
    /* Now use the information services in order to test them.  */
    status =  tx_block_pool_info_get(&block_pool_0, TX_NULL, TX_NULL,  TX_NULL,       TX_NULL,          TX_NULL,          TX_NULL);
    status +=  tx_block_pool_info_get(&block_pool_0, &name, &available, &total_blocks, &first_suspended, &suspended_count, &next_pool);
    
    /* Check for an error condition.  */
    if ((status) || (available != block_pool_0.tx_block_pool_available) || (total_blocks != block_pool_0.tx_block_pool_total) || 
        (first_suspended != &thread_4) || (suspended_count != block_pool_0.tx_block_pool_suspended_count) || (next_pool != &block_pool_0))
    {

        /* Block Pool error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

#ifdef TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO  

     /* Call the core block pool info get function with a NULL pointer.  */
    status =  _tx_block_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check for the proper error code.  */
    if (status != TX_PTR_ERROR)
    {

        /* Block Pool error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

   /* Call the core block pool info get function with a non-initialized pool.  */
    status =  _tx_block_pool_performance_info_get(&block_pool_1, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check for the proper error code.  */
    if (status != TX_PTR_ERROR)
    {

        /* Block Pool error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Now get the pool performance information.   */
    status =  tx_block_pool_performance_info_get(&block_pool_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_block_pool_performance_info_get(&block_pool_0, &allocates, &releases, &suspensions, &timeouts);
    
    /* Check for an error condition.  */
    if ((status) || (allocates != block_pool_0.tx_block_pool_performance_allocate_count) || 
        (releases != block_pool_0.tx_block_pool_performance_release_count) || 
        (suspensions != block_pool_0.tx_block_pool_performance_suspension_count) || 
        (timeouts != block_pool_0.tx_block_pool_performance_timeout_count))
    {

        /* Block Pool error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Now get the system pool performance information.  */
    status =  tx_block_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_block_pool_performance_system_info_get(&allocates, &releases, &suspensions, &timeouts);
    
    /* Check for an error condition.  */
    if ((status) || (allocates != _tx_block_pool_performance_allocate_count) || (releases != _tx_block_pool_performance_release_count) || 
        (suspensions != _tx_block_pool_performance_suspension_count) || (timeouts != _tx_block_pool_performance_timeout_count))
    {

        /* Block Pool error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
#else

    /* Call the block pool performance info get function.  */
    status =  tx_block_pool_performance_info_get(&block_pool_0, &allocates, &releases, &suspensions, &timeouts);
    
    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Call the block pool performance info get function.  */
    status =  tx_block_pool_performance_info_get(TX_NULL, &allocates, &releases, &suspensions, &timeouts);
    
    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Call the block pool performance info get function.  */
    status =  tx_block_pool_performance_info_get(TX_NULL, TX_NULL, &releases, &suspensions, &timeouts);
    
    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Call the block pool performance info get function.  */
    status =  tx_block_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &suspensions, &timeouts);
    
    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Call the block pool performance info get function.  */
    status =  tx_block_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);
    
    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Call the block pool performance info get function.  */
    status =  tx_block_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Now get the system pool performance information.  */
    status =  tx_block_pool_performance_system_info_get(&allocates, &releases, &suspensions, &timeouts);

    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Now get the system pool performance information.  */
    status =  tx_block_pool_performance_system_info_get(TX_NULL, &releases, &suspensions, &timeouts);

    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Now get the system pool performance information.  */
    status =  tx_block_pool_performance_system_info_get(TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Now get the system pool performance information.  */
    status =  tx_block_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Now get the system pool performance information.  */
    status =  tx_block_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for the proper error code.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Block Pool error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
#endif
}


static void    thread_1_entry(ULONG thread_input)
{
UINT    status;
VOID    *pointer;

    /* Loop forever!  */
    while(1)
    {


        /* Get block from pool.  */
        status =  tx_block_allocate(&block_pool_0, &pointer, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;
VOID    *pointer;


    /* Loop forever!  */
    while(1)
    {


        /* Get block from pool.  */
        status =  tx_block_allocate(&block_pool_0, &pointer, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_2_counter++;
    }
}


static void    thread_3_entry(ULONG thread_input)
{

UINT    status;
VOID    *pointer;


    /* Loop forever!  */
    while(1)
    {


        /* Get block from pool.  */
        status =  tx_block_allocate(&block_pool_0, &pointer, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_3_counter++;
    }
}


static void    thread_4_entry(ULONG thread_input)
{

UINT    status;
VOID    *pointer;


    /* Loop forever!  */
    while(1)
    {


        /* Get block from pool.  */
        status =  tx_block_allocate(&block_pool_0, &pointer, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_4_counter++;
    }
}


static void    thread_5_entry(ULONG thread_input)
{

UINT    status;
VOID    *pointer;


    /* Loop forever!  */
    while(1)
    {


        /* Get block from pool.  */
        status =  tx_block_allocate(&block_pool_0, &pointer, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_5_counter++;
    }
}


static void    thread_6_entry(ULONG thread_input)
{

UINT    status;
VOID    *pointer;


    /* Loop forever!  */
    while(1)
    {


        /* Get block from pool.  */
        status =  tx_block_allocate(&block_pool_0, &pointer, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_6_counter++;
    }
}

