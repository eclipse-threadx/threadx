cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# BSP library for RISC-V QEMU virt regression tests.
# Provides board init, UART, CLINT timer, PLIC, trap handler,
# newlib syscall stubs, and low-level ThreadX initialization.

project(riscv_bsp LANGUAGES C ASM)

set(BSP_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(riscv_bsp
    ${BSP_DIR}/entry.S
    ${BSP_DIR}/tx_initialize_low_level.S
    ${BSP_DIR}/board.c
    ${BSP_DIR}/trap.c
    ${BSP_DIR}/hwtimer.c
    ${BSP_DIR}/plic.c
    ${BSP_DIR}/uart.c
    ${BSP_DIR}/syscalls.c
    ${BSP_DIR}/printf.c
)

target_include_directories(riscv_bsp PUBLIC ${BSP_DIR})
target_link_libraries(riscv_bsp PUBLIC azrtos::threadx)
target_compile_definitions(riscv_bsp PUBLIC EXTERNAL_EXIT)
