cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(core_v_mcu_tests C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CORE_V_MCU_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

add_executable(test_irq
    ${CMAKE_CURRENT_LIST_DIR}/test_irq.c
    ${CMAKE_CURRENT_LIST_DIR}/mock/mmio_mock.c
)
target_include_directories(test_irq PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mock
    ${CORE_V_MCU_DIR}/include
)

add_executable(test_timer
    ${CMAKE_CURRENT_LIST_DIR}/test_timer.c
    ${CMAKE_CURRENT_LIST_DIR}/mock/mmio_mock.c
)
target_include_directories(test_timer PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mock
    ${CORE_V_MCU_DIR}/include
)

enable_testing()
add_test(NAME irq_tests COMMAND test_irq)
add_test(NAME timer_tests COMMAND test_timer)
