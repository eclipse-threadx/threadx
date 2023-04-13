/* This test is designed to test thread termination of a thread in a sleep condition.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static unsigned long   thread_1_counter =  0;
static unsigned long   thread_1_enter =    0;
static unsigned long   thread_1_exit =     0;
static TX_THREAD       thread_1;

extern UINT     _tx_thread_preempt_disable;

/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);



static void   entry_exit_notify(TX_THREAD *thread_ptr, UINT type)
{

    /* Check for the appropriate thread.  */
    if (thread_ptr != &thread_1)
        return;
        
    /* Check for type.  */
    if (type == TX_THREAD_ENTRY)
        thread_1_enter++;
    else if (type == TX_THREAD_EXIT)
        thread_1_exit++;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_sleep_terminate_application_define(void *first_unused_memory)
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

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Suspend Terminate Test............................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Suspend Terminate Test............................... ERROR #2\n");
        test_control_return(1);
    }

    /* Setup the notify call to test that logic.  */
    status += tx_thread_entry_exit_notify(&thread_1, entry_exit_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Suspend Terminate Test............................... ERROR #3\n");
        test_control_return(1);
    }

#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Thread Suspend Terminate Test............................... ERROR #4\n");
        test_control_return(1);
    }

#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Thread Suspend Terminate Test............................... ");

    /* Increment run counter.  */
    thread_0_counter++;

    /* Sleep to allow lower-priority thread 1 to run.  */
    tx_thread_sleep(5);

    /* Now terminate thread 1.  */
    status =  tx_thread_terminate(&thread_1);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (_tx_thread_preempt_disable))
    {

        /* Terminate error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Now try to suspend a terminated thread.  */
    status =  tx_thread_suspend(&thread_1);
    
    /* Check status.  */
    if (status != TX_SUSPEND_ERROR)
    {

        /* Suspend error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }
    
    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    while(1)
    {

        /* Increment run counter.  */
        thread_1_counter++;

        /* Suspend thread 1.  */
        status =  tx_thread_sleep(100);

        /* Check status.  */
        if (status != TX_SUCCESS)
        {
            thread_1_counter =  0;  /* Make an error!  */ 
            return;
        }
    }
}


