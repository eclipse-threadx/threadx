#include "tx_api.h"

typedef unsigned int    TEST_FLAG;
TEST_FLAG        threadx_byte_allocate_loop_test;
TEST_FLAG        threadx_byte_release_loop_test;
TEST_FLAG        threadx_mutex_suspension_put_test;
TEST_FLAG        threadx_mutex_suspension_priority_test;
#ifndef TX_TIMER_PROCESS_IN_ISR
TEST_FLAG        threadx_delete_timer_thread;
#endif

void             abort_and_resume_byte_allocating_thread(void){}
void             abort_all_threads_suspended_on_mutex(void){}
void             suspend_lowest_priority(void){}
#ifndef TX_TIMER_PROCESS_IN_ISR
void             delete_timer_thread(void){}
#endif
TEST_FLAG        test_stack_analyze_flag;
TEST_FLAG        test_initialize_flag;
TEST_FLAG        test_forced_mutex_timeout;