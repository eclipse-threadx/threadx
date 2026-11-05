# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(rh850_bsp LANGUAGES C ASM)

set(BSP_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(rh850_bsp
    ${RH850_U2C_SOURCES}
    ${BSP_DIR}/rh850_regression.c
)

target_include_directories(rh850_bsp PUBLIC ${BSP_DIR} ${RH850_U2C_DIR})
target_compile_definitions(rh850_bsp PRIVATE ${RH850_U2C_DEFINITIONS})
target_link_libraries(rh850_bsp PUBLIC azrtos::threadx)
target_compile_definitions(rh850_bsp PUBLIC EXTERNAL_EXIT)
