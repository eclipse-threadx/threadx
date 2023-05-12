/* This is the test control routine of the ThreadX kernel.  All tests are dispatched from this routine.  */

#include "tx_api.h"
#include <stdio.h>
#include "tx_initialize.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_semaphore.h"
#include "tx_queue.h"
#include "tx_mutex.h"
#include "tx_block_pool.h"
#include "tx_byte_pool.h"
#include "tx_event_flags.h"

/* Version check. */
#ifndef EL_PRODUCT_THREADX
#error "EL_PRODUCT_THREADX is not defined."
#endif

#if defined(EXPECTED_MAJOR_VERSION) && ( !defined(THREADX_MAJOR_VERSION) || THREADX_MAJOR_VERSION != EXPECTED_MAJOR_VERSION)
#error "THREADX_MAJOR_VERSION"
#endif /* Check THREADX_MAJOR_VERSION */

#if defined(EXPECTED_MINOR_VERSION) && ( !defined(THREADX_MINOR_VERSION) || THREADX_MINOR_VERSION != EXPECTED_MINOR_VERSION) 
#error "THREADX_MINOR_VERSION"
#endif /* Check THREADX_MINOR_VERSION */


#define TEST_STACK_SIZE         6144


/* Define the test control ThreadX objects...  */

TX_THREAD               test_control_thread;
TX_THREAD               test_thread;
TX_THREAD               test_thread1;
TX_THREAD               test_thread2;
TX_THREAD               test_thread3;
ULONG                   test_thread3_stack[256];
TX_QUEUE                test_queue;
ULONG                   test_thread2_stack[256];
TX_TIMER                test_timer;
TX_MUTEX                init_mutex;
TX_MUTEX                init_mutex_inherit;
TX_MUTEX                cleanup_mutex;
TX_EVENT_FLAGS_GROUP    cleanup_event_flags;
TX_BLOCK_POOL           cleanup_block_pool;
TX_BYTE_POOL            cleanup_byte_pool;
TX_QUEUE                cleanup_queue;
TX_SEMAPHORE            cleanup_semaphore;
TX_SEMAPHORE            init_semaphore;
ULONG                   init_queue_area[20];
TX_QUEUE                init_queue;
TX_BYTE_POOL            init_byte_pool;
ULONG                   init_byte_pool_area[50];
TX_BLOCK_POOL           init_block_pool;
ULONG                   init_block_pool_area[50];
TX_EVENT_FLAGS_GROUP    init_event_flags;
TX_TIMER                init_timer;
TX_THREAD               init_test_thread;
TX_THREAD               second_test_thread;
#ifndef TX_TIMER_PROCESS_IN_ISR
TEST_FLAG               threadx_delete_timer_thread;
#endif
TX_TIMER_INTERNAL 	    **_timer_list_start_backup;
TEST_FLAG               test_stack_analyze_flag;
TEST_FLAG               test_initialize_flag; 
TX_BLOCK_POOL           fake_block_pool;
TX_BYTE_POOL            fake_byte_pool;
TX_EVENT_FLAGS_GROUP    fake_event_flags;
TX_MUTEX                fake_mutex;
TX_QUEUE                fake_queue;
TX_SEMAPHORE            fake_semaphore;

TEST_FLAG               test_forced_mutex_timeout;
TEST_FLAG               threadx_mutex_suspension_put_test;
TEST_FLAG               threadx_mutex_suspension_priority_test;
TEST_FLAG               threadx_byte_allocate_loop_test;
TEST_FLAG               threadx_byte_release_loop_test;


/* Define the test control global variables.   */

ULONG           test_control_return_status;
ULONG           test_control_successful_tests;
ULONG           test_control_failed_tests;
ULONG           test_control_system_errors;

UINT            test_mutex_from_init;
UINT            test_semaphore_from_init;
UINT            test_queue_from_init;
UINT            test_event_flags_from_init;
UINT            test_byte_pool_create_init;
UINT            test_block_pool_create_init;
UINT            test_timer_create_init;


/* Remember the start of free memory.  */

UCHAR           *test_free_memory_ptr;


/* Define the function pointer for ISR dispatch.  */

VOID            (*test_isr_dispatch)(void);


UCHAR           test_control_memory[0x60000];
UCHAR           tests_memory[0x60000];

/* Define the external reference for the preempt disable flag.  */

extern volatile UINT   _tx_thread_preempt_disable;
extern volatile ULONG  _tx_thread_system_state;


/* Define test entry pointer type.  */

typedef  struct TEST_ENTRY_STRUCT
{

VOID        (*test_entry)(void *);
} TEST_ENTRY;


/* Define the prototypes for the test entry points.  */

void    threadx_block_memory_basic_application_define(void *);
void    threadx_block_memory_error_detection_application_define(void *);
void    threadx_block_memory_suspension_application_define(void *);
void    threadx_block_memory_suspension_timeout_application_define(void *);
void    threadx_block_memory_thread_terminate_application_define(void *);
void    threadx_block_memory_prioritize_application_define(void *);
void    threadx_block_memory_information_application_define(void *);

void    threadx_byte_memory_basic_application_define(void *);
void    threadx_byte_memory_thread_contention_application_define(void *);
void    threadx_byte_memory_suspension_application_define(void *);
void    threadx_byte_memory_suspension_timeout_application_define(void *);
void    threadx_byte_memory_thread_terminate_application_define(void *);
void    threadx_byte_memory_prioritize_application_define(void *);
void    threadx_byte_memory_information_application_define(void *);

void    threadx_event_flag_basic_application_define(void *);
void    threadx_event_flag_suspension_application_define(void *);
void    threadx_event_flag_suspension_consume_application_define(void *);
void    threadx_event_flag_suspension_different_bits_application_define(void *);
void    threadx_event_flag_suspension_different_bits_consume_application_define(void *);
void    threadx_event_flag_suspension_timeout_application_define(void *);
void    threadx_event_flag_thread_terminate_application_define(void *);
void    threadx_event_flag_single_thread_terminate_application_define(void *);
void    threadx_event_flag_isr_set_clear_application_define(void *);
void    threadx_event_flag_isr_wait_abort_application_define(void *);
void    threadx_event_flag_information_application_define(void *);

void    threadx_interrupt_control_application_define(void *);

void    threadx_mutex_basic_application_define(void *);
void    threadx_mutex_delete_application_define(void *);
void    threadx_mutex_preemption_application_define(void *);
void    threadx_mutext_no_preemption_application_define(void *);
void    threadx_mutex_suspension_timeout_application_define(void *);
void    threadx_mutex_thread_terminate_application_define(void *);
void    threadx_mutex_priority_inheritance_application_define(void *);
void    threadx_mutex_nested_priority_inheritance_application_define(void *);
void    threadx_mutex_prioritize_application_define(void *);
void    threadx_mutex_information_application_define(void *);

void    threadx_queue_basic_application_define(void *);
void    threadx_queue_basic_two_word_application_define(void *);
void    threadx_queue_basic_four_word_application_define(void *);
void    threadx_queue_basic_eight_word_application_define(void *);
void    threadx_queue_basic_sixteen_word_application_define(void *);
void    threadx_queue_empty_suspension_application_define(void *);
void    threadx_queue_full_suspension_application_define(void *);
void    threadx_queue_suspension_timeout_application_define(void *);
void    threadx_queue_thread_terminate_application_define(void *);
void    threadx_queue_flush_application_define(void *);
void    threadx_queue_flush_no_suspension_application_define(void *);
void    threadx_queue_front_send_application_define(void *);
void    threadx_queue_prioritize_application_define(void *);
void    threadx_queue_information_application_define(void *);

void    threadx_semaphore_basic_application_define(void *);
void    threadx_semaphore_delete_application_define(void *);
void    threadx_semaphore_preemption_application_define(void *);
void    threadx_semaphore_non_preemption_application_define(void *);
void    threadx_semaphore_timeout_application_define(void *);
void    threadx_semaphore_thread_terminate_application_define(void *);
void    threadx_semaphore_prioritize_application_define(void *);
void    threadx_semaphore_ceiling_put_application_define(void *);
void    threadx_semaphore_information_application_define(void *);

void    threadx_thread_basic_execution_application_define(void *);
void    threadx_thread_completed_application_define(void *);
void    threadx_thread_relinquish_application_define(void *);
void    threadx_thread_simple_supsend_application_define(void *);
void    threadx_thread_multiple_suspension_application_define(void *);
void    threadx_thread_multiple_non_current_suspension_application_define(void *);
void    threadx_thread_multi_level_preemption_threshold_application_define(void *);
void    threadx_thread_preemptable_suspension_application_define(void *);
void    threadx_thread_basic_time_slice_application_define(void *);
void    threadx_thread_multiple_time_slice_application_define(void *);
void    threadx_thread_simple_sleep_application_define(void *);
void    threadx_thread_simple_sleep_non_clear_application_define(void *);
void    threadx_thread_sleep_for_100ticks_application_define(void *);
void    threadx_thread_multiple_sleep_application_define(void *);
void    threadx_thread_terminate_delete_application_define(void *);
void    threadx_thread_preemption_change_application_define(void *);
void    threadx_thread_priority_change_application_define(void *);
void    threadx_thread_time_slice_change_application_define(void *);
void    threadx_thread_sleep_terminate_application_define(void *);
void    threadx_thread_delayed_suspension_application_define(void *);
void    threadx_thread_wait_abort_application_define(void *);
void    threadx_thread_wait_abort_and_isr_application_define(void *);
void    threadx_thread_create_preemption_threshold_application_define(void *);
void    threadx_thread_information_application_define(void *);
void    threadx_thread_reset_application_define(void *);
void    threadx_thread_stack_checking_application_define(void *);

void    threadx_time_get_set_application_define(void *);

void    threadx_timer_simple_application_define(void *);
void    threadx_timer_activate_deactivate_application_define(void *);
void    threadx_timer_deactivate_accuracy_application_define(void *);
void    threadx_timer_large_timer_accuracy_application_define(void *);
void    threadx_timer_multiple_application_define(void *);
void    threadx_timer_multiple_accuracy_application_define(void *);
void    threadx_timer_information_application_define(void *);

void    threadx_trace_basic_application_define(void *);
void    test_application_define(void *first_unused_memory);



/* Define the array of test entry points.  */

TEST_ENTRY  test_control_tests[] = 
{
#if CTEST
    test_application_define,
#else

    threadx_block_memory_basic_application_define,
    threadx_block_memory_error_detection_application_define,
    threadx_block_memory_prioritize_application_define,
    threadx_block_memory_suspension_application_define,
    threadx_block_memory_suspension_timeout_application_define,
    threadx_block_memory_thread_terminate_application_define,
    threadx_block_memory_information_application_define,

    threadx_byte_memory_basic_application_define,
    threadx_byte_memory_suspension_application_define,
    threadx_byte_memory_suspension_timeout_application_define,
    threadx_byte_memory_thread_terminate_application_define,
    threadx_byte_memory_prioritize_application_define,
    threadx_byte_memory_thread_contention_application_define,
    threadx_byte_memory_information_application_define, 

    threadx_event_flag_basic_application_define,
    threadx_event_flag_suspension_application_define,
    threadx_event_flag_suspension_consume_application_define,
    threadx_event_flag_suspension_different_bits_application_define,
    threadx_event_flag_suspension_different_bits_consume_application_define,
    threadx_event_flag_suspension_timeout_application_define,
    threadx_event_flag_thread_terminate_application_define,
    threadx_event_flag_single_thread_terminate_application_define,
    threadx_event_flag_isr_set_clear_application_define,
    threadx_event_flag_isr_wait_abort_application_define,
    threadx_event_flag_information_application_define,

    threadx_interrupt_control_application_define,

    threadx_mutex_basic_application_define,
    threadx_mutex_delete_application_define,
    threadx_mutex_preemption_application_define,
    threadx_mutext_no_preemption_application_define,
    threadx_mutex_suspension_timeout_application_define,
    threadx_mutex_thread_terminate_application_define,
    threadx_mutex_priority_inheritance_application_define,
    threadx_mutex_prioritize_application_define,
    threadx_mutex_nested_priority_inheritance_application_define,
    threadx_mutex_information_application_define,

    threadx_queue_basic_application_define,
    threadx_queue_basic_two_word_application_define,
    threadx_queue_basic_four_word_application_define,
    threadx_queue_basic_eight_word_application_define,
    threadx_queue_basic_sixteen_word_application_define,
    threadx_queue_empty_suspension_application_define,
    threadx_queue_full_suspension_application_define,
    threadx_queue_suspension_timeout_application_define,
    threadx_queue_thread_terminate_application_define,
    threadx_queue_flush_application_define,
    threadx_queue_flush_no_suspension_application_define,
    threadx_queue_front_send_application_define,
    threadx_queue_prioritize_application_define,
    threadx_queue_information_application_define,

    threadx_semaphore_basic_application_define,
    threadx_semaphore_delete_application_define,
    threadx_semaphore_preemption_application_define,
    threadx_semaphore_non_preemption_application_define,
    threadx_semaphore_timeout_application_define,
    threadx_semaphore_thread_terminate_application_define,
    threadx_semaphore_prioritize_application_define,
    threadx_semaphore_ceiling_put_application_define,
    threadx_semaphore_information_application_define,

    threadx_thread_basic_execution_application_define,
    threadx_thread_completed_application_define,
    threadx_thread_relinquish_application_define,
    threadx_thread_simple_supsend_application_define,
    threadx_thread_multiple_suspension_application_define,
    threadx_thread_multiple_non_current_suspension_application_define,
    threadx_thread_multi_level_preemption_threshold_application_define,
    threadx_thread_preemptable_suspension_application_define,
    threadx_thread_basic_time_slice_application_define,
    threadx_thread_multiple_time_slice_application_define,
    threadx_thread_simple_sleep_application_define,
    threadx_thread_simple_sleep_non_clear_application_define,
    threadx_thread_sleep_for_100ticks_application_define,
    threadx_thread_multiple_sleep_application_define,
    threadx_thread_terminate_delete_application_define,

    threadx_thread_priority_change_application_define,

    threadx_thread_time_slice_change_application_define,
    threadx_thread_sleep_terminate_application_define,
    threadx_thread_delayed_suspension_application_define,
    threadx_thread_wait_abort_application_define,
    threadx_thread_wait_abort_and_isr_application_define,
    threadx_thread_create_preemption_threshold_application_define,
    threadx_thread_preemption_change_application_define,
    threadx_thread_information_application_define,
    threadx_thread_reset_application_define,
    threadx_thread_stack_checking_application_define,

    threadx_time_get_set_application_define,
    
    threadx_timer_simple_application_define,
    threadx_timer_activate_deactivate_application_define,
    threadx_timer_deactivate_accuracy_application_define,
    threadx_timer_large_timer_accuracy_application_define,
    threadx_timer_multiple_application_define,
    threadx_timer_multiple_accuracy_application_define,
    threadx_timer_information_application_define, 

    threadx_trace_basic_application_define,
#endif

    TX_NULL,
};

/* Define thread prototypes.  */

void  test_control_thread_entry(ULONG thread_input);
void  test_thread_entry(ULONG thread_input);
void  test_thread_entry1(ULONG thread_input);
void  test_control_return(UINT status);
void  test_control_cleanup(void);
void  test_exit_notify(TX_THREAD *thread_ptr, UINT type);


/* Define necessary exernal references.  */

#ifdef __ghs
extern TX_MUTEX                 __ghLockMutex;
#endif

extern TX_TIMER                 *_tx_timer_created_ptr;
extern ULONG                    _tx_timer_created_count;
#ifndef TX_TIMER_PROCESS_IN_ISR
extern TX_THREAD                _tx_timer_thread;
#endif
extern TX_THREAD                *_tx_thread_created_ptr;
extern ULONG                    _tx_thread_created_count;
extern TX_SEMAPHORE             *_tx_semaphore_created_ptr;
extern ULONG                    _tx_semaphore_created_count;
extern TX_QUEUE                 *_tx_queue_created_ptr;
extern ULONG                    _tx_queue_created_count;
extern TX_MUTEX                 *_tx_mutex_created_ptr;
extern ULONG                    _tx_mutex_created_count;
extern TX_EVENT_FLAGS_GROUP     *_tx_event_flags_created_ptr;
extern ULONG                    _tx_event_flags_created_count;
extern TX_BYTE_POOL             *_tx_byte_pool_created_ptr;
extern ULONG                    _tx_byte_pool_created_count;
extern TX_BLOCK_POOL            *_tx_block_pool_created_ptr;
extern ULONG                    _tx_block_pool_created_count;

#ifdef EXTERNAL_EXIT
void external_exit(UINT code);
#endif


/* Define the interrupt processing dispatcher.  The individual tests will set this up when they desire
   asynchrouns processing for testing purposes.  */

void test_interrupt_dispatch(void)
{

#ifndef TX_DISABLE_ERROR_CHECKING

    /* Test calling tx_thread_relinquish from ISR to see if the error checking throws it out.  */
    tx_thread_relinquish();
#endif

    /* Check for something to run... */
    if (test_isr_dispatch)
    {

        (test_isr_dispatch)();    
    }
}


/* Define init timer entry.  */

static void   init_timer_entry(ULONG timer_input)
{

}


#ifndef TX_TIMER_PROCESS_IN_ISR

/* Define the deletion of the system timer thread.  */

void delete_timer_thread(void)
{

    _tx_thread_terminate(&_tx_timer_thread);
    _tx_thread_delete(&_tx_timer_thread);
}

#endif


/* Define main entry point.  */
#ifndef EXTERNAL_MAIN
void main()
{

#ifndef TX_MISRA_ENABLE
#ifndef TX_MANUAL_TEST

    /* Test the pre-initialize path through _tx_initialize_kernel_enter.  */
    _tx_thread_system_state =  TX_INITIALIZE_ALMOST_DONE;
    test_initialize_flag =     1;
    
    /* Call the internal kernel enter function to exercise two paths.  */
    _tx_initialize_kernel_enter();
    _tx_thread_system_state =  0;

#ifndef TX_TIMER_PROCESS_IN_ISR
    threadx_delete_timer_thread =  1;
#endif
#endif
#endif

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}
#endif


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

UCHAR       *pointer;
VOID        (*temp_mutex_release)(TX_THREAD *thread_ptr);
TX_THREAD   *temp_thread;
UINT        old_preemption;
UINT        status;
ULONG       flags;
ULONG       temp;
UINT        i, j;
#if defined(TX_ENABLE_RANDOM_NUMBER_STACK_FILLING) && defined(TX_ENABLE_STACK_CHECKING)
TX_THREAD   *thread_ptr;
#endif


    /* Initialize the test error/success counters.  */
    test_control_successful_tests =  0;
    test_control_failed_tests =      0;
    test_control_system_errors =     0;

    /* Setup a pointer to the first unused memory.  */
    pointer =  (UCHAR *)   &test_control_memory[0];  //first_unused_memory; 

    /* Create the test control thread.  */
    tx_thread_create(&test_control_thread, "test control thread", test_control_thread_entry, 0,  
            pointer, TEST_STACK_SIZE, 
            17, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE;

    /* Create the test thread.  */
    tx_thread_create(&test_thread, "test thread", test_thread_entry, 0,  
            pointer, TEST_STACK_SIZE, 
            15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + TEST_STACK_SIZE;

    /* Create the second test thread.  */
    tx_thread_create(&test_thread1, "test thread 1", test_thread_entry1, 0,  
            pointer, TEST_STACK_SIZE, 
            15, 15, TX_NO_TIME_SLICE, TX_DONT_START);
    pointer =  pointer + TEST_STACK_SIZE;

    /* Suspend the test thread temporarily.  */
    tx_thread_suspend(&test_thread);
    
    /* Resume the test thread again to exercise the resume code fully.  */
    tx_thread_resume(&test_thread);

    /* Timer change just to exercise the code... an error from initialization!  */
    test_timer.tx_timer_id =  TX_TIMER_ID;
    tx_timer_change(&test_timer, 1, 1);

    /* Test mutex created and used in initialization.  */
    test_mutex_from_init =  tx_mutex_create(&init_mutex, "init mutex", TX_INHERIT);
    test_mutex_from_init += tx_mutex_get(&init_mutex, TX_NO_WAIT);
    test_mutex_from_init += tx_mutex_get(&init_mutex, TX_NO_WAIT);
    test_mutex_from_init += tx_mutex_put(&init_mutex);
    test_mutex_from_init += tx_mutex_put(&init_mutex);
    test_mutex_from_init =  tx_mutex_create(&init_mutex_inherit, "init mutex", TX_INHERIT);
    test_mutex_from_init += tx_mutex_get(&init_mutex_inherit, TX_NO_WAIT);
    test_mutex_from_init += tx_mutex_get(&init_mutex_inherit, TX_NO_WAIT);
    test_mutex_from_init += tx_mutex_put(&init_mutex_inherit);
    test_mutex_from_init += tx_mutex_put(&init_mutex_inherit);
    
#ifndef TX_DISABLE_ERROR_CHECKING

    /* Test timer create from initialization.  */
    test_block_pool_create_init =  tx_block_pool_create(&init_block_pool, "init block pool", 10, init_block_pool_area, sizeof(init_block_pool_area));
    
    /* Test byte pool create from initialization.  */
    test_byte_pool_create_init =   tx_byte_pool_create(&init_byte_pool, "init byte pool", init_byte_pool_area, sizeof(init_byte_pool_area));
    test_byte_pool_create_init +=  tx_byte_allocate(&init_byte_pool, (VOID **) &pointer, 20, TX_NO_WAIT);
    test_byte_pool_create_init +=  tx_byte_release(pointer);
    
    /* Test event flag create from initialization.  */
    test_event_flags_from_init =  tx_event_flags_create(&init_event_flags, "init events");
    
    /* Test queue create from initialization.  */
    test_queue_from_init =  tx_queue_create(&init_queue, "init queue", TX_1_ULONG, init_queue_area, sizeof(init_queue_area));
        
    /* Test semaphore create from initialization.  */
    test_semaphore_from_init =  tx_semaphore_create(&init_semaphore, "init semaphore", 0);
    
    /* Test timer creat from initialization.  */
    test_timer_create_init =  tx_timer_create(&init_timer, "init timer", init_timer_entry, 0x5678,
                        100, 200, TX_AUTO_ACTIVATE);

    /* Test calling tx_thread_relinquish to see if the error checking throws it out.  */
    tx_thread_relinquish();
#endif

    /* Remember the free memory pointer.  */
    test_free_memory_ptr =  &tests_memory[0]; //pointer;
    
    /* Clear the ISR dispatch.  */
    test_isr_dispatch =  TX_NULL;

    /* Ensure that _tx_thread_time_slice can handle NULL thread, note that current thread pointer is NULL at this point.  */
    _tx_thread_time_slice();

    /* Test to make sure _tx_thread_time_slice can handle a none-ready thread.  */
    init_test_thread.tx_thread_state =                              TX_IO_DRIVER;
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_TRUE;
    _tx_thread_current_ptr =  &init_test_thread;                    
    _tx_thread_time_slice();
    
    /* Test to make sure _tx_thread_time_slice can handle preemption-threshold set.  */
    init_test_thread.tx_thread_state =                              TX_READY;
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_priority =                           10;
    init_test_thread.tx_thread_preempt_threshold =                  9;
    init_test_thread.tx_thread_ready_next =                         &init_test_thread;
    init_test_thread.tx_thread_ready_previous =                     &init_test_thread;
    _tx_thread_time_slice();
    _tx_thread_current_ptr =  TX_NULL;

    /* Test to make sure _tx_thread_shell_entry can handle a NULL mutex release function pointer.  */
    temp_mutex_release =  _tx_thread_mutex_release;
    temp_thread =         _tx_thread_execute_ptr;
    _tx_thread_mutex_release =  TX_NULL;
    init_test_thread.tx_thread_state =                              TX_READY;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_FALSE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_current_ptr =  &init_test_thread;
    _tx_thread_execute_ptr =  &init_test_thread;
    _tx_thread_entry_exit_notify(&init_test_thread, test_exit_notify);
    _tx_thread_shell_entry();    
    _tx_thread_current_ptr =  TX_NULL;
    _tx_thread_execute_ptr =  temp_thread;
    _tx_thread_mutex_release =  temp_mutex_release;     /* Recover Mutex release pointer.  */
    
    /* Test _tx_thread_system_suspend when not current, preemption is needed but disabled.  */
    temp_thread =         _tx_thread_execute_ptr;
    init_test_thread.tx_thread_state =                              TX_READY;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_FALSE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_execute_ptr =  &init_test_thread;
#ifndef TX_NOT_INTERRUPTABLE
    _tx_thread_preempt_disable++;
#endif
    _tx_thread_system_suspend(&init_test_thread); 
    _tx_thread_execute_ptr =  temp_thread;
    
    /* Test _tx_thread_system_resume when not current, suspending and in a COMPLETED state.  */
    temp_thread =         _tx_thread_execute_ptr;
    init_test_thread.tx_thread_state =                              TX_COMPLETED;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_TRUE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_preempt_disable++;
    _tx_thread_execute_ptr =  &init_test_thread;
    _tx_thread_system_resume(&init_test_thread); 
    _tx_thread_execute_ptr =  temp_thread;

    
    /* Test _tx_thread_system_resume when not current, not suspending and already in a TX_READY state.  */
    temp_thread =         _tx_thread_execute_ptr;
    init_test_thread.tx_thread_state =                              TX_READY;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_FALSE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_preempt_disable++;
    _tx_thread_execute_ptr =  &init_test_thread;
    _tx_thread_system_resume(&init_test_thread); 
    _tx_thread_execute_ptr =  temp_thread;

    /* Test _tx_thread_system_resume when not current, suspending and in a TERMINATED state.  */
    temp_thread =         _tx_thread_execute_ptr;
    init_test_thread.tx_thread_state =                              TX_TERMINATED;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_TRUE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_preempt_disable++;
    _tx_thread_execute_ptr =  &init_test_thread;
    _tx_thread_system_resume(&init_test_thread); 
    _tx_thread_execute_ptr =  temp_thread;
  
    /* Test tx_thread_resume to test the saved_thread_ptr being NULL.  */
    temp_thread =         _tx_thread_execute_ptr;
    _tx_thread_execute_ptr =  TX_NULL;
    tx_thread_resume(&test_thread1);
    tx_thread_suspend(&test_thread1);
    _tx_thread_execute_ptr =  temp_thread;

    /* Test preemption change when the new priority is the same as the threshold.  */
    init_test_thread.tx_thread_state =                              TX_SUSPENDED;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_FALSE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_user_priority =                      10;
    init_test_thread.tx_thread_user_preempt_threshold =             10;
    init_test_thread.tx_thread_priority =                           10;
    init_test_thread.tx_thread_preempt_threshold =                  10;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_preemption_change(&init_test_thread, 10, &old_preemption);
    
#ifndef TX_NOT_INTERRUPTABLE
    
    /* Test semaphore cleanup with an invalid semaphore ID.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_semaphore;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_semaphore_cleanup);
    cleanup_semaphore.tx_semaphore_id =                 0;
    cleanup_semaphore.tx_semaphore_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_semaphore_cleanup(&init_test_thread, 0);

    /* Test semaphore cleanup with an invalid suspension sequence.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_semaphore;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_semaphore_cleanup);
    cleanup_semaphore.tx_semaphore_id =                 0;
    cleanup_semaphore.tx_semaphore_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_semaphore_cleanup(&init_test_thread, 1);
    
    /* Test semaphore cleanup with a NULL semaphore pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  TX_NULL;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_semaphore_cleanup);
    cleanup_semaphore.tx_semaphore_id =                 TX_SEMAPHORE_ID;
    cleanup_semaphore.tx_semaphore_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_semaphore_cleanup(&init_test_thread, 0);

    /* Test semaphore cleanup with an valid semaphore ID but a suspension count of 0.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_semaphore;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_semaphore_cleanup);
    cleanup_semaphore.tx_semaphore_id =                 TX_SEMAPHORE_ID;
    cleanup_semaphore.tx_semaphore_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_semaphore_cleanup(&init_test_thread, 0);

    /* Test queue cleanup with a NULL cleanup pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_queue;
    init_test_thread.tx_thread_suspend_cleanup =        TX_NULL;
    cleanup_queue.tx_queue_id =                         0;
    cleanup_queue.tx_queue_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_queue_cleanup(&init_test_thread, 0);

    /* Test queue cleanup with a NULL queue pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  TX_NULL;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_queue_cleanup);
    cleanup_queue.tx_queue_id =                         0;
    cleanup_queue.tx_queue_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_queue_cleanup(&init_test_thread, 0);

    /* Test queue cleanup with an invalid queue ID.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_queue;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_queue_cleanup);
    cleanup_queue.tx_queue_id =                         0;
    cleanup_queue.tx_queue_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_queue_cleanup(&init_test_thread, 0);

    /* Test queue cleanup with an invalid suspension sequence.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_queue;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_queue_cleanup);
    cleanup_queue.tx_queue_id =                         0;
    cleanup_queue.tx_queue_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_queue_cleanup(&init_test_thread, 1);
    
    /* Test queue cleanup with an valid queue ID but a suspension count of 0.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_queue;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_queue_cleanup);
    cleanup_queue.tx_queue_id =                         TX_QUEUE_ID;
    cleanup_queue.tx_queue_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_queue_cleanup(&init_test_thread, 0);

    /* Test mutex cleanup with a NULL cleanup pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_mutex;
    init_test_thread.tx_thread_suspend_cleanup =        TX_NULL;
    cleanup_mutex.tx_mutex_id =                         0;
    cleanup_mutex.tx_mutex_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence = 0;
    _tx_mutex_cleanup(&init_test_thread, 0);

    /* Test mutex cleanup with a NULL mutex pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  TX_NULL;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_mutex_cleanup);
    cleanup_mutex.tx_mutex_id =                         0;
    cleanup_mutex.tx_mutex_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_mutex_cleanup(&init_test_thread, 0);

    /* Test mutex cleanup with an invalid mutex ID.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_mutex;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_mutex_cleanup);
    cleanup_mutex.tx_mutex_id =                         0;
    cleanup_mutex.tx_mutex_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_mutex_cleanup(&init_test_thread, 0);

    /* Test mutex cleanup with an invalid suspension sequence.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_mutex;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_mutex_cleanup);
    cleanup_mutex.tx_mutex_id =                         0;
    cleanup_mutex.tx_mutex_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_mutex_cleanup(&init_test_thread, 1);
    
    /* Test mutex cleanup with an valid mutex ID but a suspension count of 0.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_mutex;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_mutex_cleanup);
    cleanup_mutex.tx_mutex_id =                         TX_MUTEX_ID;
    cleanup_mutex.tx_mutex_suspended_count =            0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_mutex_cleanup(&init_test_thread, 0);
    
    /* Test event flag cleanup with a NULL cleanup pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_event_flags;
    init_test_thread.tx_thread_suspend_cleanup =        TX_NULL;
    cleanup_event_flags.tx_event_flags_group_id =       0;
    cleanup_event_flags.tx_event_flags_group_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_event_flags_cleanup(&init_test_thread, 0);

    /* Test event flag cleanup with a NULL event flag pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  TX_NULL;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_event_flags_cleanup);
    cleanup_event_flags.tx_event_flags_group_id =       0;
    cleanup_event_flags.tx_event_flags_group_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_event_flags_cleanup(&init_test_thread, 0);

    /* Test event flag cleanup with an invalid ID.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_event_flags;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_event_flags_cleanup);
    cleanup_event_flags.tx_event_flags_group_id =       0;
    cleanup_event_flags.tx_event_flags_group_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_event_flags_cleanup(&init_test_thread, 0);
    
    /* Test event flag cleanup with an invalid suspension sequence.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_event_flags;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_event_flags_cleanup);
    cleanup_event_flags.tx_event_flags_group_id =       0;
    cleanup_event_flags.tx_event_flags_group_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_event_flags_cleanup(&init_test_thread, 1);

    /* Test event flag cleanup with an valid ID but a suspension count of 0.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_event_flags;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_event_flags_cleanup);
    cleanup_event_flags.tx_event_flags_group_id =       TX_EVENT_FLAGS_ID;
    cleanup_event_flags.tx_event_flags_group_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_event_flags_cleanup(&init_test_thread, 0);
    
    /* Test block pool cleanup with a NULL cleanup pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_block_pool;
    init_test_thread.tx_thread_suspend_cleanup =        TX_NULL;
    cleanup_block_pool.tx_block_pool_id =               0;
    cleanup_block_pool.tx_block_pool_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_block_pool_cleanup(&init_test_thread, 0);

    /* Test block pool cleanup with a NULL block pool pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  TX_NULL;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_block_pool_cleanup);
    cleanup_block_pool.tx_block_pool_id =               0;
    cleanup_block_pool.tx_block_pool_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_block_pool_cleanup(&init_test_thread, 0);

    /* Test block pool cleanup with an invalid ID.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_block_pool;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_block_pool_cleanup);
    cleanup_block_pool.tx_block_pool_id =               0;
    cleanup_block_pool.tx_block_pool_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_block_pool_cleanup(&init_test_thread, 0);

    /* Test block pool cleanup with an invalid suspension sequence.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_block_pool;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_block_pool_cleanup);
    cleanup_block_pool.tx_block_pool_id =               0;
    cleanup_block_pool.tx_block_pool_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_block_pool_cleanup(&init_test_thread, 1);
    
    /* Test block pool cleanup with an valid ID but a suspension count of 0.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_block_pool;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_block_pool_cleanup);
    cleanup_block_pool.tx_block_pool_id =               TX_BLOCK_POOL_ID;
    cleanup_block_pool.tx_block_pool_suspended_count =  0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_block_pool_cleanup(&init_test_thread, 0);

    /* Test byte pool cleanup with a NULL cleanup pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_byte_pool;
    init_test_thread.tx_thread_suspend_cleanup =        TX_NULL;
    cleanup_byte_pool.tx_byte_pool_id =                 0;
    cleanup_byte_pool.tx_byte_pool_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_byte_pool_cleanup(&init_test_thread, 0);

    /* Test byte pool cleanup with a NULL byte pool pointer.  */
    init_test_thread.tx_thread_suspend_control_block =  TX_NULL;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_byte_pool_cleanup);
    cleanup_byte_pool.tx_byte_pool_id =                 0;
    cleanup_byte_pool.tx_byte_pool_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_byte_pool_cleanup(&init_test_thread, 0);

    /* Test byte pool cleanup with an invalid ID.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_byte_pool;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_byte_pool_cleanup);
    cleanup_byte_pool.tx_byte_pool_id =                 0;
    cleanup_byte_pool.tx_byte_pool_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_byte_pool_cleanup(&init_test_thread, 0);

    /* Test byte pool cleanup with an invalid suspension sequence.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_byte_pool;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_byte_pool_cleanup);
    cleanup_byte_pool.tx_byte_pool_id =                 0;
    cleanup_byte_pool.tx_byte_pool_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_byte_pool_cleanup(&init_test_thread, 1);
    
    /* Test byte pool cleanup with an valid ID but a suspension count of 0.  */
    init_test_thread.tx_thread_suspend_control_block =  (VOID *) &cleanup_byte_pool;
    init_test_thread.tx_thread_suspend_cleanup =        &(_tx_byte_pool_cleanup);
    cleanup_byte_pool.tx_byte_pool_id =                 TX_BYTE_POOL_ID;
    cleanup_byte_pool.tx_byte_pool_suspended_count =    0;
    init_test_thread.tx_thread_suspension_sequence =    0;
    _tx_byte_pool_cleanup(&init_test_thread, 0);
#endif
    
#ifndef TX_ENABLE_EVENT_TRACE

    /* Call ISR trace events when trace is not enabled.  */

    /* Call trace ISR enter event insert.  */
    tx_trace_isr_enter_insert(1);

    /* Call trace ISR exit event insert.  */
    tx_trace_isr_exit_insert(1);

#endif

    /* Test the list start path in timer info get and timer deactivate.  */
    test_timer.tx_timer_id =  0;
    _tx_timer_create(&test_timer, "init timer", init_timer_entry, 0x5678, 100, 200, TX_AUTO_ACTIVATE);
    _timer_list_start_backup =  _tx_timer_list_start;
    _tx_timer_list_start =  _tx_timer_list_end;
    _tx_timer_info_get(&test_timer, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL);
    _tx_timer_deactivate(&test_timer);
    _tx_timer_list_start =  _timer_list_start_backup;
    _tx_timer_deactivate(&test_timer);
    _tx_timer_delete(&test_timer);

    /* Test the stack analyze function with a dummy thread.  */
    
    /* Clear the test stack analyze flag.  */
    test_stack_analyze_flag =  0;
    
    /* Make a fake thread with a fake stack.  */
    test_thread2.tx_thread_id =  TX_THREAD_ID;
#if defined(TX_ENABLE_RANDOM_NUMBER_STACK_FILLING) && defined(TX_ENABLE_STACK_CHECKING)

    /* Set the thread pointer.  */
    thread_ptr = &(test_thread2);

    /* Initialize the stack fill value.  */
    thread_ptr -> tx_thread_stack_fill_value = (0xFEFEFEFEUL);

#endif
    for (i = 0; i < (sizeof(test_thread2_stack)/sizeof(ULONG)); i++)
    {
        /* Set the fake thread stack to the fill pattern.  */
        test_thread2_stack[i] = TX_STACK_FILL;
    }
    
    /* Setup index to last point.  */
    i =  (sizeof(test_thread2_stack)/sizeof(ULONG)) - 1;
    
    /* Setup the stack start and end pointers.  */
    test_thread2.tx_thread_stack_start =        &(test_thread2_stack[0]);
    test_thread2.tx_thread_stack_end =          &(test_thread2_stack[i]);
    test_thread2.tx_thread_stack_size =         sizeof(test_thread2_stack);
    test_thread2.tx_thread_stack_highest_ptr =  test_thread2.tx_thread_stack_end;
    test_thread2.tx_thread_stack_ptr =          test_thread2.tx_thread_stack_start;
    
    /* Fill 20 words of stack.  */
    for (j = 0; j < 20; j++)
    {
        /* Fill the stack with 0s.  */
        test_thread2_stack[i--] =  0;
    }
            
    /* Call the analyze stack function.  */
    _tx_thread_stack_analyze(&test_thread2);
    
    /* Call it again for no change coverage.  */
    _tx_thread_stack_analyze(&test_thread2);
       
    /* Fill 99 words of stack.  */
    for (j = 0; j < 99; j++)
    {
        /* Fill the stack with 1s.  */
        test_thread2_stack[i--] =  1;
    }

    /* Call the analyze stack function.  */
    _tx_thread_stack_analyze(&test_thread2);
    
    /* Call it again for no change coverage.  */
    _tx_thread_stack_analyze(&test_thread2);

#ifndef TX_MANUAL_TEST
    
    /* Now set the flag to 1 to cause the thread ID to be cleared.  */
    test_stack_analyze_flag =  1;
    
    /* Call stack analyze with an ID that is cleared in the middle.  */
    _tx_thread_stack_analyze(&test_thread2);
    
    /* Restore the ID.  */
    test_thread2.tx_thread_id =  TX_THREAD_ID;
    
    /* Now set the flag to 2 to cause the stack ptr to be equal to the start of the stack.  */
    test_stack_analyze_flag =  2;
    
    /* Call stack analyze with an ID that is cleared in the middle.  */
    _tx_thread_stack_analyze(&test_thread2);
    test_thread2.tx_thread_stack_highest_ptr =  test_thread2.tx_thread_stack_end;

    /* Now set the flag to 3 to cause the stack pointer to not have the fill pattern.  */
    test_stack_analyze_flag =  3;
    
    /* Call stack analyze with an ID that is cleared in the middle.  */
    _tx_thread_stack_analyze(&test_thread2);
#endif
    
    /* Test error condition on _tx_queue_flush.  */
    test_queue.tx_queue_enqueued =         1;
    test_queue.tx_queue_suspended_count =  1;
    test_queue.tx_queue_suspension_list =  TX_NULL;
    
    /* Call _tx_queue_flush to test the thread NULL check.  */
    _tx_queue_flush(&test_queue);

#ifndef TX_NOT_INTERRUPTABLE

    /* Make sure the suspension cancelled path is exercised in tx_thread_system_resume... This normally happens,
       however, sometimes on less real-time platforms the timing is not guaranteed.  */

    /* Increment the preempt disable flag.  */
    _tx_thread_preempt_disable++;
  
    /* Build a thread control block with fake info.  */  
    test_thread3.tx_thread_suspending =                         TX_TRUE;
    test_thread3.tx_thread_state =                              TX_SUSPENDED;
    test_thread3.tx_thread_delayed_suspend =                    TX_FALSE;
    test_thread3.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    test_thread3.tx_thread_stack_start =                        (VOID *) &test_thread2_stack[0];
    test_thread3.tx_thread_stack_end =                          (VOID *) &test_thread2_stack[255];
    test_thread3.tx_thread_stack_ptr =                          (VOID *) &test_thread2_stack[255];

    /* Call the system resume function directly to insure the suspension cancel works properly.  */
    _tx_thread_system_resume(&test_thread3);
#endif

    /* Test preempt-disable safeguard.  */

    /* Increment the preempt disable flag.  */
    _tx_thread_preempt_disable++;
    
    /* Test block pool suspenson safeguard.  */
    fake_block_pool.tx_block_pool_available =  0;
    status =  _tx_block_allocate(&fake_block_pool, (VOID **) &pointer, TX_WAIT_FOREVER);
    if (status != TX_NO_MEMORY)
        test_control_system_errors++;    

    /* Test byte pool suspension safeguard.  */
    fake_byte_pool.tx_byte_pool_fragments =  2;
    fake_byte_pool.tx_byte_pool_available =  0;
    status =  _tx_byte_allocate(&fake_byte_pool, (VOID **) &pointer, 1000, TX_WAIT_FOREVER);
    if (status != TX_NO_MEMORY)
        test_control_system_errors++;    

    /* Test event flags suspension safeguard.  */
    fake_event_flags.tx_event_flags_group_current =  0;
    status =  _tx_event_flags_get(&fake_event_flags, 1, TX_AND, &flags, TX_WAIT_FOREVER);
    if (status != TX_NO_EVENTS)
        test_control_system_errors++;    

    /* Test mutex suspension safeguard.  */
    fake_mutex.tx_mutex_ownership_count =  1;
    fake_mutex.tx_mutex_inherit =  0;
    fake_mutex.tx_mutex_owner =  &init_test_thread;
    status =  _tx_mutex_get(&fake_mutex, TX_WAIT_FOREVER);
    if (status != TX_NOT_AVAILABLE)
        test_control_system_errors++;    

    /* Test queue front send suspension safeguard.  */
    fake_queue.tx_queue_available_storage =  0;
    status =  _tx_queue_front_send(&fake_queue, (VOID *) pointer, TX_WAIT_FOREVER);
    if (status != TX_QUEUE_FULL)
        test_control_system_errors++;    
    
    /* Test queue receive suspension safeguard.  */
    fake_queue.tx_queue_enqueued =  0;
    status =  _tx_queue_receive(&fake_queue, (VOID **) &pointer, TX_WAIT_FOREVER);
    if (status != TX_QUEUE_EMPTY)
        test_control_system_errors++;    

    /* Test queue send suspension safeguard.  */
    fake_queue.tx_queue_available_storage =  0;
    status =  _tx_queue_send(&fake_queue, (VOID *) pointer, TX_WAIT_FOREVER);
    if (status != TX_QUEUE_FULL)
        test_control_system_errors++;    
        
    /* Test semaphore suspension safeguard.  */
    fake_semaphore.tx_semaphore_count =  0;
    status =  _tx_semaphore_get(&fake_semaphore, TX_WAIT_FOREVER);
    if (status != TX_NO_INSTANCE)
        test_control_system_errors++;    

    /* Test thread sleep suspension safeguard.  */
    _tx_thread_current_ptr =  &init_test_thread;
    temp = _tx_thread_system_state;
    _tx_thread_system_state =  0;
    status =  _tx_thread_sleep(10);
    if (status != TX_CALLER_ERROR)
        test_control_system_errors++;    

    /* Test thread suspend suspension safeguard.  */
    init_test_thread.tx_thread_state =  TX_READY;
    status =  _tx_thread_suspend(&init_test_thread);
    if (status != TX_SUSPEND_ERROR)
        test_control_system_errors++;    
    _tx_thread_system_state =  temp;
    _tx_thread_current_ptr =  TX_NULL;

    
    /* Decrement the preempt disable flag.  */
    _tx_thread_preempt_disable--;
}



/* Define the test control thread.  This thread is responsible for dispatching all of the 
   tests in the ThreadX test suite.  */

void  test_control_thread_entry(ULONG thread_input)
{

UINT    i;

    /* Raise the priority of the control thread to 0.  */
    tx_thread_priority_change(&test_control_thread, 0, &i);
#ifdef CTEST
    test_control_cleanup();
#endif

    /* Print out banner.  */
    printf("********************** ThreadX Validation/Regression Test Suite *********************************\n\n");

    /* Print version id.  */
    printf("Version: %s\n\n", _tx_version_id);

    /* Print out the tests... */
    printf("Running validation/regression test:\n\n");

    /* Loop to process all tests...  */
    i =  0;
    while (test_control_tests[i].test_entry != TX_NULL)
    {

        /* Clear the ISR dispatch.  */
        test_isr_dispatch =  TX_NULL;

        /* Dispatch the test.  */
        (test_control_tests[i++].test_entry)(test_free_memory_ptr);

        /* Clear the ISR dispatch.  */
        test_isr_dispatch =  TX_NULL;

        /* Suspend control test to allow test to run.  */
        tx_thread_suspend(&test_control_thread);
    
        /* Test finished, cleanup in preparation for the next test.  */
        test_control_cleanup();
    }

    /* Finished with all tests, print results and return!  */
    printf("**** Testing Complete ****\n");
    printf("**** Test Summary:  Tests Passed:  %lu   Tests Failed:  %lu    System Errors:   %lu\n", test_control_successful_tests, test_control_failed_tests, test_control_system_errors);
#ifndef EXTERNAL_EXIT
    exit(test_control_failed_tests + test_control_system_errors);
#else
    external_exit(test_control_failed_tests + test_control_system_errors);
#endif 
}


void  test_control_return(UINT status)
{

UINT    old_posture =  TX_INT_ENABLE;


    /* Save the status in a global.  */
    test_control_return_status =  status;

    /* Ensure interrupts are enabled.  */
    old_posture =  tx_interrupt_control(TX_INT_ENABLE);

    /* Determine if it was successful or not.  */
    if (status)
        test_control_failed_tests++;
    else
        test_control_successful_tests++;

    /* Now check for system errors.  */
    
    /* Is preempt disable flag set?  */
    if (_tx_thread_preempt_disable)
    {
    
        /* System error - preempt disable should never be set inside of a thread!  */
        printf("    ***** SYSTEM ERROR ***** _tx_thread_preempt_disable is non-zero!\n");
        test_control_system_errors++;
    }

    /* Is system state set?  */
    if (_tx_thread_system_state)
    {
    
        /* System error - system state should never be set inside of a thread!  */
        printf("    ***** SYSTEM ERROR ***** _tx_thread_system_state is non-zero!\n");
        test_control_system_errors++;
    }

    /* Are interrupts disabled?  */    
    if (old_posture == TX_INT_DISABLE)
    {
    
        /* System error - interrupts should alwasy be enabled in our test threads!  */
        printf("    ***** SYSTEM ERROR ***** test returned with interrupts disabled!\n");
        test_control_system_errors++;
    }

    /* Resume the control thread to fully exit the test.  */
    tx_thread_resume(&test_control_thread);
}


void  test_control_cleanup(void)
{

TX_MUTEX    *mutex_ptr;
TX_THREAD   *thread_ptr;


    /* Delete all queues.  */
    while(_tx_queue_created_ptr)
    {

        /* Delete queue.  */
        tx_queue_delete(_tx_queue_created_ptr);
    }

    /* Delete all semaphores.  */
    while(_tx_semaphore_created_ptr)
    {

        /* Delete semaphore.  */
        tx_semaphore_delete(_tx_semaphore_created_ptr);
    }

    /* Delete all event flag groups.  */
    while(_tx_event_flags_created_ptr)
    {

        /* Delete event flag group.  */
        tx_event_flags_delete(_tx_event_flags_created_ptr);
    }

    /* Delete all byte pools.  */
    while(_tx_byte_pool_created_ptr)
    {

        /* Delete byte pool.  */
        tx_byte_pool_delete(_tx_byte_pool_created_ptr);
    }

    /* Delete all block pools.  */
    while(_tx_block_pool_created_ptr)
    {

        /* Delete block pool.  */
        tx_block_pool_delete(_tx_block_pool_created_ptr);
    }

    /* Delete all timers.  */
    while(_tx_timer_created_ptr)
    {

        /* Deactivate timer.  */
        tx_timer_deactivate(_tx_timer_created_ptr);

        /* Delete timer.  */
        tx_timer_delete(_tx_timer_created_ptr);
    }

    /* Delete all mutexes (except for system mutex).  */
    while(_tx_mutex_created_ptr)
    {

        /* Setup working mutex pointer.  */
        mutex_ptr =  _tx_mutex_created_ptr;

#ifdef __ghs

        /* Determine if the mutex is the GHS system mutex.  If so, don't delete!  */
        if (mutex_ptr == &__ghLockMutex)
        {

            /* Move to next mutex.  */
            mutex_ptr =  mutex_ptr -> tx_mutex_created_next;
        }

        /* Determine if there are no more mutexes to delete.  */
        if (_tx_mutex_created_count == 1)
            break;
#endif

        /* Delete mutex.  */
        tx_mutex_delete(mutex_ptr);
    }

    /* Delete all threads, except for timer thread, and test control thread.  */
    while (_tx_thread_created_ptr)
    {

        /* Setup working pointer.  */
        thread_ptr =  _tx_thread_created_ptr;


#ifdef TX_TIMER_PROCESS_IN_ISR

        /* Determine if there are more threads to delete.  */
        if (_tx_thread_created_count == 1)
            break;

        /* Determine if this thread is the test control thread.  */
        if (thread_ptr == &test_control_thread)
        {

            /* Move to the next thread pointer.  */
            thread_ptr =  thread_ptr -> tx_thread_created_next;
        }
#else

        /* Determine if there are more threads to delete.  */
        if (_tx_thread_created_count == 2)
            break;

        /* Move to the thread not protected.  */
        while ((thread_ptr == &_tx_timer_thread) || (thread_ptr == &test_control_thread))
        {

            /* Yes, move to the next thread.  */
            thread_ptr =  thread_ptr -> tx_thread_created_next;
        }
#endif

        /* First terminate the thread to ensure it is ready for deletion.  */
        tx_thread_terminate(thread_ptr);

        /* Delete the thread.  */
        tx_thread_delete(thread_ptr);
    }

    /* At this point, only the test control thread and the system timer thread and/or mutex should still be
       in the system.  */
}

void  test_thread_entry(ULONG thread_input)
{

    /* Resume the next test thread.  */
    tx_thread_resume(&test_thread1);

    /* Suspend this thread but with preemption disabled, so we will actually return.  */
    _tx_thread_preempt_disable++;
    tx_thread_suspend(&test_thread);
    
    /* Now perform a fake thread resume to cause preemption and exercise the path in _tx_thread_system_resume that returns to the scheduler.  */
    init_test_thread.tx_thread_state =                              TX_TERMINATED;
    init_test_thread.tx_thread_suspend_cleanup =                    TX_NULL;  
    init_test_thread.tx_thread_new_time_slice =                     0;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_suspending =                         TX_TRUE;
    init_test_thread.tx_thread_timer.tx_timer_internal_list_head =  TX_NULL;
    init_test_thread.tx_thread_entry =                              test_thread_entry1;
    _tx_thread_system_resume(&init_test_thread); 

    /* We should not get back here!  */    
}


void  test_thread_entry1(ULONG thread_input)
{

    /* Do nothing, just return!  */
}


void   test_exit_notify(TX_THREAD *thread_ptr, UINT type)
{

    /* Clear the suspending flag to short-circuit the suspension.  */
    thread_ptr -> tx_thread_suspending = TX_FALSE;   
}


__attribute__((weak)) void  abort_all_threads_suspended_on_mutex(void)
{
}

__attribute__((weak)) void  suspend_lowest_priority(void)
{
}

__attribute__((weak)) void abort_and_resume_byte_allocating_thread(void)
{
}
