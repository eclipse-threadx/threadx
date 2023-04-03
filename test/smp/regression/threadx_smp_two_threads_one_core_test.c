/* Define the ThreadX SMP two threads excluded to one core test.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"



static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;


static ULONG       thread_0_counter;
static ULONG       thread_1_counter;
static ULONG       thread_2_counter;


static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);





/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_two_threads_one_core_test(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;
ULONG   exclusion_map;


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

        printf("Running SMP Two Threads One Core Test............................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_1, 0xD);      /* Only core 1! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            2, 2, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_2, 0xD);      /* Only core 1! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #3\n");
        test_control_return(1);
    }

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #4\n");
        test_control_return(1);
    }

    /* Attempt to call the core-exclusion API with a NULL pointer.  */
    status =  tx_thread_smp_core_exclude(TX_NULL, 1);
    
	/* Check status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #5\n");
        test_control_return(1);
    }
    
    /* Attempt to call the core-exclusion API with a bad thread pointer.  */
    thread_3.tx_thread_id =  0;
    status =  tx_thread_smp_core_exclude(&thread_3, 1);
    
	/* Check status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #6\n");
        test_control_return(1);
    }
    
    /* Test the core exclusion get API.  */
    status =  tx_thread_smp_core_exclude_get(TX_NULL, TX_NULL);
    
	/* Check status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #7\n");
        test_control_return(1);
    }

    /* Test the core exclusion get API with a bad thread pointer.  */
    status =  tx_thread_smp_core_exclude_get(&thread_3, TX_NULL);
    
	/* Check status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #8\n");
        test_control_return(1);
    }

    /* Test the core exclusion get API with a good thread pointer, but a bad return pointer.  */
    status =  tx_thread_smp_core_exclude_get(&thread_2, TX_NULL);
    
	/* Check status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #9\n");
        test_control_return(1);
    }

    /* Now test the proper usage for the core exclusiong get API.  */
    status =  tx_thread_smp_core_exclude_get(&thread_2, &exclusion_map);
    
	/* Check status.  */
    if ((status != TX_SUCCESS) || (exclusion_map != 0xD))
    {

        printf("Running SMP Two Threads One Core Test............................... ERROR #10\n");
        test_control_return(1);
    }   
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP Two Threads One Core Test............................... ");

    /* Resume all the threads.  */
    status =  tx_thread_resume(&thread_1);
    status +=  tx_thread_resume(&thread_2);

    /* Suspend this thread to let the others run.  */
    tx_thread_sleep(10);

    /* Determine if the test was successful or there was an error.  */
    if ((status != TX_SUCCESS) || (error) || (thread_1_counter != 1) || 
        (thread_2_counter != 1))
    {

        /* Execution error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }
    else
    {
    
        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    thread_1_entry(ULONG thread_input)
{

    while(1)
    {
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 1)
            error++;
    
        thread_1_counter++;
        tx_thread_suspend(&thread_1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{
TX_INTERRUPT_SAVE_AREA


    while(1)
    {
    
        /* Ensure this thread is on the correct core.  */
        if (tx_thread_smp_core_get() != 1)
            error++;

        thread_2_counter++;
        
        
        /* Disable interrupts.  */
        TX_DISABLE
        
        /* Increment the preempt disable flag.  */
        _tx_thread_preempt_disable++;
        
        /* Now call the exclude routine to exclude core 1 - move the thread.  */
        if (tx_thread_smp_core_exclude(&thread_2, 0x2))
            error++;
        
        /* Decrement the preempt disable flag.  */
        _tx_thread_preempt_disable--;
        
        /* Restore interrupts.  */
        TX_RESTORE
        
        /* Suspend thread.  */       
        tx_thread_suspend(&thread_2);
    }
}
