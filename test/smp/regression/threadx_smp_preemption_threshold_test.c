/* Define the ThreadX SMP preemption-threshold test.  */

#include   <stdio.h>
#include   "tx_api.h"

extern TX_THREAD    *_tx_thread_execute_ptr[TX_THREAD_SMP_MAX_CORES];


static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_5;
static TX_THREAD       thread_16;
static TX_THREAD       thread_16_pt5;
static TX_THREAD       thread_18;
static TX_THREAD       thread_23_pt17;
static TX_THREAD       thread_25;
static TX_THREAD       thread_27_pt24;
static TX_THREAD       thread_31;

static ULONG           thread_run_counter[10];


static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_entry(ULONG thread_input);
static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void   delay(UINT i)
{

    /* Wait until the thread runs!  */
    while (thread_run_counter[i] == 0)
    {
    }
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_preemption_threshold_test(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;
UINT    i;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            0, 0, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_0, 0xE);      /* Core 0 only! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_1, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_5, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_16, "thread 16", thread_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_16, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_16_pt5, "thread 16 PT5", thread_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 5, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_16_pt5, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_18, "thread 18", thread_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_18, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_23_pt17, "thread 23 PT17", thread_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            23, 17, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_23_pt17, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #7\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_25, "thread 25", thread_entry, 7,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            25, 25, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_25, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #8\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_27_pt24, "thread 27 PT24", thread_entry, 8,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            27, 24, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_27_pt24, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #9\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31, "thread 31", thread_entry, 9,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31, 0);      /* Any core! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #10\n");
        test_control_return(1);
    }

    /* Clear the thread run count array.  */
    for (i = 0; i < 10; i++)
    {
        thread_run_counter[i] =  0;
    }

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Preemption-Threshold Test............................... ERROR #11\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP Preemption-Threshold Test............................... ");

	/* This test is only useful when preemption-threshold is enabled.  */
#ifndef TX_DISABLE_PREEMPTION_THRESHOLD 

	/* Resume thread.  */
    status =  tx_thread_resume(&thread_31);
    delay(9);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_31)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }
    
    /* Resume thread.  */
    status =  tx_thread_resume(&thread_27_pt24);
    delay(8);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_27_pt24)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_23_pt17);
    delay(6);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_23_pt17)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_16_pt5);
    delay(4);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16_pt5)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_16);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16_pt5)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_25);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16_pt5)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_18);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16_pt5)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_5);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16_pt5)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Resume thread.  */
    status =  tx_thread_resume(&thread_1);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16_pt5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Suspend Thread 16 pt5.  */
    status =  tx_thread_suspend(&thread_16_pt5);
    
    delay(2);
    delay(3);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_16)) 
    {

        /* Execution error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }
    

    /* Suspend Thread 16.  */
    status =  tx_thread_suspend(&thread_16);
   
    delay(6);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_23_pt17)) 
    {

        /* Execution error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    /* Suspend Thread 23 pt 17.  */
    status =  tx_thread_suspend(&thread_23_pt17);
   
    delay(5);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_18)) 
    {

        /* Execution error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }


    /* Suspend Thread 18.  */
    status =  tx_thread_suspend(&thread_18);
   
    delay(8);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_27_pt24)) 
    {

        /* Execution error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Suspend Thread 27 pt 24.  */
    status =  tx_thread_suspend(&thread_27_pt24);
   
    delay(7);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_25)) 
    {

        /* Execution error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

    /* Suspend Thread 25.  */
    status =  tx_thread_suspend(&thread_25);
   
    delay(9);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_31)) 
    {

        /* Execution error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Resume thread 16 pt 5.  */
    status =  tx_thread_resume(&thread_16_pt5);
    
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_31)) 
    {

        /* Execution error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }
    
    /* Suspend thread 16 pt 5.  */
    status =  tx_thread_suspend(&thread_16_pt5);
    
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != &thread_31)) 
    {

        /* Execution error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Suspend thread 31.  */
    status =  tx_thread_suspend(&thread_31);
    
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != &thread_1) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Suspend thread 1.  */
    status =  tx_thread_suspend(&thread_1);
    
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_5)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

    /* Suspend thread 5.  */
    status =  tx_thread_suspend(&thread_5);
    
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != TX_NULL)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }
#endif

    /* Successful test.  */
    printf("SUCCESS!\n");
        
    test_control_return(0);
}


static void    thread_entry(ULONG thread_input)
{

    /* Increment the run counter.  */
    thread_run_counter[thread_input]++;

    while(1)
    {
    
        tx_thread_identify();   

		/* Indicate the thread is running... */
        thread_run_counter[thread_input]++;
    }
}
