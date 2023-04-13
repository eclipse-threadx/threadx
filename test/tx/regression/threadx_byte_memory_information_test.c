/* This test is designed to test byte memory information.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_byte_pool.h"


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

static TX_BYTE_POOL    byte_pool_0;
#ifdef TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO
static TX_BYTE_POOL    byte_pool_1;
#endif
static TX_BYTE_POOL    byte_pool_2;

static int             test_status;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);

UINT  _tx_byte_pool_performance_info_get(TX_BYTE_POOL *pool_ptr, ULONG *allocates, ULONG *releases,
                    ULONG *fragments_searched, ULONG *merges, ULONG *splits, ULONG *suspensions, ULONG *timeouts);


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
void    threadx_byte_memory_information_application_define(void *first_unused_memory)
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

        printf("Running Byte Memory Information Test................................ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 4, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #7\n");
        test_control_return(1);
    }

    /* Create the byte_pool with one byte.  */
    status =  tx_byte_pool_create(&byte_pool_0, "byte_pool 0", pointer, 100);
    pointer = pointer + 100;
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Byte Memory Information Test................................ ERROR #8\n");
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
ULONG           fragments;
TX_THREAD       *first_suspended;
ULONG           suspended_count;
TX_BYTE_POOL    *next_pool;
ULONG           allocates;
ULONG           releases;
ULONG           fragments_searched;
ULONG           merges;
ULONG           splits;
ULONG           suspensions;
ULONG           timeouts;


    /* Inform user.  */
    printf("Running Byte Memory Information Test................................ ");

    /* Allocate the one byte.  */
    tx_byte_allocate(&byte_pool_0, &pointer, 80, TX_NO_WAIT);

    /* Nothing to do here, but check prioritization with no suspended threads.  */
    status =  tx_byte_pool_prioritize(&byte_pool_0);

    /* Check status and make sure thread 1 is terminated.  */
    if (status != TX_SUCCESS)
    {

        /* Byte Pool error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);

    /* Increment the thread counter.  */
    thread_0_counter++;

    /* Make sure thread 1 and 2 are suspended on the byte_pool.  */
    if ((thread_1.tx_thread_state != TX_BYTE_MEMORY) || (thread_2.tx_thread_state != TX_BYTE_MEMORY) ||
        (byte_pool_0.tx_byte_pool_suspension_list != &thread_1))
    {

        /* Byte Pool error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Prioritize the byte pool suspension list.  */
    status =  tx_byte_pool_prioritize(&byte_pool_0);

    /* Check status and make sure thread 2 is now at the front of the suspension list.  */
    if ((status != TX_SUCCESS) || (byte_pool_0.tx_byte_pool_suspension_list != &thread_2))
    {

        /* Byte Pool error.  */
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

    /* Prioritize the byte pool suspension list.  */
    status =  tx_byte_pool_prioritize(&byte_pool_0);
    
    /* Check status and make sure thread 3 is now at the front of the suspension list.  */
    if ((status != TX_SUCCESS) || (byte_pool_0.tx_byte_pool_suspension_list != &thread_3))
    {

        /* Byte Pool error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Now loop to test the interrupt of the prioritize loop logic.  */
    test_status =  1;
    test_isr_dispatch =  test_isr;  
    do
    {

        /* Prioritize the byte pool suspension list.  */
        status =  tx_byte_pool_prioritize(&byte_pool_0);

        /* Check status and make sure thread 1 is terminated.  */
        if (status != TX_SUCCESS)
        {

            /* Byte Pool error.  */
            printf("ERROR #13\n");
            test_control_return(1);
        }

    } while (test_status == 1);

    /* Check status and make sure thread 3 is now at the front of the suspension list.  */
    if ((status != TX_SUCCESS) || (byte_pool_0.tx_byte_pool_suspension_list != &thread_4))
    {

        /* Byte Pool error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Call byte pool info with a NULL pointer.  */
    status =  tx_byte_pool_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte Pool error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Call byte pool info with an non-created pool pointer.  */
    byte_pool_2.tx_byte_pool_id =  0;
    status =  tx_byte_pool_info_get(&byte_pool_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check for error.  */
    if (status != TX_POOL_ERROR)
    {

        /* Byte Pool error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
#endif

    /* Get the byte pool information.   */
    status =  tx_byte_pool_info_get(&byte_pool_0, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    status += tx_byte_pool_info_get(&byte_pool_0, &name, &available, &fragments, &first_suspended, &suspended_count, &next_pool);

    /* Check the status.  */
    if ((status != TX_SUCCESS) || (available != byte_pool_0.tx_byte_pool_available) || (fragments != byte_pool_0.tx_byte_pool_fragments) || 
        (first_suspended != &thread_4) || (suspended_count != byte_pool_0.tx_byte_pool_suspended_count) || (next_pool != &byte_pool_0))
    {

        /* Byte Pool error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }


#ifdef TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, &allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);
    
    /* Check the status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Byte Pool error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }
    
    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(&byte_pool_1, &allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);
    
    /* Check the status.  */
    if (status != TX_PTR_ERROR)
    {

        /* Byte Pool error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(&byte_pool_0, &allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);
    
    /* Check the status.  */
    if ((status != TX_SUCCESS) || (allocates != byte_pool_0.tx_byte_pool_performance_allocate_count) || (releases != byte_pool_0.tx_byte_pool_performance_release_count) || 
        (fragments_searched != byte_pool_0.tx_byte_pool_performance_search_count) || (merges != byte_pool_0.tx_byte_pool_performance_merge_count) || 
        (splits != byte_pool_0.tx_byte_pool_performance_split_count) || (suspensions != byte_pool_0.tx_byte_pool_performance_suspension_count) || 
        (timeouts != byte_pool_0.tx_byte_pool_performance_timeout_count))
    {

        /* Byte Pool error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(&allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);
    
    /* Check the status.  */
    if ((status != TX_SUCCESS) || (allocates != _tx_byte_pool_performance_allocate_count) || (releases != _tx_byte_pool_performance_release_count) || 
        (fragments_searched != _tx_byte_pool_performance_search_count) || (merges != _tx_byte_pool_performance_merge_count) || 
        (splits != _tx_byte_pool_performance_split_count) || (suspensions != _tx_byte_pool_performance_suspension_count) || 
        (timeouts != _tx_byte_pool_performance_timeout_count))
    {

        /* Byte Pool error.  */
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

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(&byte_pool_0, &allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, &allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Get the byte pool performance information.  */
    status =  tx_byte_pool_performance_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(&allocates, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, &releases, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, TX_NULL, &fragments_searched, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, &merges, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, &splits, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &suspensions, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, &timeouts);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }

    /* Get the byte pool system performance information.  */
    status =  tx_byte_pool_performance_system_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);

    /* Check the status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        /* Byte Pool error.  */
        printf("ERROR #38\n");
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


        /* Get byte from pool.  */
        status =  tx_byte_allocate(&byte_pool_0, &pointer, 25, TX_WAIT_FOREVER);

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


        /* Get byte from pool.  */
        status =  tx_byte_allocate(&byte_pool_0, &pointer, 25, TX_WAIT_FOREVER);

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


        /* Get byte from pool.  */
        status =  tx_byte_allocate(&byte_pool_0, &pointer, 25, TX_WAIT_FOREVER);

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


        /* Get byte from pool.  */
        status =  tx_byte_allocate(&byte_pool_0, &pointer, 25, TX_WAIT_FOREVER);

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


        /* Get byte from pool.  */
        status =  tx_byte_allocate(&byte_pool_0, &pointer, 25, TX_WAIT_FOREVER);

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


        /* Get byte from pool.  */
        status =  tx_byte_allocate(&byte_pool_0, &pointer, 25, TX_WAIT_FOREVER);

        if (status != TX_SUCCESS)
            break;

        /* Increment the thread counter.  */
        thread_6_counter++;
    }
}
