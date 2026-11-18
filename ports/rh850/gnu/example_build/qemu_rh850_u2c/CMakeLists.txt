# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

include(${CMAKE_CURRENT_LIST_DIR}/board_sources.cmake)
set(U2C_THREADX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../..)

add_executable(demo_threadx.elf
    ${RH850_U2C_SOURCES}
    ${RH850_U2C_DIR}/libc_locks.c
    ${RH850_U2C_DIR}/test_common.c
    ${RH850_U2C_DIR}/demo_monitor.c
    ${U2C_THREADX_DIR}/samples/demo_threadx.c
)

set_source_files_properties(${U2C_THREADX_DIR}/samples/demo_threadx.c PROPERTIES
    COMPILE_DEFINITIONS "main=demo_main;tx_application_define=demo_application_define"
    COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-return-type")

target_include_directories(demo_threadx.elf PRIVATE ${RH850_U2C_DIR})
target_compile_definitions(demo_threadx.elf PRIVATE ${RH850_U2C_DEFINITIONS})
target_compile_options(demo_threadx.elf PRIVATE
    -Wall -Wextra -Werror -ffunction-sections -fdata-sections -fno-common)
target_link_libraries(demo_threadx.elf PRIVATE threadx)
target_link_options(demo_threadx.elf PRIVATE ${RH850_U2C_LINK_OPTIONS}
    -Wl,-Map=demo_threadx.map)

find_program(RH850_QEMU NAMES qemu-system-rh850)
if(RH850_QEMU)
    add_test(NAME rh850::demo_threadx
        COMMAND ${RH850_QEMU} ${RH850_U2C_QEMU_ARGS} -kernel $<TARGET_FILE:demo_threadx.elf>)
    set_tests_properties(rh850::demo_threadx PROPERTIES
        TIMEOUT 60 PASS_REGULAR_EXPRESSION "demo_threadx pass"
        FAIL_REGULAR_EXPRESSION "FAIL")
else()
    message(STATUS "qemu-system-rh850 not found; the rh850::demo_threadx test is not added")
endif()
