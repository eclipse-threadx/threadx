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

# BSP library for the regression test on the TC375 Lite Kit, based on the
# example of the port: its low-level initialisation with the trap vector
# table and the timer interrupt, and main.c, which sends printf output to the
# UART. The start-up code is crt0-tc38xx.o of the compiler.

project(aurix_lite_kit_v2_bsp LANGUAGES C)

set(BSP_DIR ${CMAKE_CURRENT_LIST_DIR})
set(EXAMPLE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../../ports/tricore_tc1_6_2/gnu/example_build)

# The example's timer interrupt with the call of test_interrupt_dispatch().
set(LOW_LEVEL_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/tx_initialize_low_level.c)
add_custom_command(
    OUTPUT ${LOW_LEVEL_SOURCE}
    COMMAND patch --quiet --output=${LOW_LEVEL_SOURCE}
            ${EXAMPLE_DIR}/tx_initialize_low_level.c
            ${BSP_DIR}/tx_initialize_low_level.patch
    DEPENDS ${EXAMPLE_DIR}/tx_initialize_low_level.c
            ${BSP_DIR}/tx_initialize_low_level.patch
    VERBATIM)

add_library(aurix_lite_kit_v2_bsp
    ${BSP_DIR}/main.c
    ${LOW_LEVEL_SOURCE}
)
target_link_libraries(aurix_lite_kit_v2_bsp PUBLIC azrtos::threadx)
