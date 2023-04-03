/* This test is designed to test the interrupt control service call avaialbe to the 
   application.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_interrupt_control_application_define(void *first_unused_memory)
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

        printf("Running Interrupt Control Test...................................... ERROR #1\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    saved_interrupt_posture;


    /* Inform user.  */
    printf("Running Interrupt Control Test...................................... ");

    /* Lockout interrupts.  */
    saved_interrupt_posture =  tx_interrupt_control(TX_INT_DISABLE);

    /* Increment the thread counter.  */
    thread_0_counter++;

    /* Restore interrupts.  */
    saved_interrupt_posture =  tx_interrupt_control(saved_interrupt_posture);

    /* Sleep to make sure interrupts now work.  */
    tx_thread_sleep(2);

    /* Check to make sure the returned interrupt type works.  */
    if (saved_interrupt_posture != TX_INT_DISABLE)
    {

        /* Interrupt control error.  */
        printf("ERROR #2\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

