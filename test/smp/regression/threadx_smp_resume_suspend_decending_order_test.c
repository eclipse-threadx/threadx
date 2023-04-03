/* Define the ThreadX SMP resume and suspend threads in accending order test.  */

#include   <stdio.h>
#include   "tx_api.h"

extern TX_THREAD    *_tx_thread_execute_ptr[TX_THREAD_SMP_MAX_CORES];


static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;
static TX_THREAD       thread_4;
static TX_THREAD       thread_5;
static TX_THREAD       thread_6;
static TX_THREAD       thread_7;
static TX_THREAD       thread_8;
static TX_THREAD       thread_9;
static TX_THREAD       thread_10;
static TX_THREAD       thread_11;
static TX_THREAD       thread_12;
static TX_THREAD       thread_13;
static TX_THREAD       thread_14;
static TX_THREAD       thread_15;
static TX_THREAD       thread_16;
static TX_THREAD       thread_17;
static TX_THREAD       thread_18;
static TX_THREAD       thread_19;
static TX_THREAD       thread_20;
static TX_THREAD       thread_21;
static TX_THREAD       thread_22;
static TX_THREAD       thread_23;
static TX_THREAD       thread_24;
static TX_THREAD       thread_25;
static TX_THREAD       thread_26;
static TX_THREAD       thread_27;
static TX_THREAD       thread_28;
static TX_THREAD       thread_29;
static TX_THREAD       thread_30;
static TX_THREAD       thread_31;

static ULONG           thread_run_counter[32];


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
void    threadx_smp_resume_suspend_decending_order_test(void *first_unused_memory)
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

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_1, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_2, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            3, 3, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_3, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #4\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_4, "thread 4", thread_entry, 4,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            4, 4, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_4, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_5, "thread 5", thread_entry, 5,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5, 5, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_5, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_6, "thread 6", thread_entry, 6,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            6, 6, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_6, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #7\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_7, "thread 7", thread_entry, 7,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            7, 7, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_7, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #8\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_8, "thread 8", thread_entry, 8,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            8, 8, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_8, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #9\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_9, "thread 9", thread_entry, 9,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            9, 9, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_9, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #9\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_10, "thread 10", thread_entry, 10,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            10, 10, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_10, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #11\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_11, "thread 11", thread_entry, 11,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            11, 11, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_11, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #12\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_12, "thread 12", thread_entry, 12,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            12, 12, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_12, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #13\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_13, "thread 13", thread_entry, 13,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            13, 13, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_13, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #14\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_14, "thread 14", thread_entry, 14,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            14, 14, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_14, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #15\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_15, "thread 15", thread_entry, 15,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_15, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #16\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_16, "thread 16", thread_entry, 16,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_16, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #17\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_17, "thread 17", thread_entry, 17,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_17, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #18\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_18, "thread 18", thread_entry, 18,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_18, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #19\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_19, "thread 19", thread_entry, 19,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            19, 19, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_19, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #20\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_20, "thread 20", thread_entry, 20,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            20, 20, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_20, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #21\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_21, "thread 21", thread_entry, 21,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            21, 21, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_21, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #22\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_22, "thread 22", thread_entry, 22,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            22, 22, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_22, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #23\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_23, "thread 23", thread_entry, 23,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            23, 23, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_23, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #24\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_24, "thread 24", thread_entry, 24,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            24, 24, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_24, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #25\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_25, "thread 25", thread_entry, 25,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            25, 25, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_25, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #26\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_26, "thread 26", thread_entry, 26,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            26, 26, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_26, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #27\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_27, "thread 27", thread_entry, 27,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            27, 27, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_27, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #28\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_28, "thread 28", thread_entry, 28,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            28, 28, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_28, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #29\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_29, "thread 29", thread_entry, 29,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            29, 29, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_29, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #30\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_30, "thread 30", thread_entry, 30,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            30, 30, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_30, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #31\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31, "thread 31", thread_entry, 31,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31, 0x0);      /* No exclusions! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #32\n");
        test_control_return(1);
    }

    /* Clear the thread run count array.  */
    for (i = 0; i < 32; i++)
    {
        thread_run_counter[i] =  0;
    }

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Resume-Suspend Decending Priority Order Test............ ERROR #33\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP Resume-Suspend Decending Priority Order Test............ ");

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_31);
    delay(31);
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_31)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #34\n");
        test_control_return(1);
    }
    
    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_30);
    delay(30);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_31)
                               || (_tx_thread_execute_ptr[2] != &thread_30) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_29);
    delay(29);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_31)
                               || (_tx_thread_execute_ptr[2] != &thread_30) || (_tx_thread_execute_ptr[3] != &thread_29)) 
    {

        /* Execution error.  */
        printf("ERROR #36\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_28);
    delay(28);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_28)
                               || (_tx_thread_execute_ptr[2] != &thread_30) || (_tx_thread_execute_ptr[3] != &thread_29)) 
    {

        /* Execution error.  */
        printf("ERROR #37\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_27);
    delay(27);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_28)
                               || (_tx_thread_execute_ptr[2] != &thread_27) || (_tx_thread_execute_ptr[3] != &thread_29)) 
    {

        /* Execution error.  */
        printf("ERROR #38\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_26);
    delay(26);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_28)
                               || (_tx_thread_execute_ptr[2] != &thread_27) || (_tx_thread_execute_ptr[3] != &thread_26)) 
    {

        /* Execution error.  */
        printf("ERROR #39\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_25);
    delay(25);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_25)
                               || (_tx_thread_execute_ptr[2] != &thread_27) || (_tx_thread_execute_ptr[3] != &thread_26)) 
    {

        /* Execution error.  */
        printf("ERROR #40\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_24);
    delay(24);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_25)
                               || (_tx_thread_execute_ptr[2] != &thread_24) || (_tx_thread_execute_ptr[3] != &thread_26)) 
    {

        /* Execution error.  */
        printf("ERROR #41\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_23);
    delay(23);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_25)
                               || (_tx_thread_execute_ptr[2] != &thread_24) || (_tx_thread_execute_ptr[3] != &thread_23)) 
    {

        /* Execution error.  */
        printf("ERROR #42\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_22);
    delay(22);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_22)
                               || (_tx_thread_execute_ptr[2] != &thread_24) || (_tx_thread_execute_ptr[3] != &thread_23)) 
    {

        /* Execution error.  */
        printf("ERROR #43\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_21);
    delay(21);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_22)
                               || (_tx_thread_execute_ptr[2] != &thread_21) || (_tx_thread_execute_ptr[3] != &thread_23)) 
    {

        /* Execution error.  */
        printf("ERROR #44\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_20);
    delay(20);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_22)
                               || (_tx_thread_execute_ptr[2] != &thread_21) || (_tx_thread_execute_ptr[3] != &thread_20)) 
    {

        /* Execution error.  */
        printf("ERROR #45\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_19);
    delay(19);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_19)
                               || (_tx_thread_execute_ptr[2] != &thread_21) || (_tx_thread_execute_ptr[3] != &thread_20)) 
    {

        /* Execution error.  */
        printf("ERROR #46\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_18);
    delay(18);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_19)
                               || (_tx_thread_execute_ptr[2] != &thread_18) || (_tx_thread_execute_ptr[3] != &thread_20)) 
    {

        /* Execution error.  */
        printf("ERROR #47\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_17);
    delay(17);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_19)
                               || (_tx_thread_execute_ptr[2] != &thread_18) || (_tx_thread_execute_ptr[3] != &thread_17)) 
    {

        /* Execution error.  */
        printf("ERROR #48\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_16);
    delay(16);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16)
                               || (_tx_thread_execute_ptr[2] != &thread_18) || (_tx_thread_execute_ptr[3] != &thread_17)) 
    {

        /* Execution error.  */
        printf("ERROR #49\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_15);
    delay(15);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16)
                               || (_tx_thread_execute_ptr[2] != &thread_15) || (_tx_thread_execute_ptr[3] != &thread_17)) 
    {

        /* Execution error.  */
        printf("ERROR #50\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_14);
    delay(14);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_16)
                               || (_tx_thread_execute_ptr[2] != &thread_15) || (_tx_thread_execute_ptr[3] != &thread_14)) 
    {

        /* Execution error.  */
        printf("ERROR #51\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_13);
    delay(13);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_13)
                               || (_tx_thread_execute_ptr[2] != &thread_15) || (_tx_thread_execute_ptr[3] != &thread_14)) 
    {

        /* Execution error.  */
        printf("ERROR #52\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_12);
    delay(12);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_13)
                               || (_tx_thread_execute_ptr[2] != &thread_12) || (_tx_thread_execute_ptr[3] != &thread_14)) 
    {

        /* Execution error.  */
        printf("ERROR #53\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_11);
    delay(11);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_13)
                               || (_tx_thread_execute_ptr[2] != &thread_12) || (_tx_thread_execute_ptr[3] != &thread_11)) 
    {

        /* Execution error.  */
        printf("ERROR #54\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_10);
    delay(10);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_10)
                               || (_tx_thread_execute_ptr[2] != &thread_12) || (_tx_thread_execute_ptr[3] != &thread_11)) 
    {

        /* Execution error.  */
        printf("ERROR #55\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_9);
    delay(9);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_10)
                               || (_tx_thread_execute_ptr[2] != &thread_9) || (_tx_thread_execute_ptr[3] != &thread_11)) 
    {

        /* Execution error.  */
        printf("ERROR #56\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_8);
    delay(8);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_10)
                               || (_tx_thread_execute_ptr[2] != &thread_9) || (_tx_thread_execute_ptr[3] != &thread_8)) 
    {

        /* Execution error.  */
        printf("ERROR #57\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_7);
    delay(7);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_7)
                               || (_tx_thread_execute_ptr[2] != &thread_9) || (_tx_thread_execute_ptr[3] != &thread_8)) 
    {

        /* Execution error.  */
        printf("ERROR #58\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_6);
    delay(6);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_7)
                               || (_tx_thread_execute_ptr[2] != &thread_6) || (_tx_thread_execute_ptr[3] != &thread_8)) 
    {

        /* Execution error.  */
        printf("ERROR #59\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_5);
    delay(5);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_7)
                               || (_tx_thread_execute_ptr[2] != &thread_6) || (_tx_thread_execute_ptr[3] != &thread_5)) 
    {

        /* Execution error.  */
        printf("ERROR #60\n");
        test_control_return(1);
    }
    
    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_4);
    delay(4);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_4)
                               || (_tx_thread_execute_ptr[2] != &thread_6) || (_tx_thread_execute_ptr[3] != &thread_5)) 
    {

        /* Execution error.  */
        printf("ERROR #61\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_3);
    delay(3);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_4)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_5)) 
    {

        /* Execution error.  */
        printf("ERROR #62\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_2);
    delay(2);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_4)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #63\n");
        test_control_return(1);
    }

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_1);
    delay(1);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #64\n");
        test_control_return(1);
    }

    /* Suspend thread in decending priority.  */
    status =   tx_thread_suspend(&thread_31);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #65\n");
        test_control_return(1);
    }
    
    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_30);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #66\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_29);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #67\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_28);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #68\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_27);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #69\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_26);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #70\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_25);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #71\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_24);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #72\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_23);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #73\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_22);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #74\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_21);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #75\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_20);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #76\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_19);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #77\n");
        test_control_return(1);
    }
    
    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_18);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #78\n");
        test_control_return(1);
    }
    
    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_17);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #79\n");
        test_control_return(1);
    }
    
    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_16);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #80\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_15);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #81\n");
        test_control_return(1);
    }
    
    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_14);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #82\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_13);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #83\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_12);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #84\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_11);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #85\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_10);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #86\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_9);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #87\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_8);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #88\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_7);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #89\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_6);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #90\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_5);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #91\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_4);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != &thread_3) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #92\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_3);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != &thread_2)) 
    {

        /* Execution error.  */
        printf("ERROR #93\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_2);

    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != &thread_1)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #94\n");
        test_control_return(1);
    }

    /* Suspend thread in accending priority.  */
    status =   tx_thread_suspend(&thread_1);
 
    /* Check for the correct results.  */
    if ((status != TX_SUCCESS) || (_tx_thread_execute_ptr[0] != &thread_0) || (_tx_thread_execute_ptr[1] != TX_NULL)
                               || (_tx_thread_execute_ptr[2] != TX_NULL) || (_tx_thread_execute_ptr[3] != TX_NULL)) 
    {

        /* Execution error.  */
        printf("ERROR #95\n");
        test_control_return(1);
    }

    
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
