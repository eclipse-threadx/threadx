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

add_library(aurix_lite_kit_v2_bsp
    ${BSP_DIR}/main.c
    ${BSP_DIR}/tx_initialize_low_level.c
)
target_link_libraries(aurix_lite_kit_v2_bsp PUBLIC azrtos::threadx)
