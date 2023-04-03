/* Define the ThreadX SMP rebalance exclusion test.  */

#include   <stdio.h>
#include   "tx_api.h"



static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_31k;


static ULONG       thread_0_counter;
static ULONG       thread_1_counter;
static ULONG       thread_2_counter;
static ULONG       thread_31k_counter;


static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_31k_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);





/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_rebalance_exclustion_test(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            0, 0, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_0, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Rebalance Exclusion Test................................ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_1, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Rebalance Exclusion Test................................ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_2, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Rebalance Exclusion Test................................ ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31k, "thread 31k", thread_31k_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 15, 16, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31k, 0xE);      /* Core 0 only! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Rebalance Exclusion Test................................ ERROR #4\n");
        test_control_return(1);
    }

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Rebalance Exclusion Test................................ ERROR #5\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP Rebalance Exclusion Test................................ ");

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_1);
    status +=  tx_thread_resume(&thread_2);
    status +=  tx_thread_resume(&thread_31k);

    /* Suspend this thread to let the others run.  */
    tx_thread_sleep(5);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (error) || (thread_1_counter != 1) || 
        (thread_2_counter != 1) || (thread_31k_counter != 1))
    {

        /* Execution error.  */
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

    while(1)
    {
    
        thread_1_counter++;
        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    while(1)
    {
    
        thread_2_counter++;
        tx_thread_suspend(&thread_2);
    }
}


static void    thread_31k_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 0)
            error++;
    
        thread_31k_counter++;
        tx_thread_suspend(&thread_31k);
    }
}
