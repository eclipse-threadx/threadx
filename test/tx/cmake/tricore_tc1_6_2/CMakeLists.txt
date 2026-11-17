##############################################################################
# Copyright (c) 2026 safecubed GmbH
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
##############################################################################

# Portions of this file were generated with AI assistance.

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

# ThreadX regression test for the TriCore TC1.6.2 port on the TC375 Lite Kit.
# All tests run in one image, regression.elf, which a debugger programs into
# the board. The test output goes to the UART of the board. See README.md.

project(threadx_tricore_tc1_6_2_test LANGUAGES C)

# Set build configurations
set(BUILD_CONFIGURATIONS default_build disable_notify_callbacks_build
                         stack_checking_build stack_checking_rand_fill_build
                         trace_build misra_build misra_trace_build
                         disable_inline_build port_use_ccpn_build
                         timer_process_in_isr_build not_interruptable_build
                         execution_profile_build low_power_build)
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

# The configurations of test/tx/cmake/CMakeLists.txt.
set(default_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32)
set(disable_notify_callbacks_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING)
set(stack_checking_rand_fill_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING -DTX_ENABLE_RANDOM_NUMBER_STACK_FILLING)
set(trace_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_EVENT_TRACE)
set(misra_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_MISRA_ENABLE)
set(misra_trace_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_MISRA_ENABLE -DTX_ENABLE_EVENT_TRACE)

# Options with code of their own in the port.
set(disable_inline_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_DISABLE_INLINE)
set(port_use_ccpn_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_PORT_USE_CCPN)
set(timer_process_in_isr_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_TIMER_PROCESS_IN_ISR)
set(not_interruptable_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_NOT_INTERRUPTABLE)
set(execution_profile_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_EXECUTION_PROFILE_ENABLE)
set(low_power_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_LOW_POWER)

add_compile_options(
    -O0
    -g3
    -fdiagnostics-color
    -DTX_REGRESSION_TEST
    -DTEST_STACK_SIZE_PRINTF=4096
    -DEXTERNAL_EXIT
    ${${CMAKE_BUILD_TYPE}}
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../.. threadx)
add_subdirectory(bsp)
add_subdirectory(regression)

target_compile_options(
    threadx
    PRIVATE -Werror
            -Wall
            -Wextra
            -fmessage-length=0
)

# tx_api.h includes tx_execution_profile.h when the option is set.
if("-DTX_EXECUTION_PROFILE_ENABLE" IN_LIST ${CMAKE_BUILD_TYPE})
    target_include_directories(threadx PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../../../../utility/execution_profile_kit)
endif()

# With TX_NOT_INTERRUPTABLE, the cleanup functions of the common sources do
# not use their suspension_sequence parameter.
if("-DTX_NOT_INTERRUPTABLE" IN_LIST ${CMAKE_BUILD_TYPE})
    target_compile_options(threadx PRIVATE -Wno-unused-parameter)
endif()
