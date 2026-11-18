# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

project(threadx_rh850_test LANGUAGES C ASM)

set(BUILD_CONFIGURATIONS default_build optimized_build disable_notify_callbacks_build
                         stack_checking_build stack_checking_rand_fill_build trace_build)
set(CMAKE_CONFIGURATION_TYPES
    ${BUILD_CONFIGURATIONS}
    CACHE STRING "list of supported configuration types" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                                             ${CMAKE_CONFIGURATION_TYPES})
list(GET CMAKE_CONFIGURATION_TYPES 0 BUILD_TYPE)
if((NOT CMAKE_BUILD_TYPE) OR (NOT ("${CMAKE_BUILD_TYPE}" IN_LIST
                                   CMAKE_CONFIGURATION_TYPES)))
    set(CMAKE_BUILD_TYPE
        "${BUILD_TYPE}"
        CACHE STRING "Build Type of the project" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.")
message(STATUS "THREADX_ARCH: ${THREADX_ARCH}")
message(STATUS "RH850_FLOAT: ${RH850_FLOAT}")

set(default_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32)
set(optimized_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32)
set(disable_notify_callbacks_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING)
set(stack_checking_rand_fill_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING -DTX_ENABLE_RANDOM_NUMBER_STACK_FILLING)
set(trace_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_EVENT_TRACE)

if(CMAKE_BUILD_TYPE STREQUAL "optimized_build")
    set(REGRESSION_OPTIMIZATION -O2)
else()
    set(REGRESSION_OPTIMIZATION -O0)
endif()

add_compile_options(
    -std=c99
    ${REGRESSION_OPTIMIZATION}
    -g3
    -fdiagnostics-color
    -Werror
    -ffunction-sections
    -fdata-sections
    -DTX_REGRESSION_TEST
    -DTEST_STACK_SIZE_PRINTF=4096
    -DEXTERNAL_EXIT
    -DTX_TIMER_TICKS_PER_SECOND=100UL
    ${${CMAKE_BUILD_TYPE}}
)

enable_testing()

set(TX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/bsp/tx_user.h)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../.. threadx)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../ports/rh850/gnu/example_build/qemu_rh850_u2c/board_sources.cmake)
add_subdirectory(bsp)
add_subdirectory(regression)

target_compile_options(
    threadx
    PRIVATE -Werror
            -Wall
            -Wextra
            -pedantic
            -fmessage-length=0
            -fsigned-char
            -ffunction-sections
            -fdata-sections
            -Wunused
            -Wuninitialized
            -Wmissing-declarations
            -Wconversion
            -Wpointer-arith
            -Wlogical-op
            -Waggregate-return
            -Wfloat-equal
            $<$<COMPILE_LANGUAGE:C>:-include$<SEMICOLON>stdlib.h>
)
