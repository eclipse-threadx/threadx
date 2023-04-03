/* This test is designed to test multiple threads sleeping for 33 ticks.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;
static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;
//static unsigned long   thread_3_counter =  0;
static TX_THREAD       thread_3;


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
void    threadx_thread_multiple_sleep_application_define(void *first_unused_memory)
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
            16, 16, 3, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Sleep for 33 Test.................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 3, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Sleep for 33 Test.................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, 3, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Sleep for 33 Test.................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, 3, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Multiple Thread Sleep for 33 Test.................... ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 0 counter.  */
        thread_0_counter++;

        /* Sleep for a couple ticks.  */
        tx_thread_sleep(33);
    }
}


static void    thread_1_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 1 counter.  */
        thread_1_counter++;

        /* Sleep for a couple ticks.  */
        tx_thread_sleep(33);
    }
}


static void    thread_2_entry(ULONG thread_input)
{


    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 0 counter.  */
        thread_2_counter++;

        /* Sleep for a couple ticks.  */
        tx_thread_sleep(33);
    }
}


static void    thread_3_entry(ULONG thread_input)
{


    /* Inform user.  */
    printf("Running Thread Multiple Thread Sleep for 33 Test.................... ");

    /* Clear the tick count. */
    tx_time_set(0);

    /* Sleep for 100 ticks (+1 in case tick before threads 0,1,2 have run). */
    tx_thread_sleep(101);

    /* Determine if the sleep was accurate.  */
    if ((thread_0_counter == 4) && (thread_1_counter == 4) &&
        (thread_2_counter == 4))
    {

        /* Successful Multiple Sleep test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
    else
    {

        /* Thread Multiple Sleep error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
}

