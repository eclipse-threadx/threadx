cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0057 NEW)

project(posix_regression_test LANGUAGES C ASM)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../regression)
set(TX_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../tx/regression)
set(BSP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../tx/cmake/riscv/bsp)

if(THREADX_ARCH STREQUAL "risc-v64")
    set(QEMU_SYSTEM "qemu-system-riscv64")
    set(XLEN_TAG "rv64")
else()
    set(QEMU_SYSTEM "qemu-system-riscv32")
    set(XLEN_TAG "rv32")
endif()

set(posix_test_cases
    ${SOURCE_DIR}/posix_mq_basic_test.c
    ${SOURCE_DIR}/posix_mq_send_abort_test.c
    ${SOURCE_DIR}/posix_pthread_basic_test.c
    ${SOURCE_DIR}/posix_pthread_self_test.c
)

set(WEAK_DEFAULTS_SOURCE ${CMAKE_CURRENT_LIST_DIR}/../../../shared/regression/testcontrol_weak_defaults.c)

# Weak defaults must be an OBJECT library so all objects are always linked into
# each test executable.  A STATIC library would have the linker skip them, since
# weak definitions do not satisfy undefined strong references while scanning an
# archive.
add_library(posix_test_weak_defaults OBJECT ${WEAK_DEFAULTS_SOURCE})
target_compile_definitions(posix_test_weak_defaults PRIVATE
    CTEST BATCH_TEST EXTERNAL_EXIT TEST_STACK_SIZE_PRINTF=4096)
target_compile_options(posix_test_weak_defaults PRIVATE -include stdlib.h)
target_link_libraries(posix_test_weak_defaults PRIVATE azrtos::threadx)

# testcontrol.c supplies main(), tx_application_define() and
# test_control_return(), and is shared with the ThreadX regression suite.
add_library(posix_test_utility ${TX_TEST_DIR}/testcontrol.c)
target_link_libraries(posix_test_utility PUBLIC riscv_bsp azrtos::threadx)
target_compile_definitions(posix_test_utility PUBLIC
    CTEST BATCH_TEST EXTERNAL_EXIT TEST_STACK_SIZE_PRINTF=4096)
target_compile_options(posix_test_utility PUBLIC -include stdlib.h)

function(register_posix_test test_name)
    target_link_options(${test_name} PRIVATE
        -T ${BSP_DIR}/link.lds -nostartfiles
        -Wl,--gc-sections)
    add_test(
        NAME ${CMAKE_BUILD_TYPE}::${XLEN_TAG}::${test_name}
        COMMAND ${QEMU_SYSTEM} -nographic -smp 1 -bios none -m 128M
                -machine virt -kernel $<TARGET_FILE:${test_name}>
    )
    set_tests_properties(${CMAKE_BUILD_TYPE}::${XLEN_TAG}::${test_name}
        PROPERTIES TIMEOUT 120)
endfunction()

foreach(test_case ${posix_test_cases})
    get_filename_component(test_name ${test_case} NAME_WE)
    add_executable(${test_name} ${test_case} $<TARGET_OBJECTS:posix_test_weak_defaults>)
    target_link_libraries(${test_name} PRIVATE
        -Wl,--whole-archive riscv_bsp -Wl,--no-whole-archive
        posix_test_utility posix-threadx)
    register_posix_test(${test_name})
endforeach()
