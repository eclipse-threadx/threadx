/* This test is designed to test a simple sleep for 100 ticks.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_thread.h"
#include   "tx_timer.h"

//#define DEBUG

/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);



static TX_THREAD       thread_0;

static int             error = 0;

static int             isr_count =  0;

#ifdef TEST_INTERRUPT_CONDITION
#ifndef TX_NOT_INTERRUPTABLE

#if defined(TX_WIN32_MEMORY_SIZE) || defined(TX_LINUX_MEMORY_SIZE)
/* Use larger array size when running on Win32 test platform because of greater speed.  */
#define ARRAY_SIZE              100
#else
#define ARRAY_SIZE              10
#endif


static UINT                     isr_test_suspend_interrupt = TX_TRUE;
static UINT                     isr_test_suspend_interrupted_condition =  TX_FALSE;
static ULONG                    min_loop_count;
static ULONG                    max_loop_count;
static TX_SEMAPHORE             test_semaphore;
static ULONG                    loop_count;
static volatile ULONG           count;
static volatile ULONG           destination =  0;
static ULONG                    array_delay[ARRAY_SIZE];
static ULONG                    start_time;
static ULONG                    lower_bound;
static ULONG                    upper_bound;
static ULONG                    current_itterations;
#ifdef DEBUG_1
static ULONG                    last_loop_count;
#endif
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

#endif
#endif


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);


/* Prototype for test control return.  */

void  test_control_return(UINT status);



/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

UINT    status;

#ifdef TEST_INTERRUPT_CONDITION
#ifndef TX_NOT_INTERRUPTABLE
ULONG i;

    /* Determine if we are in calibration mode.  */
    if ((loop_count) && (loop_count != 0xFFFFFFFF))
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

    /* Determine if the ISR is in the mode to wakeup the thread suspending with a timeout.   */
    if (isr_test_suspend_interrupt)
    {

        /* Determine if the thread is suspended on the semaphore...  */
        if (thread_0.tx_thread_state == TX_SEMAPHORE_SUSP)
        {

            /* Determine if the test condition is present...  */
            if ((_tx_thread_preempt_disable) &&
                (thread_0.tx_thread_timer.tx_timer_internal_list_head == TX_NULL))
            {
        
                /* Set the flag showing the condition is present.  */
                isr_test_suspend_interrupted_condition =  TX_TRUE;
                
                /* All done with the test.  */
                isr_test_suspend_interrupt =  TX_FALSE;
            }
        
            /* Post to the semaphore to wakeup the thread.  */
            tx_semaphore_put(&test_semaphore);
        }        
        
        return;
    }
#endif
#endif
    
    /* Increment the ISR count.  */
    isr_count++;
    
    /* Call sleep from ISR to check for error!  */
    status =  tx_thread_sleep(100);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {


        error =  1;
    }
    
    /* End the ISR.  */
    test_isr_dispatch =  TX_NULL;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_sleep_for_100ticks_application_define(void *first_unused_memory)
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
            16, 16, 3, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Sleep for 100 Ticks Test............................. ERROR #1\n");
        test_control_return(1);
    }

#ifdef TEST_INTERRUPT_CONDITION
#ifndef TX_NOT_INTERRUPTABLE

    status =  tx_semaphore_create(&test_semaphore, "test semaphore", 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Sleep for 100 Ticks Test............................. ERROR #2\n");
        test_control_return(1);
    }

    min_loop_count =       0xFFFFFFFF;
    max_loop_count =       0;
    loop_count =           0xFFFFFFFF;
    current_itterations =  0;
#ifdef DEBUG_1
    last_loop_count =      0x0;
#endif 
#endif
#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
#ifdef TEST_INTERRUPT_CONDITION
#ifndef TX_NOT_INTERRUPTABLE
ULONG                i;
volatile ULONG       value = 0;
#endif
#endif

    /* Inform user.  */
    printf("Running Thread Sleep for 100 Ticks Test............................. ");

    /* Call sleep with an expiration of 0 and test error code.  */
    status =  tx_thread_sleep(0);
    
    /* Check error code.  */
    if (status != TX_SUCCESS)
    {
    
        /* Thread Simple Sleep error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

#ifdef TEST_INTERRUPT_CONDITION
#ifndef TX_NOT_INTERRUPTABLE


    isr_test_suspend_interrupt = TX_TRUE;
    isr_test_suspend_interrupted_condition =  TX_FALSE;
#endif
#endif

    /* Setup the test ISR.  */
    test_isr_dispatch =  test_isr;

#ifdef TEST_INTERRUPT_CONDITION
#ifndef TX_NOT_INTERRUPTABLE

    /* Callibrate the loop count from thread sleep.  */
    for (i = 0; i < 180; i++)
    {
    
        /* Sleep to get a fresh time.  */
        tx_thread_sleep(1);
        
        /* Set the loop count to 0 and start counting....  */
        loop_count =  0;
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

#if 1
    /* Setup the lower and upper bounds.  */
    lower_bound =  min_loop_count;
    if (lower_bound > 5)
        lower_bound = lower_bound - 5;
    upper_bound =  max_loop_count + 5;
#else
    /* Setup the lower and upper bounds.  */
    lower_bound =  min_loop_count;
    upper_bound =  max_loop_count;
#endif

    current_itterations =  lower_bound;
#ifdef DEBUG
    i =  0;
#endif
    while (isr_test_suspend_interrupted_condition != TX_TRUE)
    {

        /* Sleep to get a frest timer slot.  */
        tx_thread_sleep(1);

        /* Loop to delay to next interrupt.  */
        loop_count =  0;
        start_time =  _tx_timer_system_clock;
        do
        {
            /* Call delay function.  */
            delay_function();
            loop_count++;
        } while (loop_count < current_itterations);      

        /* Check for a timer interrupt... if so, just skip the semaphore get.  */
        if (start_time != _tx_timer_system_clock)
            continue;

        /* Suspend on the semaphore for 20 ticks...  */
        tx_semaphore_get(&test_semaphore, 20);
        
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

        /* Set the tick count simply to use value.  */
        tx_time_set(value);
#ifdef DEBUG
        /* Debug block.  */
        i++;
        if ((i % 180) == 0)
        {
            printf("*** update ***\n");
            if (loop_count == 0xFFFFFFFF)
            printf("loop count:      NA\n");
            else
            printf("loop count:      %lu\n", loop_count);
            printf("current:         %lu\n", current_itterations);
            printf("last loop count: %lu\n", last_loop_count);
            printf("minimum:         %lu\n", min_loop_count);
            printf("maximum:         %lu\n", max_loop_count);
            printf("lower bound:     %lu\n", lower_bound);
            printf("upper bound:     %lu\n", upper_bound);
            printf("count:           %lu\n", i);
        }
#endif
    }

#ifdef DEBUG
    /* Debug block  */
    printf("*** final ***\n");
    if (loop_count == 0xFFFFFFFF)
    printf("loop count:      NA\n");
    else
    printf("loop count:      %lu\n", loop_count);
    printf("current:         %lu\n", current_itterations);
    printf("last loop count: %lu\n", last_loop_count);
    printf("minimum:         %lu\n", min_loop_count);
    printf("maximum:         %lu\n", max_loop_count);
    printf("lower bound:     %lu\n", lower_bound);
    printf("upper bound:     %lu\n", upper_bound);
    printf("count:           %lu\n", i);
#endif

    /* Clear the tick count. */
    tx_time_set(0);

    /* Sleep for 100 ticks.  */
    tx_thread_sleep(100);

    /* Check for error.  */
    if (tx_time_get() < 100)
    {
        
        /* Thread Simple Sleep error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }    
#endif
#endif
    
    /* Clear the tick count. */
    tx_time_set(0);


    /* Sleep for 100 ticks.  */
    status =  tx_thread_sleep(100);

    /* Determine if the sleep was accurate.  */
    if ((status != TX_SUCCESS) || (tx_time_get() < 100) ||
        (tx_time_get() > 101))
    {

        /* Thread Simple Sleep error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Check to make sure the ISR happened and the proper return value was present.  */
    if ((isr_count == 0) || (error))
    {
    
        /* Thread Simple Sleep error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }
    else      
    {

        /* Successful Simple Sleep test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

