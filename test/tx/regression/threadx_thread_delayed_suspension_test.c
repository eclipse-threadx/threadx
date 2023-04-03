/* This test checks out the delayed suspension clear from tx_thread_resume.  */

#include <stdio.h>
#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"


#define DEMO_STACK_SIZE TEST_STACK_SIZE_PRINTF


/* Define the ThreadX object control blocks...  */

static TX_THREAD               thread_0;
static TX_THREAD               thread_1;
static TX_SEMAPHORE            semaphore_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define the ISR dispatch routine.  */

#ifndef TX_NOT_INTERRUPTABLE

#if defined(TX_WIN32_MEMORY_SIZE) || defined(TX_LINUX_MEMORY_SIZE)
/* Use larger array size when running on Win32 test platform because of greater speed.  */
#define ARRAY_SIZE              100
#else
#define ARRAY_SIZE              10
#endif


/* Define the ISR dispatch.  */

extern VOID             (*test_isr_dispatch)(void);
static void             thread_2_entry(ULONG thread_input);

static UINT             delayed_suspend_set;

static ULONG            thread_2_counter;
static ULONG            thread_2_counter_capture;

static ULONG            min_loop_count;
static ULONG            max_loop_count;
static ULONG            loop_count;
static volatile ULONG   count;
static volatile ULONG   destination =  0;
static ULONG            start_time;
static ULONG            lower_bound;
static ULONG            upper_bound;
static ULONG            current_itterations;
#ifdef DEBUG_1
static ULONG            last_loop_count;
#endif
static TX_THREAD        thread_2;
static TX_SEMAPHORE     semaphore_1;

static ULONG            array_delay[ARRAY_SIZE];

static ULONG   delay_function(void)
{

ULONG   accumulator;
ULONG   i;

    for (i = 0; i < ARRAY_SIZE; i++)
        array_delay[i] = i;

    for (i = 0; i < ARRAY_SIZE-4; i++)
    {
        array_delay[i] =  (array_delay[i+1] * array_delay[i+2]) * (array_delay[i+3] * array_delay[i+4]);
    }

    accumulator =  0;
    for (i = 0; i < ARRAY_SIZE; i++)
        accumulator =  accumulator + array_delay[i];

    return(accumulator);
}


static void    test_isr(void)
{
ULONG i;

    /* Determine if we are in calibration mode.  */
    if (loop_count != 0xFFFFFFFF)
    {
        if (loop_count < min_loop_count)
            min_loop_count =  loop_count;
        if (loop_count > max_loop_count)
            max_loop_count =  loop_count;   

        lower_bound = loop_count - 1;
        upper_bound = loop_count + 1;
        if (lower_bound < min_loop_count)
            lower_bound =  min_loop_count;
        if (upper_bound > max_loop_count)
            lower_bound = max_loop_count;

        if ((current_itterations < lower_bound) || (current_itterations > upper_bound))
            current_itterations =  lower_bound;
        
#ifdef DEBUG_1
        /* Last loop count.  */
    last_loop_count =  loop_count;
#endif
    
        /* Reset the loop count to all ones!  */
        loop_count =  0xFFFFFFFF;
    }
    count++;
    for (i = 0; i < (count%32); i++)
        destination++;            

    /* Check to see if the interrupt occurred in the middle of the suspension.  */
    if ((thread_2.tx_thread_suspending) && (delayed_suspend_set == 0))
    {
    
        /* Yes, we have taken the interrupt in the middle of a thread suspension.  */
        
        /* Indicate we have got the condition.  */
        delayed_suspend_set =  1;

        /* Capture the current thread 2 counter.  */
        thread_2_counter_capture =  thread_2_counter;
        
        /* Now attempt to set the delayed suspension.  */
        tx_thread_suspend(&thread_2);
        
        /* Check for the delayed suspension flag being set.  */
        if (thread_2.tx_thread_delayed_suspend != 1)
        {
        
            /* Error!  Setup the counters to indicate an error.  */
            thread_2_counter =  0xEEEEEEEE;
            thread_2_counter_capture =  0xFFFFFFFF;
        }

        /* Now, abort the suspension for thread 2... the thread should switch to a pure suspended state.  */
        tx_thread_wait_abort(&thread_2);
        
        /* Check for the proper state.  */
        if (thread_2.tx_thread_state != TX_SUSPENDED)
        {
        
            /* Error!  Setup the counters to indicate an error.  */
            thread_2_counter =  0xEEEEEEEE;
            thread_2_counter_capture =  0xFFFFFFFF;
        }
    }

}
#endif



/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_delayed_suspension_application_define(void *first_unused_memory)
#endif
{

CHAR    *pointer;

    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + DEMO_STACK_SIZE;

    /* Create threads 1 and 2.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, DEMO_STACK_SIZE, 
            2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + DEMO_STACK_SIZE;

    /* Create the semaphore.  */
    tx_semaphore_create(&semaphore_0, "semaphore 0", 0);

#ifndef TX_NOT_INTERRUPTABLE

    tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, DEMO_STACK_SIZE, 
            1, 1, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + DEMO_STACK_SIZE;

    tx_semaphore_create(&semaphore_1, "semaphore 1", 0);

    thread_2_counter =  0;
    thread_2_counter_capture =  0;
    min_loop_count =  0xFFFFFFFF;
    max_loop_count =  0;
    loop_count =      0xFFFFFFFF;
#ifdef DEBUG_1
    last_loop_count = 0;
#endif
#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Thread Delayed Suspension Clearing Test..................... ");

    /* Relinquish to the other thread.  */
    tx_thread_relinquish();

    /* At this point thread 1 has suspended on the semaphore.  */
    
    /* Suspend the already suspended thread.  */
    tx_thread_suspend(&thread_1);

    /* Set the semaphore, which should make it go into a suspend state.  */
    tx_semaphore_put(&semaphore_0);

    /* Resume the other thread so it runs again.  */
    tx_thread_resume(&thread_1);

    /* Relinquish so it can run again.  */
    tx_thread_relinquish();

    /* Suspend the already suspended thread.  */
    tx_thread_suspend(&thread_1);

    /* Now, clear the delayed suspension.  */
    status = tx_thread_resume(&thread_1);

    if (status != TX_SUSPEND_LIFTED)
    {

        /* Delayed suspension error.  */
        printf("ERROR #1\n");
        test_control_return(1);
    }

#ifndef TX_NOT_INTERRUPTABLE

    /* Setup the test ISR.  */
    test_isr_dispatch =  test_isr;

    /* Resume the test thread. */
    tx_thread_resume(&thread_2);

    /* Wait until we see the delayed suspension set flag.  */
    while(delayed_suspend_set == 0)
    {

        /* Abort the suspension for thread 2.  */
        tx_thread_wait_abort(&thread_2);

        /* Just relinquish.  */
        tx_thread_relinquish();
    }
    
    /* Relinquish one more time to make sure thread 2 could run if it is ready.  */
    tx_thread_relinquish();
    
    /* At this point, check for an error.  */
    if (thread_2_counter != thread_2_counter_capture)
    {
    
        /* Delayed suspension error... thread kept running!  */
        printf("ERROR #2\n");
        test_control_return(1);
    }
#endif
    
    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}


static void    thread_1_entry(ULONG thread_input)
{

UINT    status;

    /* This thread simply gets the semaphore...  */
    while(1)
    {

        /* Get semaphore.  */
        status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


#ifndef TX_NOT_INTERRUPTABLE


static void    thread_2_entry(ULONG thread_input)
{

ULONG   i;

    /* Callibrate the loop count from thread sleep.  */
    for (i = 0; i < 10; i++)
    {
    
        /* Sleep to get a fresh time.  */
        tx_thread_sleep(1);
        
        start_time =  _tx_timer_system_clock;
        do
        {

            /* Call delay function.  */
            delay_function();
            loop_count++;
        } while (start_time == _tx_timer_system_clock);
        
        /* Wait to reset the loop count.  */
        tx_thread_sleep(1);
    }

    /* Setup the lower and upper bounds.  */
    lower_bound =  min_loop_count;
    if (lower_bound > 5)
        lower_bound = lower_bound - 5;
    upper_bound =  max_loop_count + 5;

    current_itterations =  lower_bound;

    /* This thread simply suspends over and over...  */
    while(1)
    {

        /* Sleep to get a fresh starting time.  */
        tx_thread_sleep(1);
        
        loop_count =  0;
        start_time =  _tx_timer_system_clock;
        do
        {
            /* Call delay function.  */
            delay_function();
            loop_count++;
        } while (loop_count < current_itterations);      

        /* Suspend this thread.  */
        tx_semaphore_get(&semaphore_1, TX_WAIT_FOREVER);

        /* Adjust the current itterations.  */
        current_itterations++;
        if (current_itterations > upper_bound)
        {
            if (lower_bound > min_loop_count)
                lower_bound--;
            if (upper_bound < max_loop_count)
                upper_bound++;
            current_itterations =  lower_bound;
        }

        /* Increment the thread counter.  */
        thread_2_counter++;
    }
}

#endif
