/* Define the ThreadX SMP one thread dynamic thread exclusion test.  */

#include   <stdio.h>
#include   "tx_api.h"



static TX_THREAD       thread_0;


static ULONG       thread_0_counter;


static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);





/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_one_thread_dynamic_exclusion_test(void *first_unused_memory)
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

        printf("Running SMP One Thread Dynamic Exclusion Test....................... ERROR #1\n");
        test_control_return(1);
    }


	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP One Thread Dynamic Exclusion Test....................... ERROR #2\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP One Thread Dynamic Exclusion Test....................... ");

    /* Move to core 1.  */
    status =   tx_thread_smp_core_exclude(&thread_0, 0xD);      /* Only core 1! */

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (tx_thread_smp_core_get() != 1))
    {

        /* Execution error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Move to core 2.  */
    status =   tx_thread_smp_core_exclude(&thread_0, 0xB);      /* Only core 2! */

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (tx_thread_smp_core_get() != 2))
    {

        /* Execution error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Move to core 3.  */
    status =   tx_thread_smp_core_exclude(&thread_0, 0x7);      /* Only core 3! */

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (tx_thread_smp_core_get() != 3))
    {

        /* Execution error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Move back to core 0.  */
    status =   tx_thread_smp_core_exclude(&thread_0, 0xE);      /* Only core 0! */

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (tx_thread_smp_core_get() != 0))
    {

        /* Execution error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
    else
    {
    
        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

