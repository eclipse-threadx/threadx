cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
cmake_policy(SET CMP0057 NEW)

project(regression_test LANGUAGES C)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../regression)

set(regression_test_cases
    ${SOURCE_DIR}/threadx_block_memory_basic_test.c
    ${SOURCE_DIR}/threadx_block_memory_error_detection_test.c
    ${SOURCE_DIR}/threadx_block_memory_information_test.c
    ${SOURCE_DIR}/threadx_block_memory_prioritize_test.c
    ${SOURCE_DIR}/threadx_block_memory_suspension_test.c
    ${SOURCE_DIR}/threadx_block_memory_suspension_timeout_test.c
    ${SOURCE_DIR}/threadx_block_memory_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_byte_memory_basic_test.c
    ${SOURCE_DIR}/threadx_byte_memory_information_test.c
    ${SOURCE_DIR}/threadx_byte_memory_prioritize_test.c
    ${SOURCE_DIR}/threadx_byte_memory_suspension_test.c
    ${SOURCE_DIR}/threadx_byte_memory_suspension_timeout_test.c
    ${SOURCE_DIR}/threadx_byte_memory_thread_contention_test.c
    ${SOURCE_DIR}/threadx_byte_memory_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_event_flag_basic_test.c
    ${SOURCE_DIR}/threadx_event_flag_information_test.c
    ${SOURCE_DIR}/threadx_event_flag_isr_set_clear_test.c
    ${SOURCE_DIR}/threadx_event_flag_isr_wait_abort_test.c
    ${SOURCE_DIR}/threadx_event_flag_single_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_event_flag_suspension_consume_test.c
    ${SOURCE_DIR}/threadx_event_flag_suspension_different_bits_consume_test.c
    ${SOURCE_DIR}/threadx_event_flag_suspension_different_bits_test.c
    ${SOURCE_DIR}/threadx_event_flag_suspension_test.c
    ${SOURCE_DIR}/threadx_event_flag_suspension_timeout_test.c
    ${SOURCE_DIR}/threadx_event_flag_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_interrupt_control_test.c
    ${SOURCE_DIR}/threadx_mutex_basic_test.c
    ${SOURCE_DIR}/threadx_mutex_delete_test.c
    ${SOURCE_DIR}/threadx_mutex_information_test.c
    ${SOURCE_DIR}/threadx_mutex_nested_priority_inheritance_test.c
    ${SOURCE_DIR}/threadx_mutex_no_preemption_test.c
    ${SOURCE_DIR}/threadx_mutex_preemption_test.c
    ${SOURCE_DIR}/threadx_mutex_priority_inheritance_test.c
    ${SOURCE_DIR}/threadx_mutex_proritize_test.c
    ${SOURCE_DIR}/threadx_mutex_suspension_timeout_test.c
    ${SOURCE_DIR}/threadx_mutex_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_queue_basic_eight_word_test.c
    ${SOURCE_DIR}/threadx_queue_basic_four_word_test.c
    ${SOURCE_DIR}/threadx_queue_basic_one_word_test.c
    ${SOURCE_DIR}/threadx_queue_basic_sixteen_word_test.c
    ${SOURCE_DIR}/threadx_queue_basic_two_word_test.c
    ${SOURCE_DIR}/threadx_queue_empty_suspension_test.c
    ${SOURCE_DIR}/threadx_queue_flush_no_suspension_test.c
    ${SOURCE_DIR}/threadx_queue_flush_test.c
    ${SOURCE_DIR}/threadx_queue_front_send_test.c
    ${SOURCE_DIR}/threadx_queue_full_suspension_test.c
    ${SOURCE_DIR}/threadx_queue_information_test.c
    ${SOURCE_DIR}/threadx_queue_prioritize.c
    ${SOURCE_DIR}/threadx_queue_suspension_timeout_test.c
    ${SOURCE_DIR}/threadx_queue_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_semaphore_basic_test.c
    ${SOURCE_DIR}/threadx_semaphore_ceiling_put_test.c
    ${SOURCE_DIR}/threadx_semaphore_delete_test.c
    ${SOURCE_DIR}/threadx_semaphore_information_test.c
    ${SOURCE_DIR}/threadx_semaphore_non_preemption_test.c
    ${SOURCE_DIR}/threadx_semaphore_preemption_test.c
    ${SOURCE_DIR}/threadx_semaphore_prioritize.c
    ${SOURCE_DIR}/threadx_semaphore_thread_terminate_test.c
    ${SOURCE_DIR}/threadx_semaphore_timeout_test.c
    ${SOURCE_DIR}/threadx_thread_basic_execution_test.c
    ${SOURCE_DIR}/threadx_thread_basic_time_slice_test.c
    ${SOURCE_DIR}/threadx_thread_completed_test.c
    ${SOURCE_DIR}/threadx_thread_create_preemption_threshold_test.c
    ${SOURCE_DIR}/threadx_thread_delayed_suspension_test.c
    ${SOURCE_DIR}/threadx_thread_information_test.c
    ${SOURCE_DIR}/threadx_thread_multi_level_preemption_threshold_test.c
    ${SOURCE_DIR}/threadx_thread_multiple_non_current_test.c
    ${SOURCE_DIR}/threadx_thread_multiple_sleep_test.c
    ${SOURCE_DIR}/threadx_thread_multiple_suspension_test.c
    ${SOURCE_DIR}/threadx_thread_multiple_time_slice_test.c
    ${SOURCE_DIR}/threadx_thread_preemptable_suspension_test.c
    ${SOURCE_DIR}/threadx_thread_preemption_change_test.c
    ${SOURCE_DIR}/threadx_thread_priority_change.c
    ${SOURCE_DIR}/threadx_thread_relinquish_test.c
    ${SOURCE_DIR}/threadx_thread_reset_test.c
    ${SOURCE_DIR}/threadx_thread_simple_sleep_non_clear_test.c
    ${SOURCE_DIR}/threadx_thread_simple_sleep_test.c
    ${SOURCE_DIR}/threadx_thread_simple_suspend_test.c
    ${SOURCE_DIR}/threadx_thread_sleep_for_100ticks_test.c
    ${SOURCE_DIR}/threadx_thread_sleep_terminate_test.c
    ${SOURCE_DIR}/threadx_thread_stack_checking_test.c
    ${SOURCE_DIR}/threadx_thread_terminate_delete_test.c
    ${SOURCE_DIR}/threadx_thread_time_slice_change_test.c
    ${SOURCE_DIR}/threadx_thread_wait_abort_and_isr_test.c
    ${SOURCE_DIR}/threadx_thread_wait_abort_test.c
    ${SOURCE_DIR}/threadx_time_get_set_test.c
    ${SOURCE_DIR}/threadx_timer_activate_deactivate_test.c
    ${SOURCE_DIR}/threadx_timer_deactivate_accuracy_test.c
    ${SOURCE_DIR}/threadx_timer_information_test.c
    ${SOURCE_DIR}/threadx_timer_large_timer_accuracy_test.c
    ${SOURCE_DIR}/threadx_timer_multiple_accuracy_test.c
    ${SOURCE_DIR}/threadx_timer_multiple_test.c
    ${SOURCE_DIR}/threadx_timer_simple_test.c
    ${SOURCE_DIR}/threadx_trace_basic_test.c
    ${SOURCE_DIR}/threadx_initialize_kernel_setup_test.c)

add_custom_command(
  OUTPUT ${SOURCE_DIR}/tx_initialize_low_level.c
  COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/generate_test_file.sh
  COMMENT "Generating tx_initialize_low_level.c for test")

add_library(test_utility ${SOURCE_DIR}/tx_initialize_low_level.c
                         ${SOURCE_DIR}/testcontrol.c)
target_link_libraries(test_utility PUBLIC azrtos::threadx)
target_compile_definitions(test_utility PUBLIC CTEST BATCH_TEST
                                               TEST_STACK_SIZE_PRINTF=4096)

foreach(test_case ${regression_test_cases})
  get_filename_component(test_name ${test_case} NAME_WE)
  add_executable(${test_name} ${test_case})
  target_link_libraries(${test_name} PRIVATE test_utility)
  add_test(${CMAKE_BUILD_TYPE}::${test_name} ${test_name})
endforeach()
