/* Define the ThreadX SMP non-trivial scheduling test.  */

#include   <stdio.h>
#include   "tx_api.h"



static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;


static ULONG       thread_0_counter;
static ULONG       thread_1_counter;
static ULONG       thread_2_counter;
static ULONG       thread_3_counter;


extern TX_THREAD    *_tx_thread_execute_ptr[TX_THREAD_SMP_MAX_CORES];

static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);





/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_non_trivial_scheduling_test(void *first_unused_memory)
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
    status +=   tx_thread_smp_core_exclude(&thread_0, 0xE);      /* Only allow core 0 for now */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Non-Trivial Scheduling Test............................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_1, 0x9);      /* Exclude core 2 and 1 */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Non-Trivial Scheduling Test............................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_2, 0xB);      /* Exclude core 3, 1 and 0 */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Non-Trivial Scheduling Test............................. ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_3, 0xE);      /* Exclude core  0 */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Non-Trivial Scheduling Test............................. ERROR #4\n");
        test_control_return(1);
    }

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Non-Trivial Scheduling Test............................. ERROR #5\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
UINT    original_threshold;


    /* Inform user.  */
    printf("Running SMP Non-Trivial Scheduling Test............................. ");

    /* Move enable core 0 and 1.  */
    status =   tx_thread_smp_core_exclude(&thread_0, 0xC);      /* Allow core 0 and 1 */

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0))
    {

        /* Execution error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Resume thread 1.  */
    status =   tx_thread_resume(&thread_1);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Resume thread 2.  */
    status =   tx_thread_resume(&thread_2);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_2) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Resume thread 3.  */
    status =   tx_thread_resume(&thread_3);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_2) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Suspend thread 2 and cause a rebalance of the execution list.  */
    status =  tx_thread_suspend(&thread_2);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_3) || (_tx_thread_execute_ptr[1] != &thread_0)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #10");
        test_control_return(1);
    }

    /* Now suspend threads 3 and 1.  */
    status =  tx_thread_suspend(&thread_3);
    status += tx_thread_suspend(&thread_1);
    
    /* Use preemption-threshold to test the rebalance routine.  */
    status += tx_thread_preemption_change(&thread_3, 2, &original_threshold);

    /* Move thread 0 back to core 0 and then allow core 1 again.  */
    status += tx_thread_smp_core_exclude(&thread_0, 0xE);
    status += tx_thread_smp_core_exclude(&thread_0, 0xC);
    
    /* Make sure threads 1 defaults to core 1.  */
    status += tx_thread_smp_core_exclude(&thread_1, 0xD);
    status += tx_thread_smp_core_exclude(&thread_1, 0x9);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != TX_NULL)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #11");
        test_control_return(1);
    }

    /* Resume thread 1.  */
    status =   tx_thread_resume(&thread_1);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Resume thread 2.  */
    status =   tx_thread_resume(&thread_2);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_2) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Resume thread 3.  */
    status =   tx_thread_resume(&thread_3);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_2) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Suspend thread 2 and cause a rebalance of the execution list.  */
    status =  tx_thread_suspend(&thread_2);

	/* With preemption-threshold disabled, thread_3 can not run since preemption-threshold is set to 0 and there is already a 
	   zero priority thread running.  */
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 
	/* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_3) || (_tx_thread_execute_ptr[1] != &thread_0)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
#else
	/* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
#endif
	{

        /* Execution error.  */
        printf("ERROR #15");
        test_control_return(1);
    }

    /* Now suspend threads 3 and 1.  */
    status =  tx_thread_suspend(&thread_3);
    status += tx_thread_suspend(&thread_1);

#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 
    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != TX_NULL) || (_tx_thread_execute_ptr[1] != &thread_0)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
#else
    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != TX_NULL)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
#endif
	{

        /* Execution error.  */
        printf("ERROR #16");
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
        tx_thread_identify();
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    while(1)
    {
        thread_2_counter++;
        tx_thread_identify();
    }
}

static void    thread_3_entry(ULONG thread_input)
{

    while(1)
    {
        thread_3_counter++;
        tx_thread_identify();
    }
}


