/* Define the ThreadX SMP multiple threads excluded to one core test.  */

#include   <stdio.h>
#include   "tx_api.h"



static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;
static TX_THREAD       thread_4;
static TX_THREAD       thread_5;
static TX_THREAD       thread_6;
static TX_THREAD       thread_7;


static ULONG       thread_0_counter;
static ULONG       thread_1_counter;
static ULONG       thread_2_counter;
static ULONG       thread_3_counter;
static ULONG       thread_4_counter;
static ULONG       thread_5_counter;
static ULONG       thread_6_counter;
static ULONG       thread_7_counter;


static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);
static void    thread_5_entry(ULONG thread_input);
static void    thread_6_entry(ULONG thread_input);
static void    thread_7_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);





/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_multiple_threads_one_core_test(void *first_unused_memory)
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
    status +=   tx_thread_smp_core_exclude(&thread_0, 0xE);      /* Core 0 only! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_1, 0xD);      /* Only core 1! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_2, 0xB);      /* Only core 2! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_3, 0x7);      /* Only core 3! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_4, 0xE);      /* Only core 0! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_5_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_5, 0xD);      /* Only core 1! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6, "thread 6", thread_6_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_6, 0xB);      /* Only core 2! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #7\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_7, "thread 7", thread_7_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_7, 0x7);      /* Only core 3! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #8\n");
        test_control_return(1);
    }

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Multiple Threads One Core Test.......................... ERROR #9\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP Multiple Threads One Core Test.......................... ");

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_1);
    status +=  tx_thread_resume(&thread_2);
    status +=  tx_thread_resume(&thread_3);
    status +=  tx_thread_resume(&thread_4);
    status +=  tx_thread_resume(&thread_5);
    status +=  tx_thread_resume(&thread_6);
    status +=  tx_thread_resume(&thread_7);

    /* Suspend this thread to let the others run.  */
    tx_thread_sleep(5);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (error) || (thread_1_counter != 1) || (tx_thread_smp_core_get() != 0) ||
        (thread_2_counter != 1) || (thread_3_counter != 1) || (thread_4_counter != 1) || 
        (thread_5_counter != 1) || (thread_6_counter != 1) || (thread_7_counter != 1))
    {

        /* Execution error.  */
        printf("ERROR #10\n");
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
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 1)
            error++;
    
        thread_1_counter++;
        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 2)
            error++;

        thread_2_counter++;
        tx_thread_suspend(&thread_2);
    }
}


static void    thread_3_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 3)
            error++;

        thread_3_counter++;
        tx_thread_suspend(&thread_3);
    }
}


static void    thread_4_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 0)
            error++;

        thread_4_counter++;
        tx_thread_suspend(&thread_4);
    }
}


static void    thread_5_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 1)
            error++;

        thread_5_counter++;
        tx_thread_suspend(&thread_5);
    }
}


static void    thread_6_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 2)
            error++;

        thread_6_counter++;
        tx_thread_suspend(&thread_6);
    }
}


static void    thread_7_entry(ULONG thread_input)
{

    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 3)
            error++;

        thread_7_counter++;
        tx_thread_suspend(&thread_7);
    }
}
