
target_sources(${PROJECT_NAME}
    PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
	${CMAKE_CURRENT_LIST_DIR}/src/tx_clib_lock.c
	${CMAKE_CURRENT_LIST_DIR}/src/tx_initialize_low_level.c
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_restore.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_save.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_control.c
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_schedule.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_stack_build.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_system_return.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_timer_interrupt.S
	${CMAKE_CURRENT_LIST_DIR}/src/tx_xtensa_stack_error_handler.c
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_context.S
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_coproc_handler.S
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_init.c
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_intr_asm.S
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_intr.c
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_intr_wrapper.c
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_overlay_os_hook.c
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_vectors.S
	${CMAKE_CURRENT_LIST_DIR}/src/xtensa_vectors_xea3.S

    # {{END_TARGET_SOURCES}}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/src/xtensa_vectors_xea3.S
                            TARGET_DIRECTORY ${PROJECT_NAME}
                            PROPERTIES COMPILE_OPTIONS "-mtext-section-literals")

