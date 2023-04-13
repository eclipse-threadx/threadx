/* This test is designed to test kernel setup functionality in ThreadX.  */

#include   <stdio.h>
#include   "tx_api.h"
#include   "tx_initialize.h"
#include   "tx_thread.h"


TEST_FLAG               test_forced_mutex_timeout;
TEST_FLAG               threadx_mutex_suspension_put_test;
TEST_FLAG               threadx_mutex_suspension_priority_test;
TEST_FLAG               threadx_byte_allocate_loop_test;
TEST_FLAG               test_initialize_flag; 
TEST_FLAG               threadx_byte_release_loop_test;
TEST_FLAG               test_stack_analyze_flag;


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
UINT            mutex_priority_change_extension_selection; 
UINT            priority_change_extension_selection;


__attribute__((weak)) void  abort_all_threads_suspended_on_mutex(void)
{
}

__attribute__((weak)) void  suspend_lowest_priority(void)
{
}

__attribute__((weak)) void abort_and_resume_byte_allocating_thread(void)
{
}

void main()
{

    /* Setup the ThreadX kernel.  */
    _tx_initialize_kernel_setup();

    if (_tx_thread_system_state == TX_INITIALIZE_ALMOST_DONE)
    {
        printf("Running Initialize Kernel Setup Test................................ SUCCESS!\n");
        exit(0);
    }
    else
    {
        printf("Running Initialize Kernel Setup Test................................ ERROR!\n");
        exit(1);
    }
}

void test_application_define(void *first_unused_memory){}
void tx_application_define(void *first_unused_memory){}


#ifndef TX_TIMER_PROCESS_IN_ISR

/* Define the deletion of the system timer thread.  */

extern TX_THREAD _tx_timer_thread;
TEST_FLAG threadx_delete_timer_thread;
void delete_timer_thread(void)
{

    _tx_thread_terminate(&_tx_timer_thread);
    _tx_thread_delete(&_tx_timer_thread);
}

#endif