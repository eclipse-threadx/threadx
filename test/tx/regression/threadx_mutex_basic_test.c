/* This test is designed to test the mutex create/delete and immediate
   return gets and puts.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_mutex.h"


typedef struct MUTEX_MEMORY_TEST_STRUCT
{
    ULONG           first;
    ULONG           second;
    TX_MUTEX        mutex;
    ULONG           next_to_last;
    ULONG           last;

} MUTEX_MEMORY_TEST;

static  MUTEX_MEMORY_TEST   mutex_memory;


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


static unsigned long   thread_0_counter =  0;
static unsigned long   thread_1_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;
static TX_THREAD       thread_4;

static TX_MUTEX        mutex_0;
static TX_MUTEX        mutex_1;
static TX_MUTEX        mutex_2;
static TX_MUTEX        mutex_3;
static TX_MUTEX        mutex_4;
static TX_MUTEX        mutex_5;
static TX_MUTEX        mutex_6;
static TX_MUTEX        mutex_7;
static TX_MUTEX        mutex_8;

static TX_TIMER        timer_0;


static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_2_entry(ULONG thread_input);
static void    thread_3_entry(ULONG thread_input);
static void    thread_4_entry(ULONG thread_input);

UINT        _txe_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT mutex_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;

    /* Attempt to create a mutex from a timer.  */
    status =  tx_mutex_create(&mutex_4, "mutex 4", TX_NO_INHERIT);
    
    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a mutex from a timer.  */
    status =  tx_mutex_delete(&mutex_2);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }
    
    /* Attempt to get from mutex from a timer with suspension.   */
    status =  tx_mutex_get(&mutex_2, 100);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    timer_executed =  1;
#endif
}

/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;


    /* Attempt to create a mutex from an ISR.  */
    status =  tx_mutex_create(&mutex_4, "mutex 4", TX_NO_INHERIT);
    
    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a mutex from an ISR.  */
    status =  tx_mutex_delete(&mutex_2);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to get from mutex from an ISR with suspension.   */
    status =  tx_mutex_get(&mutex_2, 100);

    /* Check status.  */
    if (status != TX_WAIT_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to get from mutex from an ISR without suspension.   */
    status =  tx_mutex_get(&mutex_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to put a mutex from an ISR.  */
    status =  tx_mutex_put(&mutex_2);

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
void    threadx_mutex_basic_application_define(void *first_unused_memory)
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
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;


    status +=  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    status +=  tx_thread_create(&thread_2, "thread 2", thread_2_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    status +=  tx_thread_create(&thread_3, "thread 3", thread_3_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    status +=  tx_thread_create(&thread_4, "thread 4", thread_4_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            18, 18, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer = pointer + TEST_STACK_SIZE_PRINTF;

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Basic Test............................................ ERROR #1\n");
        test_control_return(1);
    }

    /* Create a mutex.  */
    status =  tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Basic Test............................................ ERROR #2\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_1, "mutex 1", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Basic Test............................................ ERROR #3\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_2, "mutex 2", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Basic Test............................................ ERROR #4\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_3, "mutex 3", TX_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Basic Test............................................ ERROR #5\n");
        test_control_return(1);
    }

    /* Create another mutex.  */
    status =  tx_mutex_create(&mutex_8, "mutex 8", TX_NO_INHERIT);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Mutex Basic Test............................................ ERROR #5a\n");
        test_control_return(1);
    }
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT    status;


    /* Inform user.  */
    printf("Running Mutex Basic Test............................................ ");

    /* Perform mutex memory test.  */
    mutex_memory.first =        0x11223344;
    mutex_memory.second =       0x55667788;
    mutex_memory.next_to_last = 0x99aabbcc;
    mutex_memory.last =         0xddeeff00;
    
    /* Create the semaphore.  */
    status =  tx_mutex_create(&mutex_memory.mutex, "mutex memory", TX_INHERIT);
    tx_mutex_delete(&mutex_memory.mutex);
    
    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (mutex_memory.first != 0x11223344) ||
        (mutex_memory.second != 0x55667788) ||
        (mutex_memory.next_to_last != 0x99aabbcc) ||
        (mutex_memory.last != 0xddeeff00))
    {

        /* Mutex error.  */
        printf("ERROR #6\n");
        test_control_return(1);
    }
    
    /* Increment thread 0 counter.  */
    thread_0_counter++;

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to create a mutex with a NULL pointer.  */
    status =  tx_mutex_create(TX_NULL, "mutex 2", TX_INHERIT);
    
    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to create a mutex with a bad size.  */
    status =  _txe_mutex_create(&mutex_5, "mutex 5", TX_INHERIT, (sizeof(TX_MUTEX)+1));
    
    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to create a mutex that has already been created.  */
    status =  tx_mutex_create(&mutex_2, "mutex 2", TX_INHERIT);
    
    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to create a mutex with a bad inheritance option.  */
    status =  tx_mutex_create(&mutex_4, "mutex 4", 14);
    
    /* Check status.  */
    if (status != TX_INHERIT_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to delete a mutex with a NULL pointer.  */
    status =  tx_mutex_delete(TX_NULL);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-created mutex.  */
    mutex_4.tx_mutex_id =  0;
    status =  tx_mutex_delete(&mutex_4);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Attempt to get a mutex with a NULL pointer.  */
    status =  tx_mutex_get(TX_NULL, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to get a non-created mutex.  */
    mutex_4.tx_mutex_id =  0;
    status =  tx_mutex_get(&mutex_4, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to put a NULL mutex.  */
    status =  tx_mutex_put(TX_NULL);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to put a non-created mutex.  */
    mutex_4.tx_mutex_id =  0;
    status =  tx_mutex_put(&mutex_4);

    /* Check status.  */
    if (status != TX_MUTEX_ERROR)
    {

        /* Mutex error.  */
        printf("ERROR #16\n");
        test_control_return(1);
    }
#endif

    /* Attempt to get from mutex that is available.  Should be successful!  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to get the same mutex again.  Should be successful!  */
    status =  tx_mutex_get(&mutex_0, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Put the mutex. */
    status =  tx_mutex_put(&mutex_0);

    /* Check status.  */
    if ((status != TX_SUCCESS) || (mutex_0.tx_mutex_ownership_count != 1))
    {

        /* Mutex error.  */
        printf("ERROR #19\n");
        test_control_return(1);
    }

    /* Put the mutex again.  Should be successful! */
    status =  tx_mutex_put(&mutex_0);

    /* Check status.  */
    if ((status != TX_SUCCESS) != (mutex_0.tx_mutex_owner != TX_NULL))
    {

        /* Mutex error.  */
        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Relinquish to allow other thread to get the mutex.  */
    tx_thread_relinquish();

    /* Attempt to get the mutex.  Should be unsuccessful.  */
    status =  tx_mutex_get(&mutex_1, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_NOT_AVAILABLE)
    {

        /* Mutex error.  */
        printf("ERROR #21\n");
        test_control_return(1);
    }

    /* Relinquish again so that the other thread can release it.  */
    tx_thread_relinquish();

    /* Delete mutex.  */
    status =  tx_mutex_delete(&mutex_0);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #22\n");
        test_control_return(1);
    }

    status =  tx_mutex_delete(&mutex_1);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #23\n");
        test_control_return(1);
    }

    /* Attempt to get a priority inheritance mutex.  */
    status =  tx_mutex_get(&mutex_2, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Attempt to get another priority inheritance mutex.  */
    status =  tx_mutex_get(&mutex_3, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #25\n");
        test_control_return(1);
    }

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Wakeup thread 2 to get the ISR to take place on top of the thread.  */
    tx_thread_resume(&thread_2);

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Block memory error.  */
        printf("ERROR #26\n");
        test_control_return(1);
    }

#endif

    /* Release mutex multiple times.  */
    status =  tx_mutex_put(&mutex_2);
    status += tx_mutex_put(&mutex_2);
    
    /* Check status.  */
    if (status != TX_NOT_OWNED)
    {

        /* Mutex error.  */
        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Attempt to release a mutex that is not owned.  */
    status =  _tx_mutex_put(&mutex_2);

    /* Check status.  */
    if (status != TX_NOT_OWNED)
    {

        /* Mutex error.  */
        printf("ERROR #28\n");
        test_control_return(1);
    }
    
    /* Delete mutex.  */
    status =  tx_mutex_delete(&mutex_2);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Get mutex 8.  */
    status =  tx_mutex_get(&mutex_8, TX_WAIT_FOREVER);
    
    /* Start thread 3 and 4.  */
    status += tx_thread_resume(&thread_3);
    status += tx_thread_resume(&thread_4);
    
    /* Sleep to let thread 3 suspend on the mutex.  */
    tx_thread_sleep(2);
    
    /* Now, put the mutex to give it to thread 3.  */
    status += tx_mutex_put(&mutex_8);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #29a\n");
        test_control_return(1);
    }

    status =  tx_mutex_delete(&mutex_3);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #30\n");
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

UINT    status;

    /* Increment thread 1 counter.  */
    thread_1_counter++;

    /* Attempt to get from mutex that is available.  Should be successful!  */
    status =  tx_mutex_get(&mutex_1, TX_NO_WAIT);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Let other thread run again.  */
    tx_thread_relinquish();

    /* Release mutex!  */
    status =  tx_mutex_put(&mutex_1);

    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #32\n");
        test_control_return(1);
    }

    /* Create and obtain a couple mutexes so the thread completion can release them.  */ 
    status =  tx_mutex_create(&mutex_6, "mutex 6", TX_NO_INHERIT);
    status += tx_mutex_create(&mutex_7, "mutex 7", TX_NO_INHERIT);
    status += tx_mutex_get(&mutex_6, TX_NO_WAIT);
    status += tx_mutex_get(&mutex_7, TX_NO_WAIT);
    status += tx_mutex_get(&mutex_6, TX_NO_WAIT);
    status += tx_mutex_get(&mutex_7, TX_NO_WAIT);
    
    /* Check status.  */
    if (status != TX_SUCCESS)
    {

        /* Mutex error.  */
        printf("ERROR #33\n");
        test_control_return(1);
    }
}


static void    thread_2_entry(ULONG thread_input)
{

    while(1)
    {

        tx_thread_relinquish();    
    }
}


static void    thread_3_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_mutex_get(&mutex_8, TX_WAIT_FOREVER);
        tx_mutex_put(&mutex_8);
    }
}


static void    thread_4_entry(ULONG thread_input)
{

    while(1)
    {
    
        tx_mutex_get(&mutex_8, TX_WAIT_FOREVER);
        tx_mutex_put(&mutex_8);
    }
}
