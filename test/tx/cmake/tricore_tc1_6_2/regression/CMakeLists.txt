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

project(tricore_tc1_6_2_regression_test LANGUAGES C)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../regression)
set(BSP_DIR ${CMAKE_CURRENT_LIST_DIR}/../bsp)
set(EXAMPLE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../../ports/tricore_tc1_6_2/gnu/example_build)

# testcontrol.c runs every test of its table test_control_tests[] in one
# image, so every test file is linked. threadx_initialize_kernel_setup_test.c
# provides its own main() and is not part of the table.
file(GLOB regression_test_cases ${SOURCE_DIR}/threadx_*.c)
list(REMOVE_ITEM regression_test_cases
    ${SOURCE_DIR}/threadx_initialize_kernel_setup_test.c)

# testcontrol.patch reduces the test memory to fit the RAM of the TC375.
set(TESTCONTROL_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/testcontrol.c)
add_custom_command(
    OUTPUT ${TESTCONTROL_SOURCE}
    COMMAND patch --quiet --output=${TESTCONTROL_SOURCE}
            ${SOURCE_DIR}/testcontrol.c
            ${CMAKE_CURRENT_LIST_DIR}/testcontrol.patch
    DEPENDS ${SOURCE_DIR}/testcontrol.c
            ${CMAKE_CURRENT_LIST_DIR}/testcontrol.patch
    VERBATIM)
# Cpu0_Main.c of the BSP starts the test control.
set_source_files_properties(${TESTCONTROL_SOURCE} PROPERTIES
    COMPILE_DEFINITIONS main=tx_regression_main)

set(WEAK_DEFAULTS_SOURCE ${CMAKE_CURRENT_LIST_DIR}/../../../../shared/regression/testcontrol_weak_defaults.c)

# The kernel and the port call into these utilities when the option is set.
set(UTILITY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../../utility)
set(utility_sources)
if("-DTX_EXECUTION_PROFILE_ENABLE" IN_LIST ${CMAKE_BUILD_TYPE})
    list(APPEND utility_sources ${UTILITY_DIR}/execution_profile_kit/tx_execution_profile.c)
endif()
if("-DTX_LOW_POWER" IN_LIST ${CMAKE_BUILD_TYPE})
    list(APPEND utility_sources ${UTILITY_DIR}/low_power/tx_low_power.c)
endif()

add_executable(regression
    ${TESTCONTROL_SOURCE}
    ${regression_test_cases}
    ${WEAK_DEFAULTS_SOURCE}
    ${utility_sources})
target_include_directories(regression PRIVATE ${SOURCE_DIR})
# The BSP overrides write() of libos, so the whole BSP library is linked.
target_link_libraries(regression PRIVATE
    -Wl,--whole-archive aurix_lite_kit_v2_bsp -Wl,--no-whole-archive
    azrtos::threadx)
target_link_options(regression PRIVATE
    "SHELL:-T ${EXAMPLE_DIR}/sample_threadx.ld"
    "SHELL:-T ${BSP_DIR}/regression.ld"
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/regression.map)
set_target_properties(regression PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET regression POST_BUILD
    COMMAND ${OBJCOPY} -O ihex
            $<TARGET_FILE:regression> ${CMAKE_CURRENT_BINARY_DIR}/regression.hex
    COMMAND ${SIZE} $<TARGET_FILE:regression>
    VERBATIM)
