/* This test is designed to see if a thread can be created with a time-slice.  
   No time-slice occurs, only the processing to check for time-slicing.  */

#include   <stdio.h>
#include   "tx_api.h"

static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


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
UCHAR	 *memory;

  
    memory =  (UCHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            memory, TEST_STACK_SIZE_PRINTF, 
            16, 16, 1, TX_AUTO_START);
	memory =  memory + TEST_STACK_SIZE_PRINTF;
    status +=  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            memory, TEST_STACK_SIZE_PRINTF, 
            16, 16, 1, TX_DONT_START);
    status +=  tx_thread_smp_core_exclude(&thread_1, 0xF);
    status +=  tx_thread_resume(&thread_1);

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

ULONG   target_time;


    /* Inform user.  */
    printf("Running Thread Basic Time-Slice Test................................ ");

    /* Calculate the target running time.  */
    target_time =  tx_time_get() + 20;

    /* Enter into a forever loop.  */
    while(1)
    {

        /* Increment thread 0 counter.  */
        thread_0_counter++;

        /* Determine if we are done.  */
        if (tx_time_get() >= target_time)
        {

            /* Determine if thread 1 executed.  */
            if (thread_1_counter != 0)
            {
            
                /* Error, thread 1 has all cores excluded so it should never run.  */
                printf("ERROR #2\n");
                test_control_return(0);
            }
            else
            {

                /* Successful Time-slice test.  */
                printf("SUCCESS!\n");
                test_control_return(0);
            }
        }
    }
}


static void    thread_1_entry(ULONG thread_input)
{

    /* Loop forever.  */
    while(1)
    {

        /* Increment counter.  */
        thread_1_counter++;    
    }
}
