/* Define the ThreadX SMP time-slice test.  */

#include   <stdio.h>
#include   "tx_api.h"

static TX_THREAD       thread_0;
static TX_THREAD       thread_31a;
static TX_THREAD       thread_31b;
static TX_THREAD       thread_31c;
static TX_THREAD       thread_31d;
static TX_THREAD       thread_31e;
static TX_THREAD       thread_31f;
static TX_THREAD       thread_31g;
static TX_THREAD       thread_31h;


static ULONG           thread_31a_counter;
static ULONG           thread_31b_counter;
static ULONG           thread_31c_counter;
static ULONG           thread_31d_counter;
static ULONG           thread_31e_counter;
static ULONG           thread_31f_counter;
static ULONG           thread_31g_counter;
static ULONG           thread_31h_counter;


static unsigned long error =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_31a_entry(ULONG thread_input);
static void    thread_31b_entry(ULONG thread_input);
static void    thread_31c_entry(ULONG thread_input);
static void    thread_31d_entry(ULONG thread_input);
static void    thread_31e_entry(ULONG thread_input);
static void    thread_31f_entry(ULONG thread_input);
static void    thread_31g_entry(ULONG thread_input);
static void    thread_31h_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_smp_time_slice_test(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;
UINT    i;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            0, 0, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_0, 0xE);      /* Core 0 only! */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31a, "thread 31a", thread_31a_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31a, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31b, "thread 31b", thread_31b_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31b, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #3\n");
        test_control_return(1);
    }


    status =  tx_thread_create(&thread_31c, "thread 31c", thread_31c_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31c, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #4\n");
        test_control_return(1);
    }


    status =  tx_thread_create(&thread_31d, "thread 31d", thread_31d_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31d, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #5\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31e, "thread 31e", thread_31e_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31e, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #6\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_31f, "thread 31f", thread_31f_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31f, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #7\n");
        test_control_return(1);
    }


    status =  tx_thread_create(&thread_31g, "thread 31g", thread_31g_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 31, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31g, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #8\n");
        test_control_return(1);
    }

    /* Enable preemption-threshold for this thread to hit branch in tx_thread_time_slice
       where the expired time-slice thread is replaced by a thread with preemption-threshold
       enabled.  */
    status =  tx_thread_create(&thread_31h, "thread 31h", thread_31h_entry, 0,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            31, 30, 1, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE_PRINTF;
    status +=   tx_thread_smp_core_exclude(&thread_31h, 0);      /* Any core.  */

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #9\n");
        test_control_return(1);
    }

    /* Clear all the counters.  */
    thread_31a_counter =   0;
    thread_31b_counter =   0;
    thread_31c_counter =   0;
    thread_31d_counter =   0;
    thread_31e_counter =   0;
    thread_31f_counter =   0;
    thread_31g_counter =   0;
    thread_31h_counter =   0;

	/* Resume thread 0.  */
    status =  tx_thread_resume(&thread_0);

	/* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running SMP Time-Slice Test......................................... ERROR #10\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;



    /* Inform user.  */
    printf("Running SMP Time-Slice Test......................................... ");

    /* Resume all the same priority threads.  */
    status =  tx_thread_resume(&thread_31a);
    status += tx_thread_resume(&thread_31b);
    status += tx_thread_resume(&thread_31c);
    status += tx_thread_resume(&thread_31d);
    status += tx_thread_resume(&thread_31e);
    status += tx_thread_resume(&thread_31f);
    status += tx_thread_resume(&thread_31g);
    status += tx_thread_resume(&thread_31h);

    /* Now sleep for 20 ticks to let see if all the threads execute.  */
    tx_thread_sleep(20);
    
    /* Now check and make sure all the threads ran.  */
    if ((status != TX_SUCCESS) || (thread_31a_counter == 0) || (thread_31b_counter == 0) || (thread_31c_counter == 0) || (thread_31d_counter == 0) ||
        (thread_31e_counter == 0) || (thread_31f_counter == 0) || (thread_31g_counter == 0) || (thread_31h_counter == 0))
    {

        /* Execution error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }
    else
    {
    
        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }    
}


static void    thread_31a_entry(ULONG thread_input)
{

    while(1)
    {
        tx_thread_identify();
        thread_31a_counter++;
    }
}


static void    thread_31b_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31b_counter++;
    }
}


static void    thread_31c_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31c_counter++;
    }
}


static void    thread_31d_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31d_counter++;
    }
}


static void    thread_31e_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31e_counter++;
    }
}


static void    thread_31f_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31f_counter++;
    }
}


static void    thread_31g_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31g_counter++;
    }
}


static void    thread_31h_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_thread_identify();   
        thread_31h_counter++;
    }
}

