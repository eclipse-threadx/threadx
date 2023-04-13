/* This test is designed to test a simple application timer services, including create, 
   activate, deactivate, change, and delete.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_timer.h"

typedef struct TIMER_MEMORY_TEST_STRUCT
{
    ULONG       first;
    ULONG       second;
    TX_TIMER    timer;
    ULONG       next_to_last;
    ULONG       last;

} TIMER_MEMORY_TEST;

static  TIMER_MEMORY_TEST   timer_memory;


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Define external reference to status from timer create during initialization.  */

extern UINT     test_timer_create_init;


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;

static unsigned long   timer_0_counter =  0;
static TX_TIMER        timer_0;

static unsigned long   timer_1_counter =  0;
static TX_TIMER        timer_1;

static TX_TIMER        timer_2;
static TX_TIMER        timer_3;

static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;

/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    timer_0_expiration(ULONG timer_input);
static void    timer_1_expiration(ULONG timer_input);

UINT        _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, 
                VOID (*expiration_function)(ULONG), ULONG expiration_input,
                ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate, UINT timer_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;


    /* Determine if the timer was able to be created durning initialization.   */
    if (test_timer_create_init != TX_SUCCESS)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to delete a timer from a timer.  */
    status =  tx_timer_delete(&timer_0);
    
    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create a timer from a timer.  */
    status =  tx_timer_create(&timer_2, "timer 2", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {
    
        /* Error!  */
        error++;
    }

#ifndef TX_TIMER_PROCESS_IN_ISR
    /* Just to test another path, set this timer expired variable.  */
    if (timer_executed == 0)
        _tx_timer_expired =  1;
#endif

    timer_executed =  1;
#endif
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;

    /* Attempt to delete a timer from an ISR.  */
    status =  tx_timer_delete(&timer_0);
    
    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {
    
        /* Error!  */
        error++;
    }

    /* Attempt to create a timer from an ISR.  */
    status =  tx_timer_create(&timer_2, "timer 2", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {
    
        /* Error!  */
        error++;
    }

    isr_executed =  1;
#endif
}


/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_timer_simple_application_define(void *first_unused_memory)
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

    status +=  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, 3, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Simple Test........................................... ERROR #1\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Simple Test........................................... ERROR #2\n");
        test_control_return(1);
    }

    status =  tx_timer_create(&timer_1, "timer 1", timer_1_expiration, 0x1234,
                        1000, 1000, TX_AUTO_ACTIVATE);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Timer Simple Test........................................... ERROR #3\n");
        test_control_return(1);
    }

#ifndef TX_TIMER_PROCESS_IN_ISR

    /* Call the timer thread entry function with a bad ID to test another path.  */
    _tx_timer_thread_entry(0);
#endif

}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Timer Simple Test........................................... ");

    /* Perform timer memory test.  */
    timer_memory.first =        0x11223344;
    timer_memory.second =       0x55667788;
    timer_memory.next_to_last = 0x99aabbcc;
    timer_memory.last =         0xddeeff00;
    
    /* Create the timer.  */
    status =  tx_timer_create(&timer_memory.timer, "timer memory", timer_0_expiration, 0x1234,
                        1000000, 100000, TX_NO_ACTIVATE);
    tx_timer_delete(&timer_memory.timer);

    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (timer_memory.first != 0x11223344) ||
        (timer_memory.second != 0x55667788) ||
        (timer_memory.next_to_last != 0x99aabbcc) ||
        (timer_memory.last != 0xddeeff00))
    {
    
        /* Memory overwrite error.  */
        printf("ERROR #4\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to activate a non-timer.  */
    status =  tx_timer_activate(TX_NULL);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to activate a non-created timer.  */
    timer_2.tx_timer_id =  0;
    status =  tx_timer_activate(&timer_2);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to activate a timer with a 0 expiration.  */
    timer_2.tx_timer_id =  ((ULONG) 0x4154494D);
    timer_2.tx_timer_internal.tx_timer_internal_list_head =  TX_NULL;
    timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks =  0;
    status =  tx_timer_activate(&timer_2);
    
    /* Check status.  */
    if (status != TX_ACTIVATE_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }


    /* Attempt to activate a timer with a wait forever expiration.  */
    timer_2.tx_timer_id =  ((ULONG) 0x4154494D);
    timer_2.tx_timer_internal.tx_timer_internal_list_head =  TX_NULL;
    timer_2.tx_timer_internal.tx_timer_internal_remaining_ticks =  TX_WAIT_FOREVER;
    status =  tx_timer_activate(&timer_2);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {
    
        /* Application timer error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    timer_2.tx_timer_id =  0;

    /* Attempt to deactivate a non-timer.  */
    status =  tx_timer_deactivate(TX_NULL);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to deactivate a non-created timer.  */
    timer_2.tx_timer_id =  0;
    status =  tx_timer_deactivate(&timer_2);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to change a non-timer.  */
    status =  tx_timer_change(TX_NULL, 1, 1);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to change a non-created timer.  */
    timer_2.tx_timer_id =  0;
    status =  tx_timer_change(&timer_2, 1, 1);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Attempt to change a timer with a 0 initial ticks.  */
    status =  tx_timer_change(&timer_0, 0, 1);
    
    /* Check status.  */
    if (status != TX_TICK_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-time.  */
    status =  tx_timer_delete(TX_NULL);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-created time.  */
    timer_2.tx_timer_id =  0;
    status =  tx_timer_delete(&timer_2);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to get info from a non-timer.  */
    status =  tx_timer_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Attempt to get info from a non-created timer.  */
    timer_2.tx_timer_id =  0;
    status =  tx_timer_info_get(&timer_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to create a timer with a NULL pointer.  */
    status =  tx_timer_create(TX_NULL, "timer 0", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE);

    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Attempt to create a timer with a bad control block size.  */
    status =  _txe_timer_create(&timer_3, "timer 3", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE, (sizeof(TX_TIMER)+1));

    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Attempt to create a timer that has already been created.  */
    status =  tx_timer_create(&timer_0, "timer 0", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE);

    /* Check status.  */
    if (status != TX_TIMER_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to create a timer with an invalid number of initial ticks.  */
    status =  tx_timer_create(&timer_2, "timer 2", timer_0_expiration, 0x1234,
                        0, 18, TX_AUTO_ACTIVATE);

    /* Check status.  */
    if (status != TX_TICK_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Attempt to create a timer with an invalid activation.  */
    status =  tx_timer_create(&timer_2, "timer 2", timer_0_expiration, 0x1234,
                        1, 18, TX_AUTO_ACTIVATE+3999);

    /* Check status.  */
    if (status != TX_ACTIVATE_ERROR)
    {
    
        /* Application timer error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

#endif

    /* Sleep for a couple ticks.  */
    tx_thread_sleep(19);

    /* Check for an error.  */
    if (timer_0_counter != 2)
    {

        /* Application timer error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }    

    /* Deactivate the timer.  */
    status =  tx_timer_deactivate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Sleep again.  */
    tx_thread_sleep(19);

    /* Check for an error.  */
    if (timer_0_counter != 2)
    {

        /* Application timer error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }    

    /* Modify the timer.  */
    status =  tx_timer_change(&timer_0, 100, 1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Clear the system time.  */
    tx_time_set(0);

    /* Activate the timer.  */
    status =  tx_timer_activate(&timer_0);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Sleep for 120.  */
    tx_thread_sleep(120);

    /* Check the counters to make sure everything is where it should be.  */
    if ((timer_0_counter != 23) || (tx_time_get() != 120))
    {

        /* Application timer error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Delete the timer...  that are currently active and on the same expiration
       list!  */
    status =  tx_timer_deactivate(&timer_0);
    status += tx_timer_deactivate(&timer_1);
    status += tx_timer_change(&timer_0, 100, 100);
    status += tx_timer_change(&timer_1, 100, 100);
    status += tx_timer_activate(&timer_0);
    status += tx_timer_activate(&timer_1);
    status += tx_timer_delete(&timer_0);
    status += tx_timer_delete(&timer_1);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        /* Application timer error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Resume thread 1 to take an interrupt on top of it.  */
    tx_thread_resume(&thread_1);

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Thread error.  */
        printf("ERROR #30\n");
        test_control_return(1);
    }

#endif

    if (error)
    {
    
        /* Thread error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }
    else
    {

        /* Successful Multiple Sleep test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}

static void    thread_1_entry(ULONG thread_input)
{
    
    while(1)
    {
 
        tx_thread_relinquish();   
    }
}

static void    timer_0_expiration(ULONG timer_input)
{


    /* Process timer expiration.  */
    timer_0_counter++;
}


static void    timer_1_expiration(ULONG timer_input)
{
   
    /* Process timer expiration.  */
    timer_1_counter++;
}
