cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

project(threadx_smp_test LANGUAGES C)

# Set build configurations
set(BUILD_CONFIGURATIONS default_build_coverage
                         disable_notify_callbacks_build stack_checking_build stack_checking_rand_fill_build
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
set(default_build_coverage "")
set(disable_notify_callbacks_build -DTX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build -DTX_ENABLE_STACK_CHECKING)
set(stack_checking_rand_fill_build -DTX_ENABLE_STACK_CHECKING -DTX_ENABLE_RANDOM_NUMBER_STACK_FILLING)
set(trace_build -DTX_ENABLE_EVENT_TRACE)

add_compile_options(
  -m32
  -std=c99
  -ggdb
  -g3
  -gdwarf-2
  -fdiagnostics-color
  # -Werror
  -DTX_THREAD_SMP_ONLY_CORE_0_DEFAULT
  -DTX_SMP_NOT_POSSIBLE
  -DTX_REGRESSION_TEST
  -DTEST_STACK_SIZE_PRINTF=4096
  ${${CMAKE_BUILD_TYPE}})
add_link_options(-m32)

enable_testing()

add_subdirectory(threadx_smp)
add_subdirectory(regression)
add_subdirectory(samples)

# Coverage
if(CMAKE_BUILD_TYPE MATCHES ".*_coverage")
  target_compile_options(threadx_smp PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(threadx_smp PRIVATE -fprofile-arcs -ftest-coverage)
endif()

target_compile_options(
  threadx_smp
  PRIVATE # -Werror
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
          # -Wshadow
          -Wlogical-op
          -Waggregate-return
          -Wfloat-equal)
