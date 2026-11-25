cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(demo_threadx
    LANGUAGES C ASM
)

set(SRCS
    ${CMAKE_CURRENT_LIST_DIR}/demo_threadx.c
    ${CMAKE_CURRENT_LIST_DIR}/pre_main.c
    ${CMAKE_CURRENT_LIST_DIR}/boards/board_riscv_dummy/src/uart/board_uart.c
    ${CMAKE_CURRENT_LIST_DIR}/boards/board_riscv_dummy/src/board_init.c
    ${CMAKE_CURRENT_LIST_DIR}/components/csi/csi2/src/csi_misc.c
    ${CMAKE_CURRENT_LIST_DIR}/components/libc_threadx/mini_printf.c
    ${CMAKE_CURRENT_LIST_DIR}/components/libc_threadx/newlib_stub.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/arch/e906fdp/startup.S
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/arch/e906fdp/vectors.S
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/arch/e906fdp/system.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/arch/e906fdp/trap_c.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/drivers/dw_uart_ll.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/drivers/uart.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/devices.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/feature.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/irq_port.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/irq.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/pre_main.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/sys_clk.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/target_get.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/tick.c
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/src/sys/weak.c
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../../../../../common/inc
    ${CMAKE_CURRENT_LIST_DIR}/../../inc
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/boards/board_riscv_dummy/include
    ${CMAKE_CURRENT_LIST_DIR}/components/csi/csi2/include
    ${CMAKE_CURRENT_LIST_DIR}/components/libc_threadx/include
    ${CMAKE_CURRENT_LIST_DIR}/components/libc_threadx/compilers/gcc
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/include
    ${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/include/asm
)

add_compile_options(
    -Os -g -Wno-main
    -Wpointer-arith -Wno-undef -ffunction-sections -fdata-sections -fno-inline-functions -fno-builtin -fno-strict-aliasing
    -DCONFIG_SUPPORT_TSPEND=1
    -DCONFIG_ARCH_MAINSTACK=4096
    -DCONFIG_ARCH_INTERRUPTSTACK=4096
    -DCONFIG_SUPPORT_IRQ_NESTED=1
    -DCONFIG_INTC_CLIC=1
    -DCONFIG_INTC_CLINT=1
    -DCONFIG_XIP=1
    -DCONFIG_LIBC_MINI_PRINTF_SUPPORT=1
    -DCONFIG_SYSTICK_HZ=100
    -DCONFIG_BOARD_SMARTL_EVB=1
    -DCONFIG_CPU_XUANTIE_E906FDP=1
    -DCONFIG_KERNEL_THREADX=1
    -DTX_INCLUDE_USER_DEFINE_FILE
)

add_link_options(
    -nostartfiles -Wl,--gc-sections
    -T${CMAKE_CURRENT_LIST_DIR}/components/chip_riscv_dummy/gcc_flash_smartl.ld
    -Wl,-zmax-page-size=1024
    -Wl,-Map=${PROJECT_NAME}.map
)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../build)
add_executable(${PROJECT_NAME} ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
target_link_libraries(${PROJECT_NAME} PRIVATE threadx)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${OBJDUMP} -d $<TARGET_FILE:${PROJECT_NAME}> > ${PROJECT_NAME}.asm
    COMMENT "Generating ASM disassembly files"
)