
set(CURRENT_DIR ${PROJECT_DIR}/ports_smp/linux/gnu)
target_sources(${PROJECT_NAME}
    PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
	${CURRENT_DIR}/src/tx_initialize_low_level.c
	${CURRENT_DIR}/src/tx_thread_context_restore.c
	${CURRENT_DIR}/src/tx_thread_context_save.c
	${CURRENT_DIR}/src/tx_thread_interrupt_control.c
	${CURRENT_DIR}/src/tx_thread_schedule.c
	${CURRENT_DIR}/src/tx_thread_smp_core_get.c
	${CURRENT_DIR}/src/tx_thread_smp_core_preempt.c
	${CURRENT_DIR}/src/tx_thread_smp_current_state_get.c
	${CURRENT_DIR}/src/tx_thread_smp_current_thread_get.c
	${CURRENT_DIR}/src/tx_thread_smp_initialize_wait.c
	${CURRENT_DIR}/src/tx_thread_smp_low_level_initialize.c
	${CURRENT_DIR}/src/tx_thread_smp_protect.c
	${CURRENT_DIR}/src/tx_thread_smp_time_get.c
	${CURRENT_DIR}/src/tx_thread_smp_unprotect.c
	${CURRENT_DIR}/src/tx_thread_stack_build.c
	${CURRENT_DIR}/src/tx_thread_system_return.c
	${CURRENT_DIR}/src/tx_timer_interrupt.c

    # {{END_TARGET_SOURCES}}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CURRENT_DIR}/inc
)

target_compile_definitions(${PROJECT_NAME} PUBLIC "-D_GNU_SOURCE -DTX_LINUX_DEBUG_ENABLE")