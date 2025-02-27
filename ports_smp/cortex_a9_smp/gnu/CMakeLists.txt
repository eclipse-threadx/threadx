
target_sources(${PROJECT_NAME}
    PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_restore.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_save.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_control.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_disable.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_restore.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_irq_nesting_end.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_irq_nesting_start.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_schedule.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_core_get.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_core_preempt.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_current_state_get.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_current_thread_get.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_initialize_wait.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_low_level_initialize.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_protect.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_time_get.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_smp_unprotect.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_stack_build.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_system_return.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_vectored_context_save.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_timer_interrupt.S
    # {{END_TARGET_SOURCES}}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)
