# Portions of this file were generated with AI assistance.

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(windows_port_test LANGUAGES C)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../windows_port)

# Port-level tests for the Windows simulators. They link the library the regression
# suite builds, and fake.c supplies the test hooks that TX_REGRESSION_TEST compiles
# into it.
add_executable(tx_win_isr_handoff
               ${SOURCE_DIR}/threadx_windows_isr_handoff_test.c
               ${CMAKE_CURRENT_LIST_DIR}/../samples/fake.c)
target_link_libraries(tx_win_isr_handoff PRIVATE azrtos::threadx)
add_test(${CMAKE_BUILD_TYPE}::threadx_windows_isr_handoff_test tx_win_isr_handoff)

add_executable(tx_win_tick_rate
               ${SOURCE_DIR}/threadx_windows_tick_rate_test.c
               ${CMAKE_CURRENT_LIST_DIR}/../samples/fake.c)
target_link_libraries(tx_win_tick_rate PRIVATE azrtos::threadx)
add_test(${CMAKE_BUILD_TYPE}::threadx_windows_tick_rate_test tx_win_tick_rate)
