# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

target_sources(${PROJECT_NAME}
    PRIVATE
    # {{BEGIN_TARGET_SOURCES}}
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_initialize_low_level.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_restore.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_context_save.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_interrupt_control.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_schedule.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_stack_build.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_thread_system_return.S
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_timer_interrupt.c
    ${CMAKE_CURRENT_LIST_DIR}/src/tx_port_offset_check.c
    # {{END_TARGET_SOURCES}}
)

target_include_directories(${PROJECT_NAME}
    SYSTEM
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

if(RH850_FLOAT STREQUAL "soft")
    target_compile_definitions(${PROJECT_NAME} PUBLIC RH850_FLOAT_HARD=0)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC RH850_FLOAT_HARD=1)
endif()

option(TX_RH850_LOW_POWER "Build with tickless idle (TX_LOW_POWER)" OFF)
if(TX_RH850_LOW_POWER)
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../../../utility/low_power/tx_low_power.c)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../../../utility/low_power)
    target_compile_definitions(${PROJECT_NAME} PUBLIC TX_LOW_POWER)
endif()

option(TX_RH850_BUILD_QEMU_EXAMPLE "Build the QEMU rh850-u2c example" OFF)
if(TX_RH850_BUILD_QEMU_EXAMPLE)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/example_build/qemu_rh850_u2c)
endif()
