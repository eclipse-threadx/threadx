/* This test is for the thread stack checking services.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Prototype for direct call.  */

VOID        _tx_thread_stack_analyze(TX_THREAD *thread_ptr);



static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;
static CHAR           *thread_2_stack_start;
static UINT            stack_error =  0;


/* Define task prototypes.  */

static void    thread_0_entry(ULONG task_input);
static void    thread_1_entry(ULONG task_input);
static void    thread_2_entry(ULONG task_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the stack checking error handler.  */

void    stack_error_handler(TX_THREAD *thread_ptr)
{

    /* Check for the right thread.  */
    if (thread_ptr == &thread_2)
        stack_error = 1;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_stack_checking_application_define(void *first_unused_memory)
#endif
{

INT     status;
CHAR    *pointer;

    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Stack Checking Test.................................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Stack Checking Test.................................. ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, TX_NO_TIME_SLICE, TX_DONT_START);
    thread_2_stack_start =  pointer;
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Stack Checking Test.................................. ERROR #3\n");
        test_control_return(1);
    }

    /* Register the stack checking handler.  */
    status =  tx_thread_stack_error_notify(stack_error_handler);

#ifdef TX_ENABLE_STACK_CHECKING
    /* Check for status.  */
    if (status != TX_SUCCESS)
#else
    if (status != TX_FEATURE_NOT_ENABLED)
#endif

    {

        printf("Running Thread Stack Checking Test.................................. ERROR #4\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT        status;


    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Inform user of success getting to this test.  */
    printf("Running Thread Stack Checking Test.................................. ");

    /* Resume thread 1 to get the stack checking to take place.  */
    status =  tx_thread_resume(&thread_1);
    
    /* Suspend to allow thread 1 to run.  */
    tx_thread_suspend(&thread_0);

    /* Terminate thread 1.  */
    status = tx_thread_terminate(&thread_1);

    /* Check error code.  */
#ifdef TX_ENABLE_STACK_CHECKING
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 0) || (stack_error != 1))
#else
    if ((status != TX_SUCCESS) || (thread_1_counter != 1) || (thread_2_counter != 0) || (stack_error != 0))
#endif
    {

        /* Thread error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }
    else
    {
    
        /* Success!  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    thread_1_entry(ULONG thread_input)
{

TX_THREAD   fake_thread;

    /* Call stack analyze with a NULL pointer.  */
    _tx_thread_stack_analyze(TX_NULL);

    /* Call the stack analyze with a fake stack just to test that path.  */
    fake_thread.tx_thread_id =  0;
    _tx_thread_stack_analyze(&fake_thread);

    /* Call the stack analyze with a NULL stack pointer.   */
    fake_thread.tx_thread_id =  ((ULONG) 0x54485244);
    fake_thread.tx_thread_stack_start =  TX_NULL;
    _tx_thread_stack_analyze(&fake_thread);

    /* Call the stack analyze with a NULL highest stack pointer.   */
    fake_thread.tx_thread_id =                 ((ULONG) 0x54485244);
    fake_thread.tx_thread_stack_start =        (void *) 0x1000;
    fake_thread.tx_thread_stack_highest_ptr =  TX_NULL;
    _tx_thread_stack_analyze(&fake_thread);

    /* Clear the pattern in thread 2's stack.  */
    TX_MEMSET(thread_2_stack_start, (CHAR) 0x11, TEST_STACK_SIZE_PRINTF);

    /* Resume thread 2, which should cause the stack error to occur.  */
    tx_thread_resume(&thread_2);

    /* Suspend thread 2.  */
    tx_thread_suspend(&thread_2);

    /* Increment thread 1 counter.  */
    thread_1_counter++;

    /* Now, deregister the stack error handler and get into a spin condition. We will then 
       want to terminate thread 1 from thread 0 when it awakes!  */
    tx_thread_stack_error_notify(TX_NULL);
    
    /* Now resume thread 2 again to cause the stack error!  */
    tx_thread_resume(&thread_2);

    /* Now suspend thread 2 again.  */
    tx_thread_suspend(&thread_2);

    /* Resume thread 0.  */
    tx_thread_resume(&thread_0);
}


static void    thread_2_entry(ULONG thread_input)
{
       
        /* Increment thread 1 counter.  */
        thread_2_counter++;
}
