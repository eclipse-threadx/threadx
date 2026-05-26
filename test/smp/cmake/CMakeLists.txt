cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

if((DEFINED THREADX_ARCH) AND (THREADX_ARCH STREQUAL "win64"))
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

project(threadx_smp_test LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set build configurations
set(BUILD_CONFIGURATIONS default_build_coverage disable_notify_callbacks_build
                         stack_checking_build stack_checking_rand_fill_build
                         trace_build)
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
set(default_build_coverage TX_QUEUE_MESSAGE_MAX_SIZE=32)
set(disable_notify_callbacks_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_ENABLE_STACK_CHECKING)
set(stack_checking_rand_fill_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_ENABLE_STACK_CHECKING TX_ENABLE_RANDOM_NUMBER_STACK_FILLING)
set(trace_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_ENABLE_EVENT_TRACE)

add_compile_definitions(
  TX_THREAD_SMP_ONLY_CORE_0_DEFAULT
  TX_SMP_NOT_POSSIBLE
  TX_REGRESSION_TEST
  TEST_STACK_SIZE_PRINTF=4096
  ${${CMAKE_BUILD_TYPE}})

if(MSVC)
  add_compile_options(/W3 /Zi)
  add_link_options(/DEBUG /INCREMENTAL:NO)
else()
  add_compile_options(
    -m32
    -ggdb
    -g3
    -gdwarf-2
    -fdiagnostics-color)
  add_link_options(-m32)
endif()

enable_testing()

add_subdirectory(threadx_smp)
add_subdirectory(regression)
add_subdirectory(samples)

# Coverage
if(CMAKE_BUILD_TYPE MATCHES ".*_coverage")
  if(NOT MSVC)
    target_compile_options(threadx_smp PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_options(threadx_smp PRIVATE -fprofile-arcs -ftest-coverage)
  endif()
endif()

if(MSVC)
  target_compile_options(threadx_smp PRIVATE /W3)
else()
  target_compile_options(
    threadx_smp
    PRIVATE
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
            -Wconversion
            -Wpointer-arith
            -Wlogical-op
            -Waggregate-return
            -Wfloat-equal)
endif()
