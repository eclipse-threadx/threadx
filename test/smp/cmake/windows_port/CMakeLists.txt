# Portions of this file were generated with AI assistance.

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(windows_port_test LANGUAGES C)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../tx/windows_port)

# Port-level tests for the SMP Windows simulator, shared with the single-core tree.
add_executable(tx_win_tick_rate
               ${SOURCE_DIR}/threadx_windows_tick_rate_test.c
               ${CMAKE_CURRENT_LIST_DIR}/../samples/fake.c)
target_link_libraries(tx_win_tick_rate PRIVATE azrtos::threadx_smp)
add_test(${CMAKE_BUILD_TYPE}::threadx_windows_tick_rate_test tx_win_tick_rate)
