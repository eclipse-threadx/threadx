/* This test is designed to test for simultaneous thread event flag set AND ISR event flag set and clear.  */

#include   <stdio.h>
#include   "tx_api.h"


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;

static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_1;

static unsigned long   thread_2_counter =  0;
static TX_THREAD       thread_2;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;

static unsigned long   event_flags_set_counter =  0;
static unsigned long   condition_count =  0;

static TX_EVENT_FLAGS_GROUP    event_flags_0;

extern UINT _tx_thread_preempt_disable;
extern UINT _tx_timer_system_clock;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    timer_0_entry(ULONG timer_input);


static void event_set_notify(TX_EVENT_FLAGS_GROUP *group)
{

    /* Not necessary to do anything in this function.  */
}


static void    test_isr(void)
{

UINT    status;
ULONG   actual;
static volatile UINT miss_count = 0;


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
    {
        for (miss_count = _tx_timer_system_clock % 100; miss_count != 0; --miss_count);
    }

    if (((event_flags_0.tx_event_flags_group_current & 0x3) == 0) &&
        (event_flags_0.tx_event_flags_group_suspended_count == 2))
    {

        /* Put the event_flags to wakeup thread 0.  */
        status =  tx_event_flags_set(&event_flags_0, 0x3, TX_OR);

        /* Clear the same flags immediately.  */
        status += tx_event_flags_set(&event_flags_0, 0xFFFFFFFC, TX_AND);

        /* Setup some event flags just so we can clear them.  */
        status +=  tx_event_flags_set(&event_flags_0, 0x30000, TX_OR);
        
        /* Clear the same flags immediately.  */
        status += tx_event_flags_set(&event_flags_0, 0xFFFEFFFF, TX_AND);
        
        /* Clear the same flags immediately.  */
        status += tx_event_flags_set(&event_flags_0, 0xFFFDFFFC, TX_AND);

        /* Check for an error.  */
        if (status)
            return;

        /* Get the events from an ISR.  */
        status =  tx_event_flags_get(&event_flags_0, 0x30000, TX_OR, &actual, TX_NO_WAIT);
        
        /* Check to make sure this results in an error.  */
        if (status != TX_NO_EVENTS)
            return;
        
        /* Do a set and a get consume from an ISR.  */
        status =  tx_event_flags_set(&event_flags_0, 0x000000C0, TX_OR);

        status += tx_event_flags_get(&event_flags_0, 0x00000080, TX_OR, &actual, TX_NO_WAIT);

        status += tx_event_flags_get(&event_flags_0, 0x000000C0, TX_OR_CLEAR, &actual, TX_NO_WAIT);

        /* Increment the event_flags counter.  */
        if ((status == TX_SUCCESS) && (actual == 0xC4))
        {
            event_flags_set_counter++;
        }
    }
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_event_flag_isr_set_clear_application_define(void *first_unused_memory)
#endif
{

UINT    status;
CHAR    *pointer;


    /* Put first available memory address into a character pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    status =  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            17, 17, 100, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #3\n");
        test_control_return(1);
    }

    /* Create event flags group.  */
    status =  tx_event_flags_create(&event_flags_0, "event_flags 0");
    
    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #4\n");
        test_control_return(1);
    }

    /* Create a timer to ensure a context save is called for every interrupt.  */
    status =  tx_timer_create(&timer_0, "timer 0", timer_0_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #5\n");
        test_control_return(1);
    }
    
    /* Register the event set notify function.  */
    status =  tx_event_flags_set_notify(&event_flags_0, event_set_notify);

#ifndef TX_DISABLE_NOTIFY_CALLBACKS

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #6\n");
        test_control_return(1);
    }
#else

    /* Check status.  */
    if (status != TX_FEATURE_NOT_ENABLED)
    {

        printf("Running Event Flag Set/Clear from ISR Test.......................... ERROR #7\n");
        test_control_return(1);
    }

#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;
ULONG   actual;


    /* Inform user.  */
    printf("Running Event Flag Set/Clear from ISR Test.......................... ");

    /* Setup the test ISR.  */
    test_isr_dispatch =  test_isr;  

    /* Loop to exploit the probability window inside tx_event_flags_set call.  */
    while (condition_count < 40)
    {

        /* Suspend on the event_flags that is going to be set via the ISR.  */
        status =  tx_event_flags_get(&event_flags_0, 2, TX_OR_CLEAR, &actual, 4);

        /* Determine if we have an unexpected result.  */
        if (status != TX_SUCCESS) 
        {

            /* Test error!  */
            printf("ERROR #8\n");
            test_control_return(1);
        }

        /* Check for the preempt disable flag being set.  */
        if (_tx_thread_preempt_disable)
        {

            /* Test error!  */
            printf("ERROR #9\n");
            test_control_return(2);
        }

        /* Determine if we really got the event_flags.  */
        if (status == TX_SUCCESS)
        {

            /* Increment the thread count.  */
            thread_0_counter++;
        }
    }

    /* Setup the test ISR.  */
    test_isr_dispatch =  TX_NULL;  

    /* Let the other threads run once more...  */
    tx_thread_relinquish();

    /* At this point, check to see if we got all the event_flagss!  */
    if ((thread_0_counter != event_flags_set_counter) ||
        (thread_1_counter != event_flags_set_counter))
    {

        /* Test error!  */
        printf("ERROR #10\n");
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

UINT    status;
ULONG   actual;


    /* Loop to exploit the probability window inside tx_event_flags_set call.  */
    while (1)
    {

        /* Suspend on the event_flags that is going to be set via the ISR.  */
        status =  tx_event_flags_get(&event_flags_0, 1, TX_OR_CLEAR, &actual, 4);

        /* Determine if we have an unexpected result.  */
        if (status != TX_SUCCESS) 
        {

            break;
        }

        /* Increment this thread's counter.  */
        thread_1_counter++;
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    /* Loop forever!  */
    while(1)
    {


        /* Set event flags - not the one needed by threads 0 and 1.  */
        tx_event_flags_set(&event_flags_0, 0x4, TX_OR);

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Let thread 0 run again!  */
        tx_thread_relinquish();
    }
}


static void    timer_0_entry(ULONG input)
{
    timer_0_counter++;
}


