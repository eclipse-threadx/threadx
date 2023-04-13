/* This test is designed to see if a thread can be created with a time-slice.  
   No time-slice occurs, only the processing to check for time-slicing.  */

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
void    threadx_thread_basic_time_slice_application_define(void *first_unused_memory)
#endif
{

UINT     status;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            first_unused_memory, TEST_STACK_SIZE_PRINTF, 
            16, 16, 1, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Basic Time-Slice Test................................ ERROR #1\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{


    /* Inform user.  */
    printf("Running Thread Basic Time-Slice Test................................ ");

    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 0 counter.  */
        thread_0_counter++;

        /* Determine if we are done.  */
        if (tx_time_get() > 18)
        {

            /* Successful Time-slice test.  */
            printf("SUCCESS!\n");
            test_control_return(0);
        }
    }
}
