/* This test is designed to test the mutex suspension prioritization.  */

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


static TX_MUTEX        mutex_0;
static TX_MUTEX        mutex_1;

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


/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

    /* Determine if the test case we are looking for is present.  */
    if ((_tx_thread_preempt_disable) && (test_status == 1))
    {

        /* Determine if thread 3 is at the front of the suspension list.  */
        if (mutex_0.tx_mutex_suspension_list == &thread_3)
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
void    threadx_mutex_prioritize_application_define(void *first_unused_memory)
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

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            14, 14, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 4, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, 100, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #7\n");
        test_control_return(1);
    }

    /* Create a mutex.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Prioritize Test....................................... ERROR #8\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Mutex Prioritize Test....................................... ");

    /* Increment thread 0 counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to prioritize a NULL mutex.  */
    status =  tx_mutex_prioritize(TX_NULL);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to prioritize a non-created mutex.  */
    mutex_1.tx_mutex_id =  0;
    status =  tx_mutex_prioritize(&mutex_1);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }
#endif

    /* Prioritize the mutex with no suspended threads!  */
    status =  tx_mutex_prioritize(&mutex_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Grab the mutex so it is owned by this thread.  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Resume other threads run.  */
    tx_thread_resume(&thread_1);
    tx_thread_resume(&thread_2);

    /* Other threads should now be suspended on the mutex.  Thread 1 should be
       in front of thread 2 since it was suspended first.  */
    if (mutex_0.tx_mutex_suspension_list != &thread_1)
    {

        /* Mutex error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Prioritize the mutex to test it out!  */
    status =  tx_mutex_prioritize(&mutex_0);

    /* Check status and make sure thread 2 is now at the front of the list.  */
    if ((status != TX_SUCCESS) || (mutex_0.tx_mutex_suspension_list != &thread_2))
    {

        /* Mutex error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Prioritize the mutex again to make sure nothing has changed!  */
    status =  tx_mutex_prioritize(&mutex_0);

    /* Check status and make sure thread 2 is now at the front of the list.  */
    if ((status != TX_SUCCESS) || (mutex_0.tx_mutex_suspension_list != &thread_2))
    {

        /* Mutex error.  */
        printf("ERROR #14a\n");
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
    status =  tx_mutex_prioritize(&mutex_0);

    /* Check status and make sure thread 3 is at the front of the suspension list.  */
    if ((status != TX_SUCCESS) || (mutex_0.tx_mutex_suspension_list != &thread_3))
    {

        /* Mutex error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Now loop to test the interrupt of the prioritize loop logic.  */
    test_status =  1;
    test_isr_dispatch =  test_isr;  
    do
    {

        /* Prioritize the mutex suspension list.  */
        status =  tx_mutex_prioritize(&mutex_0);

        /* Check status and make sure thread 1 is terminated.  */
        if (status != TX_SUCCESS)
        {

            /* Mutex error.  */
            printf("ERROR #16\n");
            test_control_return(1);
        }

    } while (test_status == 1);
    
    /* Now determine if thread 4 is at the front of the list... It should be!  */
    if (mutex_0.tx_mutex_suspension_list != &thread_4)
    {    

        /* Mutex error.  */
        printf("ERROR #17\n");
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


    while (1)
    {
    
        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Release the mutex.   */
        status += tx_mutex_put(&mutex_0);
    
        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_1_counter++;

        /* Self suspend.  */
        tx_thread_suspend(&thread_1); 
    }
}


static void    thread_2_entry(ULONG thread_input)
{

UINT    status;


    while (1)
    {
    
        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Release the mutex.   */
        status += tx_mutex_put(&mutex_0);
    
        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_2_counter++;

        /* Self suspend.  */
        tx_thread_suspend(&thread_2); 
    }
}


static void    thread_3_entry(ULONG thread_input)
{

UINT    status;


    while (1)
    {
    
        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Release the mutex.   */
        status += tx_mutex_put(&mutex_0);
    
        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_3_counter++;

        /* Self suspend.  */
        tx_thread_suspend(&thread_3); 
    }
}


static void    thread_4_entry(ULONG thread_input)
{

UINT    status;


    while (1)
    {
    
        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Release the mutex.   */
        status += tx_mutex_put(&mutex_0);
    
        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_4_counter++;

        /* Self suspend.  */
        tx_thread_suspend(&thread_4); 
    }
}


static void    thread_5_entry(ULONG thread_input)
{

UINT    status;


    while (1)
    {
    
        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Release the mutex.   */
        status += tx_mutex_put(&mutex_0);
    
        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_5_counter++;

        /* Self suspend.  */
        tx_thread_suspend(&thread_5); 
    }
}


static void    thread_6_entry(ULONG thread_input)
{

UINT    status;


    while (1)
    {
    
        /* Suspend on the mutex. */
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Release the mutex.   */
        status += tx_mutex_put(&mutex_0);
    
        /* Did we get the right status?  */
        if (status == TX_SUCCESS)
            thread_6_counter++;

        /* Self suspend.  */
        tx_thread_suspend(&thread_6); 
    }
}

