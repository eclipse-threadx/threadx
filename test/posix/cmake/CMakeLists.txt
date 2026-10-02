cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

# Regression tests for the POSIX compatibility layer.
#
# The layer cannot run on the hosted simulation ports: it supplies its own
# pthread.h, errno.h, time.h and friends, which collide with the C library's,
# and the linux port emulates threads with the very pthread calls the layer
# exports.  These tests therefore run on emulated targets, reusing the RISC-V
# BSP and QEMU setup that the ThreadX RISC-V regression suite already uses.

project(threadx_posix_test LANGUAGES C ASM)

set(BUILD_CONFIGURATIONS default_build disable_notify_callbacks_build
                         stack_checking_build trace_build)
set(CMAKE_CONFIGURATION_TYPES
    ${BUILD_CONFIGURATIONS}
    CACHE STRING "list of supported configuration types" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                                             ${CMAKE_CONFIGURATION_TYPES})
list(GET CMAKE_CONFIGURATION_TYPES 0 BUILD_TYPE)
if((NOT CMAKE_BUILD_TYPE) OR (NOT ("${CMAKE_BUILD_TYPE}" IN_LIST
                                   CMAKE_CONFIGURATION_TYPES)))
    set(CMAKE_BUILD_TYPE
        "${BUILD_TYPE}"
        CACHE STRING "Build Type of the project" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}.")
message(STATUS "THREADX_ARCH: ${THREADX_ARCH}")

set(default_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32)
set(disable_notify_callbacks_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING)
set(trace_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_EVENT_TRACE)

# These tests are 32-bit only for now, and the reason is a gap in the layer
# rather than a choice about coverage.
#
# The layer passes a message by putting the address of a private buffer into the
# queue.  ULONG is 32 bits on every ThreadX port, including the 64-bit ones, so
# on a 64-bit target that address only fits if TX_64_BIT is defined: the layer
# then splits it across two message words and sizes the message accordingly.
# Without it, px_mq_send.c truncates the pointer to 32 bits and px_mq_receive.c
# casts the truncated value back, which the compiler reports only as a warning.
#
# Defining TX_64_BIT is not a way out either: tx_api.h then uses the extension
# pointer macros, which need tx_thread_extension_ptr in the thread control
# block, and no 64-bit port outside ports_smp and ports/linux declares it.
# Closing that gap belongs with the layer, not with its tests.
if(THREADX_ARCH STREQUAL "risc-v64")
    message(FATAL_ERROR
        "The POSIX layer cannot run on a 64-bit port yet: it needs TX_64_BIT to "
        "carry a 64-bit message pointer, and TX_64_BIT needs a port that declares "
        "tx_thread_extension_ptr. Use the riscv32 toolchain file for these tests.")
endif()

add_compile_options(
    -std=c99
    -O0
    -g3
    -fdiagnostics-color
    -Werror
    -ffunction-sections
    -fdata-sections
    -DTX_REGRESSION_TEST
    -DTEST_STACK_SIZE_PRINTF=4096
    -DEXTERNAL_EXIT
    ${${CMAKE_BUILD_TYPE}}
)

enable_testing()

# ThreadX itself, plus the posix-threadx target the top-level file defines.
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../.. threadx)

# Reuse the RISC-V BSP from the ThreadX regression suite rather than keeping a
# second copy of the board support in this tree.
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../tx/cmake/riscv/bsp riscv_bsp)

add_subdirectory(regression)
