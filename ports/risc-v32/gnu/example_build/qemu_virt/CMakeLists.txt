# ThreadX RISC-V32 GNU port: QEMU virt demo build.
#
# This subdirectory is opted-in by ports/risc-v32/gnu/CMakeLists.txt when
# the consumer is building the threadx library for the risc-v32/gnu port.
#
# kernel.elf is marked EXCLUDE_FROM_ALL so that:
#   - The default `cmake --build .` (which builds the threadx library
#     and any tests the consumer wires up) is unaffected by the demo.
#   - Users who explicitly want the demo can `cmake --build .
#     --target kernel.elf` or `cmake --build . --target
#     check-functional-riscv32` to also drive the QEMU/GDB test runner.
#
# This avoids touching the root CMakeLists.txt while still keeping the
# build-system entry point for the demo discoverable next to the demo
# sources themselves.

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

target_link_libraries(kernel.elf PRIVATE threadx)

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
    add_custom_target(check-functional-riscv32
        COMMAND ${Python3_EXECUTABLE}
            ${QEMU_DEMO_DIR}/test/azrtos_test_tx_gnu_riscv32_qemu.py
            --elf $<TARGET_FILE:kernel.elf>
            --qemu qemu-system-riscv32
            --gdb gdb
        DEPENDS kernel.elf
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running RISC-V32 QEMU/GDB functional test runner..."
    )
else()
    message(STATUS
        "Python3 not found; check-functional-riscv32 target unavailable.")
endif()
