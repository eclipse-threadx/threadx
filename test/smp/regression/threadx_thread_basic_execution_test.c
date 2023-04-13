/* This test is designed to see if one thread can be created and executed.  
   It thread_0_entry is hit, then the thread was successfully scheduled.  
   On success, thread_0_counter gets incremented.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_block_pool.h"
#include   "tx_byte_pool.h"
#include   "tx_event_flags.h"
#include   "tx_mutex.h"
#include   "tx_queue.h"
#include   "tx_semaphore.h"
#include   "tx_thread.h"


typedef struct THREAD_MEMORY_TEST_STRUCT
{
    ULONG       first;
    ULONG       second;
    TX_THREAD   thread_block;
    ULONG       first_middle;
    ULONG       second_middle;
    ULONG       stack[2048/sizeof(ULONG)];
    ULONG       next_to_last;
    ULONG       last;

} THREAD_MEMORY_TEST;

static  THREAD_MEMORY_TEST   thread_memory;



/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


static unsigned long   thread_0_counter =  0;
static TX_THREAD       thread_0;
static TX_THREAD       thread_1;
static TX_THREAD       thread_2;
static TX_THREAD       thread_3;
static TX_THREAD       test_thread;

static TX_TIMER        timer_0;


static UCHAR           not_used_stack[TEST_STACK_SIZE_PRINTF];

static unsigned long error =  0;
static unsigned long timer_executed =  0;
static unsigned long isr_executed =  0;


/* Define task prototypes.  */

static void    thread_0_entry(ULONG task_input);

UINT        _txe_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start, UINT thread_control_block_size);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the timer for this test.  */

static void    timer_entry(ULONG i)
{

#ifndef TX_DISABLE_ERROR_CHECKING

UINT    status;
CHAR    *pointer;

    /* Attempt to create a thread from a timer.  */
    pointer =  (CHAR *) 0x3000;
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt a thread reset from a timer.  */
    status =  tx_thread_reset(&thread_0);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
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

CHAR    *pointer;
UINT    status;
UINT    old_value;
ULONG   old_time_slice;


    /* Call tx_thread_relinquish from ISR to make sure the error checking discards the call.  */
    tx_thread_relinquish();

    /* Attempt to create a thread from a timer.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to delete a thread from an ISR.  */
    status =  tx_thread_delete(&thread_0);
    
    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to change preemption from an ISR.  */
    status =  tx_thread_preemption_change(&thread_0, 1, &old_value);
    
    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt to change priority from an ISR.  */
    status =  tx_thread_priority_change(&thread_0, 1, &old_value);
    
    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt a thread reset from an ISR.  */
    status =  tx_thread_reset(&thread_0);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt a thread terminate from an ISR.  */
    status =  tx_thread_terminate(&thread_0);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    /* Attempt a thread time slice change from an ISR.  */
    status =  tx_thread_time_slice_change(&thread_0, 1, &old_time_slice);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }

    isr_executed =  1;
#endif

    /* Test thread sleep call from ISR.  */
    status =  tx_thread_sleep(11);

    /* Check for status.  */
    if (status != TX_CALLER_ERROR)
    {

        /* Error!  */
        error++;
    }
}

#if 0
static void    test_thread_entry(ULONG thread_input)
{
    /* Do nothing here!  */
}
#endif

/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    threadx_thread_basic_execution_application_define(void *first_unused_memory)
#endif
{

INT         status;
CHAR        *pointer;
TX_THREAD   fake_thread;


    /* Setup a pointer.  */
    pointer =  (CHAR *) first_unused_memory;
    
    /* Adjust it forward just to make sure there is some space for the test below.  */
    pointer =  pointer + 200;

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */
    
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SUCCESS)
    {

        printf("Running Thread Basic Execution Test................................. ERROR #1\n");
        test_control_return(1);
    }

#ifndef TX_NOT_INTERRUPTABLE

    /* Now setup a fake thread to generate the other NULL pointer test in the cleanup routines.  */
    fake_thread.tx_thread_suspend_control_block =  TX_NULL;
    _tx_semaphore_cleanup(&fake_thread, 0);
    _tx_queue_cleanup(&fake_thread, 0);
    _tx_mutex_cleanup(&fake_thread, 0);
    _tx_event_flags_cleanup(&fake_thread, 0);
    _tx_byte_pool_cleanup(&fake_thread, 0);
    _tx_block_pool_cleanup(&fake_thread, 0);
#endif
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

#ifndef TX_DISABLE_ERROR_CHECKING
UINT    status;
CHAR    *pointer;
UINT    old_value;
ULONG   old_time_slice;
#endif

VOID            (*temp_mutex_release)(TX_THREAD *thread_ptr);


    /* Increment thread 0 counter.  */
    thread_0_counter++;

    /* Inform user of success getting to this test.  */
    printf("Running Thread Basic Execution Test................................. ");

    /* Setup test thread to make sure _tx_thread_wait_abort can handle a NULL cleanup.  */
    test_thread.tx_thread_state =                              TX_IO_DRIVER;
    test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    test_thread.tx_thread_suspending =                         TX_TRUE;
    test_thread.tx_thread_delayed_suspend =                    TX_TRUE;
    if(_tx_thread_wait_abort(&test_thread) != TX_WAIT_ABORT_ERROR)
    {
        printf("ERROR #XX\n");
        test_control_return(1);
    }

    /* Setup test thread to make sure _tx_thread_timeout can handle a NULL cleanup.  */
    test_thread.tx_thread_state =                              TX_IO_DRIVER;
    test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    test_thread.tx_thread_suspending =                         TX_TRUE;
    test_thread.tx_thread_delayed_suspend =                    TX_TRUE;
    _tx_thread_timeout((ULONG) &test_thread);

    /* Setup test thread to make sure _tx_thread_terminate can handle a NULL mutex release function pointer.  */
    temp_mutex_release =  _tx_thread_mutex_release;
    _tx_thread_mutex_release =  TX_NULL;
    test_thread.tx_thread_state =                              TX_TERMINATED;
    test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    test_thread.tx_thread_suspending =                         TX_TRUE;
    test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    test_thread.tx_thread_delayed_suspend =                    TX_TRUE;
    status =  _tx_thread_terminate(&test_thread);    
    _tx_thread_mutex_release =  temp_mutex_release;     /* Recover Mutex release pointer.  */

    /* Perform thread memory test.  */
    thread_memory.first =        0x11223344;
    thread_memory.second =       0x55667788;
    thread_memory.first_middle = 0x21314151;
    thread_memory.second_middle= 0x61718191;
    thread_memory.next_to_last = 0x99aabbcc;
    thread_memory.last =         0xddeeff00;
    
    /* Create the thread.  */
    status +=  tx_thread_create(&thread_memory.thread_block, "thread memory", thread_0_entry, 1,  
            &thread_memory.stack[0], (2048*sizeof(ULONG))/sizeof(ULONG), 
            16, 16, TX_NO_TIME_SLICE, TX_DONT_START);
    tx_thread_delete(&thread_memory.thread_block);

    /* Check for status.  */
    if ((status != TX_SUCCESS) ||
        (thread_memory.first != 0x11223344) ||
        (thread_memory.second != 0x55667788) ||
        (thread_memory.first_middle != 0x21314151) ||
        (thread_memory.second_middle != 0x61718191) ||
        (thread_memory.next_to_last != 0x99aabbcc) ||
        (thread_memory.last != 0xddeeff00))
    {
    
        /* Memory overwrite error.  */
        printf("ERROR #2\n");
        test_control_return(1);
    }



#ifndef TX_DISABLE_ERROR_CHECKING

    /* Attempt to create a thread with a null pointer.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(TX_NULL, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #3\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with a bad control block size.  */
    pointer =  (CHAR *) not_used_stack;
    status =  _txe_thread_create(&thread_3, "thread 3", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START, (sizeof(TX_THREAD)+1));

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #4\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with a NULL entry function.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_3, "thread 3", TX_NULL, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("ERROR #5\n");
        test_control_return(1);
    }

    /* Attempt to create a thread that has already been created.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #6\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with an overlapping stack.  */
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            thread_0.tx_thread_stack_ptr, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("ERROR #7\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with another variation of an overlapping stack.  */
    pointer =  thread_0.tx_thread_stack_start;
    pointer =  pointer - 20;
    status =  tx_thread_create(&thread_1, "thread 1", TX_NULL, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("ERROR #8\n");
        test_control_return(1);
    }

    /* Attempt to create a thread an extra small stack.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            pointer, 1, 
            16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_SIZE_ERROR)
    {

        printf("ERROR #9\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with an invalid thread priority.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            5000, 5000, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_PRIORITY_ERROR)
    {

        printf("ERROR #10\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with an invalid preemption-threshold.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 17, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for status.  */
    if (status != TX_THRESH_ERROR)
    {

        printf("ERROR #11\n");
        test_control_return(1);
    }

    /* Attempt to create a thread with an invalid auto start.  */
    pointer =  (CHAR *) not_used_stack;
    status =  tx_thread_create(&thread_1, "thread 1", thread_0_entry, 1,  
            pointer, TEST_STACK_SIZE_PRINTF, 
            16, 16, TX_NO_TIME_SLICE, 3456);

    /* Check for status.  */
    if (status != TX_START_ERROR)
    {

        printf("ERROR #12\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-thread.  */
    status =  tx_thread_delete(TX_NULL);

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #13\n");
        test_control_return(1);
    }

    /* Attempt to delete a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status =  tx_thread_delete(&thread_2);

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #14\n");
        test_control_return(1);
    }

    /* Attempt to register a entry/exit callback on a non-thread.  */
    status =  tx_thread_entry_exit_notify(TX_NULL, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #15\n");
        test_control_return(1);
    }

    /* Attempt to register a entry/exit callback on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status =  tx_thread_entry_exit_notify(&thread_2, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #16\n");
        test_control_return(1);
    }

    /* Attempt to get info on a non-thread.  */
    status =  tx_thread_info_get(TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #17\n");
        test_control_return(1);
    }

    /* Attempt to get info on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status =  tx_thread_info_get(&thread_2, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #18\n");
        test_control_return(1);
    }

    /* Attempt to change preemption of a non-thread.  */
    status =  tx_thread_preemption_change(TX_NULL, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #19\n");
        test_control_return(1);
    }
    
    /* Attempt to change preemption of a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status =  tx_thread_preemption_change(&thread_2, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #20\n");
        test_control_return(1);
    }

    /* Attempt to change preemption with a NULL return value.  */
    status =  tx_thread_preemption_change(&thread_0, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("ERROR #21\n");
        test_control_return(1);
    }
    
    /* Attempt to change preemption with a bad threshold value.  */
    status =  tx_thread_preemption_change(&thread_0, 17, &old_value);
    
    /* Check for status.  */
    if (status != TX_THRESH_ERROR)
    {

        printf("ERROR #22\n");
        test_control_return(1);
    }


    /* Attempt to change priority of a non-thread.  */
    status =  tx_thread_priority_change(TX_NULL, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #23\n");
        test_control_return(1);
    }
    
    /* Attempt to change priority of a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status =  tx_thread_priority_change(&thread_2, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #24\n");
        test_control_return(1);
    }

    /* Attempt to change priority with a NULL return value.  */
    status =  tx_thread_priority_change(&thread_0, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("ERROR #25\n");
        test_control_return(1);
    }
    
    /* Attempt to change priority with a bad priority value.  */
    status =  tx_thread_priority_change(&thread_0, 2046, &old_value);
    
    /* Check for status.  */
    if (status != TX_PRIORITY_ERROR)
    {

        printf("ERROR #26\n");
        test_control_return(1);
    }

    /* Attempt a thread reset for a non-thread.  */
    status =  tx_thread_reset(TX_NULL);

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #27\n");
        test_control_return(1);
    }

    /* Attempt a thread reset from same thread.  */
    status =  tx_thread_reset(&thread_0);

    /* Check for status.  */
    if (status != TX_NOT_DONE)
    {

        printf("ERROR #28\n");
        test_control_return(1);
    }

    /* Attempt a thread reset for a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status =  tx_thread_reset(&thread_2);

    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #29\n");
        test_control_return(1);
    }

    /* Attempt a thread resume with a NULL pointer.  */
    status = tx_thread_resume(TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #30\n");
        test_control_return(1);
    }
    
    /* Attempt a thread resume on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status = tx_thread_resume(&thread_2);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #31\n");
        test_control_return(1);
    }

    /* Attempt a thread suspend with a NULL pointer.  */
    status = tx_thread_suspend(TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #32\n");
        test_control_return(1);
    }
    
    /* Attempt a thread suspend on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status = tx_thread_suspend(&thread_2);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #33\n");
        test_control_return(1);
    }

    /* Attempt a thread termiante with a NULL pointer.  */
    status = tx_thread_terminate(TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #34\n");
        test_control_return(1);
    }
    
    /* Attempt a thread terminate on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status = tx_thread_terminate(&thread_2);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #35\n");
        test_control_return(1);
    }

    /* Attempt a thread time-slice chagne with a NULL pointer.  */
    status = tx_thread_time_slice_change(TX_NULL, 1, &old_time_slice);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #36\n");
        test_control_return(1);
    }
    
    /* Attempt a thread time-slice change on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status = tx_thread_time_slice_change(&thread_2, 1, &old_time_slice);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #37\n");
        test_control_return(1);
    }

    /* Attempt a thread time-slice change with a null return pointer.  */
    status = tx_thread_time_slice_change(&thread_0, 1, TX_NULL);
    
    /* Check for status.  */
    if (status != TX_PTR_ERROR)
    {

        printf("ERROR #38\n");
        test_control_return(1);
    }

    /* Attempt a thread wait abort with a NULL pointer.  */
    status = tx_thread_wait_abort(TX_NULL);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #39\n");
        test_control_return(1);
    }
    
    /* Attempt a thread wait abort on a non-created thread.  */
    thread_2.tx_thread_id =  0;
    status = tx_thread_wait_abort(&thread_2);
    
    /* Check for status.  */
    if (status != TX_THREAD_ERROR)
    {

        printf("ERROR #40\n");
        test_control_return(1);
    }

    /* Create a timer for the test.  */
    tx_timer_create(&timer_0, "timer 0", timer_entry, 0, 1, 1, TX_AUTO_ACTIVATE);

    /* Setup the ISR.  */
    test_isr_dispatch =  test_isr;

    /* Sleep for a bit...  */
    tx_thread_sleep(3);

    /* Clear the ISR.  */
    test_isr_dispatch =  TX_NULL;

    /* Test for error.  */
    if ((error) || (timer_executed != 1) || (isr_executed != 1))
    {
    
        /* Thread error.  */
        printf("ERROR #41\n");
        test_control_return(1);
    }

#endif

    /* Check status.  */
    if (error)
    {

        /* Block memory error.  */
        printf("ERROR #42\n");
        test_control_return(1);
    }
    else
    {
    
        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }

}
