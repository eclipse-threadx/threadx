/* This test is designed to test application timer activation/deactivation services
   from threads and the activation routines.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_timer.h"


static TX_THREAD       thread_0;

static unsigned long       timer_0_counter =  0;
static TX_TIMER            timer_0;
static TX_TIMER_INTERNAL   test_timer;
static TX_TIMER_INTERNAL   *list_head;
static TX_TIMER            test_app_timer;

static TX_TIMER             timer_2;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    timer_0_expiration(ULONG timer_input);


/* Prototype for test control return.  */
void  test_control_return(UINT status);


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_timer_activate_deactivate_application_define(void *first_unused_memory)
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
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Activate/Deactivate Test.............................. ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        13, 23, TX_NO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Activate/Deactivate Test.............................. ERROR #2\n");
        test_control_return(1);
    }
}

VOID  _tx_timer_system_activate(TX_TIMER_INTERNAL *timer_ptr);

/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT                status;
TX_TIMER_INTERNAL   **current_list_head;


    /* Inform user.  */
    printf("Running Timer Activate/Deactivate Test.............................. ");

#ifndef TX_TIMER_PROCESS_IN_ISR

    /* Call the timer thread entry function with an invalid value to make sure the code simply returns.  */
    _tx_timer_thread_entry(0);
#endif   
    
#ifndef TX_TIMER_PROCESS_IN_ISR

    tx_thread_resume(&_tx_timer_thread);
#endif
      
    /* Call the internal timer activate function with 0 remaining time.  */
    test_timer.tx_timer_internal_remaining_ticks =  0;
    _tx_timer_system_activate(&test_timer);

    /* Call the internal timer activate function with an existing head pointer.  */
    test_timer.tx_timer_internal_remaining_ticks =  1;
    list_head =  TX_NULL;
    test_timer.tx_timer_internal_list_head =  &list_head;
    _tx_timer_system_activate(&test_timer);
    
    /* Call the internal timer deactivate function to ensure the list head is not updated unless valid.  */
    list_head =  TX_NULL;
    test_timer.tx_timer_internal_list_head =   &list_head;
    test_timer.tx_timer_internal_active_next =  &test_timer;
    _tx_timer_system_deactivate(&test_timer);

    /* Call timer info get with a timer setup to exercise a path not possible, in order to exercise all
       conditionals.  */
    test_app_timer.tx_timer_internal.tx_timer_internal_list_head =  (_tx_timer_list_end + 1);
    status =  _tx_timer_info_get(&test_app_timer, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL); 
     
    /* Deactivate and activate the timer.  */
    test_app_timer.tx_timer_internal.tx_timer_internal_active_next =  &test_app_timer.tx_timer_internal;
    status +=  _tx_timer_deactivate(&test_app_timer);

    /* Change timer with a non-NULL list head, to cause processing to be skipped.  */
    test_app_timer.tx_timer_internal.tx_timer_internal_list_head =  (_tx_timer_list_end + 1);
    status += _tx_timer_change(&test_app_timer, 0, 0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #3\n");
        test_control_return(1);
    }

    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Attempt to activate the same timer again.  */
    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_ACTIVATE_ERROR)
    {

        /* Application timer error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Sleep for a 14 ticks.  */
    tx_thread_sleep(14);

    /* At this point the initial expiration of the timer should have 
       happened.  */
    if (timer_0_counter != 1)
    {

        /* Application timer activate/deactivate error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Sleep for the reschedule time to make sure the timer doesn't hit
       again!  */
    tx_thread_sleep(24);

    /* At this point the timer counter should still be 1.  */ 
    if (timer_0_counter != 1)
    {

        /* Application timer activate/deactivate error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Clear timer 0's counter.  */
    timer_0_counter =  0;

    /* Activate the timer.  This should use the reschedule time.  */
    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Test timer that is in the process of expiration - on temporary "expired" list.  */
    TX_MEMSET(&timer_2, 0, (sizeof(TX_TIMER)));

    /* Setup fake timer and test for reactivate condition - current timer.  */
    timer_2.tx_timer_id =  TX_TIMER_ID;
    timer_2.tx_timer_internal.tx_timer_internal_list_head =  (TX_TIMER_INTERNAL **) &current_list_head;
    current_list_head =  (struct TX_TIMER_INTERNAL_STRUCT **) &(timer_2.tx_timer_internal);
    timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks =  TX_TIMER_ENTRIES*2; 
    timer_2.tx_timer_internal.tx_timer_internal_active_next =  &(timer_2.tx_timer_internal);
    status =  tx_timer_deactivate(&timer_2);

    /* Check for error.  */
    if ((status != TX_SUCCESS) || (timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks != TX_TIMER_ENTRIES))
    {
    
        /* Application timer error.  */
        printf("ERROR #8a\n");
        test_control_return(1);
    }


    /* Sleep for twice the expiration time to make sure the timer 
       doesn't automatically reschedule.  */
    tx_thread_sleep(47);

    /* Check for an error.  */

    /* At this point the timer counter should still be 1.  */ 
    if (timer_0_counter != 1)
    {

        /* Application timer activate/deactivate error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }
    else
    {

        /* Application Timer test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}


static void    timer_0_expiration(ULONG timer_input)
{


UINT    status;


    /* Attempt to sleep from a timer in order to test the error logic.  */
    status =  tx_thread_sleep(1);

    /* Check for proper error status.  */
    if (status == TX_CALLER_ERROR)
    {

        /* Process timer expiration.  */
        timer_0_counter++;

        /* Deactivate the timer from the expiration routine.  */
        tx_timer_deactivate(&timer_0);
    }
}


