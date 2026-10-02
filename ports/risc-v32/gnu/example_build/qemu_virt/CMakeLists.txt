set(QEMU_DEMO_DIR ${CMAKE_CURRENT_LIST_DIR})

add_executable(kernel.elf EXCLUDE_FROM_ALL
    ${QEMU_DEMO_DIR}/demo_threadx.c
    ${QEMU_DEMO_DIR}/entry.S
    ${QEMU_DEMO_DIR}/uart.c
    ${QEMU_DEMO_DIR}/plic.c
    ${QEMU_DEMO_DIR}/hwtimer.c
    ${QEMU_DEMO_DIR}/trap.c
    ${QEMU_DEMO_DIR}/board.c
    ${QEMU_DEMO_DIR}/tx_initialize_low_level.S
)

target_link_libraries(kernel.elf PRIVATE threadx gcc)

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
# The runner needs a GDB that understands riscv:rv32 remote targets
find_program(RISCV32_GDB NAMES riscv32-unknown-elf-gdb gdb-multiarch)
find_program(RISCV32_QEMU NAMES qemu-system-riscv32)
if(Python3_FOUND AND RISCV32_GDB AND RISCV32_QEMU)
    set(RISCV32_QEMU_TEST_ARGS)
    if(SOFT_FLOAT)
        list(APPEND RISCV32_QEMU_TEST_ARGS --skip-fpu)
    endif()
    add_custom_target(check-functional-riscv32
        COMMAND ${Python3_EXECUTABLE}
            ${QEMU_DEMO_DIR}/test/threadx_test_tx_gnu_riscv32_qemu.py
            --elf $<TARGET_FILE:kernel.elf>
            --qemu ${RISCV32_QEMU}
            --gdb ${RISCV32_GDB}
            ${RISCV32_QEMU_TEST_ARGS}
        DEPENDS kernel.elf
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running RISC-V32 QEMU/GDB functional test runner..."
    )
elseif(NOT Python3_FOUND)
    message(STATUS
        "Python3 not found; check-functional-riscv32 target unavailable.")
elseif(NOT RISCV32_GDB)
    message(STATUS
        "RISC-V GDB not found; check-functional-riscv32 target unavailable.")
else()
    message(STATUS
        "qemu-system-riscv32 not found; check-functional-riscv32 target unavailable.")
endif()
