/* This test is designed to see if one thread can be created, executed, and 
   return to the thread shell function.  The thread shell function places 
   the thread in a finished state.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"


static unsigned long   thread_0_counter =  0;
static unsigned long   thread_0_enter =  0;
static unsigned long   thread_0_exit =  0;
static unsigned long   thread_1_counter =  0;

static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       *saved_ptr;
static ULONG           saved_count;

/* Define task prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void   entry_exit_notify(TX_THREAD *thread_ptr, UINT type)
{

    /* Check for the appropriate thread.  */
    if (thread_ptr != &thread_0)
        return;
        
    /* Check for type.  */
    if (type == TX_THREAD_ENTRY)
        thread_0_enter++;
    else if (type == TX_THREAD_EXIT)
        thread_0_exit++;
}



/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_completed_application_define(void *first_unused_memory)
#endif
{

UINT     status;
CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
                        pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Setup the notify call to test that logic.  */
    status += tx_thread_entry_exit_notify(&thread_0, entry_exit_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Completed Test....................................... ERROR #1\n");
        test_control_return(1);
    }

#else

    /* Check for status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Thread Completed Test....................................... ERROR #2\n");
        test_control_return(1);
    }

#endif

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
                        pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Completed Test....................................... ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
                        pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Completed Test....................................... ERROR #4\n");
        test_control_return(1);
    }

    /* Move the created pointer to thread 1 to test the delete path fully.  */
    saved_ptr =  _tx_thread_created_ptr;
    _tx_thread_created_ptr =  &thread_0;
}



/* Define the test thread.  */

static void    thread_0_entry(ULONG thread_input)
{

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Fall through to the return in order to place the thread in a finished
       state.  */
}


static void    thread_1_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Thread Completed Test....................................... ");

    /* Increment thread 1 counter.  */
    thread_1_counter++;

    /* Attempt to delete thread 2, which is in the wrong stat for deleting.  */
    status =  tx_thread_delete(&thread_2);
    
    /* Check for the proper status.  */
    if (status != TX_DELETE_ERROR)
    {
    
        /* Thread delete error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to suspend thread 0, which is in a completed state.  */
    status =  tx_thread_suspend(&thread_0);
    
    /* Check for the correct status.  */
    if (status != TX_SUSPEND_ERROR)
    {
    
        /* Thread suspend error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to delete thread 0.  */
    status =  tx_thread_delete(&thread_0);
    
    /* Check for the proper status.  */
    if (status != TX_SUCCESS)
    {
    
        /* Thread delete error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Sleep to let thread 2 run.  */
    tx_thread_sleep(2);
    
    /* Save the created count.  */
    saved_count =  _tx_thread_created_count;
    
    /* Now setup things so we can fake a delete of one thread.  */
    _tx_thread_created_ptr =  &thread_2;
    thread_2.tx_thread_created_next =  &thread_2;
    thread_2.tx_thread_created_previous =  &thread_2;
    _tx_thread_created_count =  1;
    
    /* Attempt to delete thread 2.  */
    status =  tx_thread_delete(&thread_2);
    
    /* Check for the proper status.  */
    if (status != TX_SUCCESS)
    {
    
        /* Thread delete error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }
    
    /* if still okay, restore the saved thread pointer.  */
    if (saved_ptr -> tx_thread_id == TX_THREAD_ID)
    {   
        /* Restore.  */ 
        _tx_thread_created_ptr =  saved_ptr;

        /* Setup the link pointers again.  */
        saved_ptr -> tx_thread_created_previous =  &thread_1;
        thread_1.tx_thread_created_next =  saved_ptr;

        /* Setup the created count. */
        _tx_thread_created_count =  saved_count - 1;
    }

    /* Determine if the first Thread has run and if it's current state is
       finished.  */
    if ((thread_0.tx_thread_state == TX_COMPLETED) && (thread_0_counter == 1) &&
#ifndef TX_DISABLE_NOTIFY_CALLBACKS
        (thread_0_enter == 1) && (thread_0_exit == 1))
#else
        (thread_0_enter == 0) && (thread_0_exit == 0))
#endif
    {

        /* Successful thread finish test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
    else
    {
    
        /* Thread Finish error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    /* Fall through to the return in order to place the thread in a finished
       state.  */
}
