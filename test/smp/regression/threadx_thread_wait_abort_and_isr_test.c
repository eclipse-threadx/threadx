/* This test is designed to test for simultaneous thread suspension lifting AND thread wait abort calls.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_timer.h"


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;


static unsigned long   semaphore_put_counter =  0;
static unsigned long   condition_count  =  0;

static TX_SEMAPHORE    semaphore_0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    timer_0_entry(ULONG timer_input);

/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Define the external reference for the preempt disable flag.  */

extern volatile UINT   _tx_thread_preempt_disable;

/* Prototype for test control return.  */

void  test_control_return(UINT status);


static void    isr_entry(void)
{

UINT    status;
static volatile UINT miss_count = 0;


    /* Attempt to sleep from a timer in order to test the error logic.  */
    status =  tx_thread_sleep(1);

    /* Check for proper error status.  */
    if (status != TX_CALLER_ERROR)
    {
    
        /* Blow up the test to force an error.  */
        condition_count =  10000000;
        semaphore_put_counter =  0xFFFF0000;
    }

    /* Determine if the interrupt occurred when the preempt disable flag was set.  */
    if (_tx_thread_preempt_disable)
    {

        /* Yes this is the condition we are looking for, increment the test condition counter.   */
        condition_count++;
    }

    /* 
    It is possible for this test to get into a resonance condition in which
    the ISR never occurs while preemption is disabled (especially if the
    ISR is installed in the periodic timer interrupt handler, which is
    conveniently available). Detect this condition and break out of it by
    perturbing the duration of this ISR a pseudo-random amount of time.
    */
    else if (++miss_count > 100)
        for (miss_count = _tx_timer_system_clock % 100; miss_count != 0; --miss_count);

    /* Put the semaphore to wakeup thread 0.  */
    status =  tx_semaphore_put(&semaphore_0);

    /* Increment the semaphore counter.  */
    if (status == TX_SUCCESS)
        semaphore_put_counter++;
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_wait_abort_and_isr_application_define(void *first_unused_memory)
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

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #2\n");
        test_control_return(1);
    }

    /* Create semaphore - consumer producer semaphore.  */
    status =  tx_semaphore_create(&semaphore_0, "semaphore 0", 0);
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create a timer to ensure a context save is called for every interrupt.  */
    status =  tx_timer_create(&timer_0, "timer 0", timer_0_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Wait Abort and ISR Resume Test....................... ERROR #4\n");
        test_control_return(1);
    }

    /* Clear the condition count variable.  */
    condition_count  =  0;
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Setup ISR for this test.  */
    test_isr_dispatch =  isr_entry;

    /* Inform user.  */
    printf("Running Thread Wait Abort and ISR Resume Test....................... ");

    /* Loop to exploit the probability window inside tx_thread_wait_abort.  */
    while (condition_count < 20)
    {

        /* Suspend on the semaphore that is going to be set via the ISR.  */
        status =  tx_semaphore_get(&semaphore_0, (thread_0_counter % 5) + 1);

        /* Determine if we have an unexpected result.  */
        if ((status != TX_SUCCESS) && (status != TX_WAIT_ABORTED))
        {

            /* Test error!  */
            printf("ERROR #5\n");
            test_control_return(1);
        }

        /* Check for the preempt disable flag being set.  */
        if (_tx_thread_preempt_disable) 
        {

            /* Test error!  */
            printf("ERROR #6\n");
            test_control_return(2);
        }

        /* Determine if we really got the semaphore.  */
        if (status == TX_SUCCESS)
        {

            /* Increment the thread count.  */
            thread_0_counter++;

#ifdef TX_NOT_INTERRUPTABLE

            /* Determine if we have a non-interruptable build of ThreadX. If so, just 
               get out of this loop after 100 passes.  */

            if (thread_0_counter >= 100)
                break;
#endif
              
        }
    }

    /* Clear ISR dispatch.  */
    test_isr_dispatch =  TX_NULL;

#ifdef TX_NOT_INTERRUPTABLE
    /* At this point, check to see if we got all the semaphores!  */
    if ((thread_0_counter != (semaphore_put_counter - semaphore_0.tx_semaphore_count)) ||
        (condition_count != 0))    
#else
    /* At this point, check to see if we got all the semaphores!  */
    if (thread_0_counter != (semaphore_put_counter - semaphore_0.tx_semaphore_count))
#endif
    {

        /* Test error!  */
        printf("ERROR #7\n");
        test_control_return(3);
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

    /* Loop forever!  */
    while(1)
    {


        /* Abort the suspension on the semaphore in thread 0.  */
        tx_thread_wait_abort(&thread_0);

        /* Increment the thread counter.  */
        thread_1_counter++;

        /* Let thread 0 run again!  */
        tx_thread_relinquish();
    }
}


static void    timer_0_entry(ULONG input)
{
    timer_0_counter++;
}

