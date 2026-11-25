# threadx_riscv_port.cmake
#
# Helper function shared by the three RISC-V port CMakeLists files
# (risc-v32/gnu, risc-v32/clang, risc-v64/gnu).
#
# Usage:
#   include(cmake/threadx_riscv_port.cmake)
#   threadx_add_riscv_port(SRC_DIR <path-to-src>
#                          INC_DIR <path-to-inc>
#                          [EXAMPLE_DIR <path-to-example-build>])
#
# SRC_DIR   — directory containing the 8 ThreadX .S port files.
# INC_DIR   — directory containing tx_port.h (added as PUBLIC include).
# EXAMPLE_DIR — optional: if provided and contains a CMakeLists.txt,
#               add_subdirectory() is called on it.

function(threadx_add_riscv_port)
    cmake_parse_arguments(RISCV "" "SRC_DIR;INC_DIR;EXAMPLE_DIR" "" ${ARGN})

    if(NOT RISCV_SRC_DIR)
        message(FATAL_ERROR "threadx_add_riscv_port: SRC_DIR is required")
    endif()
    if(NOT RISCV_INC_DIR)
        message(FATAL_ERROR "threadx_add_riscv_port: INC_DIR is required")
    endif()

    target_sources(${PROJECT_NAME}
        PRIVATE
        # {{BEGIN_TARGET_SOURCES}}
        ${RISCV_SRC_DIR}/tx_initialize_low_level.S
        ${RISCV_SRC_DIR}/tx_thread_context_restore.S
        ${RISCV_SRC_DIR}/tx_thread_context_save.S
        ${RISCV_SRC_DIR}/tx_thread_interrupt_control.S
        ${RISCV_SRC_DIR}/tx_thread_schedule.S
        ${RISCV_SRC_DIR}/tx_thread_stack_build.S
        ${RISCV_SRC_DIR}/tx_thread_system_return.S
        ${RISCV_SRC_DIR}/tx_timer_interrupt.S
        # {{END_TARGET_SOURCES}}
    )

    target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${RISCV_INC_DIR}
    )

    if(RISCV_EXAMPLE_DIR AND
       EXISTS ${RISCV_EXAMPLE_DIR}/CMakeLists.txt)
        add_subdirectory(${RISCV_EXAMPLE_DIR})
    endif()
endfunction()
