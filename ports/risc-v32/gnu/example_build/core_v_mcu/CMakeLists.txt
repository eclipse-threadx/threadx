cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(demo_threadx_core_v_mcu LANGUAGES C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CORE_V_MCU_DIR ${CMAKE_CURRENT_LIST_DIR})
set(THREADX_ROOT   ${CMAKE_CURRENT_LIST_DIR}/../../../../..)
set(TX_USER_FILE   ${CORE_V_MCU_DIR}/include/tx_user.h)

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "gnu")

message(STATUS "Toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "C compiler:     ${CMAKE_C_COMPILER}")

add_subdirectory(${THREADX_ROOT} threadx)
target_include_directories(threadx PRIVATE ${CORE_V_MCU_DIR}/include)

set(SRCS
    ${CORE_V_MCU_DIR}/crt0.S
    ${CORE_V_MCU_DIR}/vectors.S
    ${CORE_V_MCU_DIR}/tx_initialize_low_level.S
    ${CORE_V_MCU_DIR}/bsp/irq.c
    ${CORE_V_MCU_DIR}/bsp/timer_irq.c
    ${CORE_V_MCU_DIR}/bsp/fll.c
    ${CORE_V_MCU_DIR}/bsp/gpio.c
    ${CORE_V_MCU_DIR}/bsp/uart_driver.c
    ${CORE_V_MCU_DIR}/bsp/i2c_master.c
    ${CORE_V_MCU_DIR}/bsp/temp_sensor.c
    ${CORE_V_MCU_DIR}/bsp/system_core_v_mcu.c
    ${CORE_V_MCU_DIR}/bsp/string.c
    ${CORE_V_MCU_DIR}/demo_threadx.c
)

add_executable(demo_threadx ${SRCS})
set_target_properties(demo_threadx PROPERTIES OUTPUT_NAME "demo_threadx.elf")

target_include_directories(demo_threadx PRIVATE
    ${CORE_V_MCU_DIR}/include
    ${THREADX_ROOT}/common/inc
    ${THREADX_ROOT}/ports/risc-v32/gnu/inc
)

target_link_libraries(demo_threadx PRIVATE threadx gcc)

target_link_options(demo_threadx PRIVATE
    -T${CORE_V_MCU_DIR}/link.ld
    -nostartfiles
    -nodefaultlibs
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/demo_threadx.map
)

target_compile_options(demo_threadx PRIVATE
    -Os
    -g
    -Wall
    -Wextra
    -ffreestanding
    -ffunction-sections
    -fdata-sections
    -DTX_INCLUDE_USER_DEFINE_FILE
)

add_custom_command(TARGET demo_threadx POST_BUILD
    COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:demo_threadx> ${CMAKE_CURRENT_BINARY_DIR}/demo_threadx.bin
    COMMENT "Generating demo_threadx.bin"
)
