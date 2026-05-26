cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

project(threadx_riscv_test LANGUAGES C ASM)

# Build configurations (same defines as Linux tests, minus coverage instrumentation)
set(BUILD_CONFIGURATIONS default_build disable_notify_callbacks_build
                         stack_checking_build stack_checking_rand_fill_build trace_build)
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

# Per-configuration compile definitions
set(default_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32)
set(disable_notify_callbacks_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING)
set(stack_checking_rand_fill_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_STACK_CHECKING -DTX_ENABLE_RANDOM_NUMBER_STACK_FILLING)
set(trace_build -DTX_QUEUE_MESSAGE_MAX_SIZE=32 -DTX_ENABLE_EVENT_TRACE)

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

# Add ThreadX library (uses the RISC-V toolchain set by CMAKE_TOOLCHAIN_FILE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../.. threadx)

# Add BSP
add_subdirectory(bsp)

# Add regression tests
add_subdirectory(regression)

# Strict warning flags for the ThreadX library build
target_compile_options(
    threadx
    PRIVATE -Werror
            -Wall
            -Wextra
            -pedantic
            -fmessage-length=0
            -fsigned-char
            -ffunction-sections
            -fdata-sections
            -Wunused
            -Wuninitialized
            -Wmissing-declarations
            # -Wconversion is disabled for RV64 because ULONG is intentionally 32-bit
            # (for ThreadX ABI compatibility across all ports) while sizeof(VOID*) and
            # sizeof(ALIGN_TYPE) return size_t (64-bit on RV64). This design matches
            # ARM Cortex-A72 and other 64-bit ports. The implicit conversions in
            # common/src/ byte/block pool code trigger -Wconversion warnings. Since
            # pool sizes are inherently limited to 32-bit (ULONG), the conversions
            # are safe. No other 64-bit port with 32-bit ULONG has been tested with
            # -Wconversion enabled, so we disable it here for RV64.
            $<$<STREQUAL:${THREADX_ARCH},risc-v32>:-Wconversion>
            -Wpointer-arith
            -Wlogical-op
            -Waggregate-return
            -Wfloat-equal
            $<$<COMPILE_LANGUAGE:C>:-include$<SEMICOLON>stdlib.h>
)
