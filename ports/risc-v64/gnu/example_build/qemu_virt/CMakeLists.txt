set(QEMU_DEMO_DIR ${CMAKE_CURRENT_LIST_DIR})

add_executable(kernel.elf EXCLUDE_FROM_ALL
    ${QEMU_DEMO_DIR}/demo_threadx.c
    ${QEMU_DEMO_DIR}/entry.s
    ${QEMU_DEMO_DIR}/uart.c
    ${QEMU_DEMO_DIR}/plic.c
    ${QEMU_DEMO_DIR}/hwtimer.c
    ${QEMU_DEMO_DIR}/trap.c
    ${QEMU_DEMO_DIR}/board.c
    ${QEMU_DEMO_DIR}/tx_initialize_low_level.S
)

target_link_libraries(kernel.elf PRIVATE
    -Wl,--whole-archive
    threadx
    -Wl,--no-whole-archive
)

target_include_directories(kernel.elf PRIVATE
    ${CMAKE_SOURCE_DIR}/common/inc
    ${CMAKE_SOURCE_DIR}/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN}/inc
    ${QEMU_DEMO_DIR}
)

target_link_options(kernel.elf PRIVATE
    -T${QEMU_DEMO_DIR}/link.lds
    -nostartfiles
    -Wl,-Map=kernel.map
)

# QEMU/GDB functional test runner.  Optional: skipped silently if the
# host has no Python 3 interpreter on PATH.
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
    add_custom_target(check-functional-riscv64
        COMMAND ${Python3_EXECUTABLE}
            ${QEMU_DEMO_DIR}/test/azrtos_test_tx_gnu_riscv64_qemu.py
            --elf $<TARGET_FILE:kernel.elf>
            --qemu qemu-system-riscv64
            --gdb gdb
        DEPENDS kernel.elf
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running RISC-V64 QEMU/GDB functional test runner..."
    )
else()
    message(STATUS
        "Python3 not found; check-functional-riscv64 target unavailable.")
endif()
