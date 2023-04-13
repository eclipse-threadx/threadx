/* This test is designed to test thread terminate (self, and other), thread delete,
   and thread identify services.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static unsigned long   thread_1_enter =  0;
static unsigned long   thread_1_exit =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static TX_THREAD       thread_3;
static unsigned long   thread_3_enter =  0;
static unsigned long   thread_3_exit =  0;


static TX_SEMAPHORE    semaphore_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);


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


static void   entry_exit_notify3(TX_THREAD *thread_ptr, UINT type)
{

    /* Check for the appropriate thread.  */
    if (thread_ptr != &thread_3)
        return;
        
    /* Check for type.  */
    if (type == TX_THREAD_ENTRY)
        thread_3_enter++;
    else if (type == TX_THREAD_EXIT)
        thread_3_exit++;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_terminate_delete_application_define(void *first_unused_memory)
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
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #2\n");
        test_control_return(1);
    }

    /* Setup the notify call to test that logic.  */
    status =  tx_thread_entry_exit_notify(&thread_1, entry_exit_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #3\n");
        test_control_return(1);
    }

#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #4\n");
        test_control_return(1);
    }


#endif

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 3,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            12, 12, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #6\n");
        test_control_return(1);
    }

    /* Setup the notify call to test that logic.  */
    status =  tx_thread_entry_exit_notify(&thread_3, entry_exit_notify3);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #7\n");
        test_control_return(1);
    }

#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #8\n");
        test_control_return(1);
    }

#endif


    /* Create a semaphore for thread 3 to suspend on.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Terminate and Delete Test............................ ERROR #9\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Thread Terminate and Delete Test............................ ");

    /* Let other threads execute.  */
    tx_thread_relinquish();

    /* Make sure thread 1 is terminated and thread 2 is suspended.  */
    if ((thread_1.tx_thread_state != TX_TERMINATED) ||
#ifndef TX_DISABLE_NOTIFY_CALLBACKS
        (thread_1_enter != 1) || (thread_1_exit != 1) ||
#else
        (thread_1_enter != 0) || (thread_1_exit != 0) ||
#endif
        (thread_2.tx_thread_state !=  TX_SUSPENDED))
    {

        /* Thread error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* At this point, terminate thread 2 which should be in a suspended state 
       right now.  */
    status =  tx_thread_terminate(&thread_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Delete thread 1 (thread 1 alread terminated) and 2.  */
    status =  tx_thread_delete(&thread_2);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    status =  tx_thread_delete(&thread_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* At this point, terminate thread 3 which should be in a suspended state 
       on the semaphore right now.  */
    status =  tx_thread_terminate(&thread_3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Delete thread 3.  */
    status =  tx_thread_delete(&thread_3);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Thread error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

     /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

static void    thread_1_entry(ULONG thread_input)
{

    /* Test identity.  */
    if (tx_thread_identify() != &thread_1)
        return;

    /* Increment the thread counter.  */
    thread_1_counter++;

    /* Terminate self.  */
    tx_thread_terminate(&thread_1);
}

static void    thread_2_entry(ULONG thread_input)
{

    /* Test identity.  */
    if (tx_thread_identify() != &thread_2)
        return;

    /* Increment thread counter.  */
    thread_2_counter++;

    /* Suspend thread.  */
    tx_thread_suspend(&thread_2);
}


static void    thread_3_entry(ULONG thread_input)
{

    /* Get the semaphore with wait forever!  */
    tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

    /* Increment thread counter.  */
    thread_2_counter++;
}

