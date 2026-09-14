cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

project(threadx_test LANGUAGES C)

# Set build configurations
set(BUILD_CONFIGURATIONS default_build_coverage disable_notify_callbacks_build
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
  -Werror
  -DTX_REGRESSION_TEST
  -DTEST_STACK_SIZE_PRINTF=4096
  ${${CMAKE_BUILD_TYPE}})
add_link_options(-m32)

enable_testing()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../.. threadx)
add_subdirectory(regression)
add_subdirectory(samples)

# Coverage
#
# The gate here used to be the build type alone, and only one of the five
# configurations -- default_build_coverage -- has a name matching "*_coverage".
# So four configurations built and ran every test and their coverage was thrown
# away. That is not redundancy discarded: each configuration selects a different
# set of TX_ feature macros, so the code the other four compile is *absent from
# the denominator* rather than reported as uncovered. A figure taken from one of
# them is the executable-line count of one configuration, not of the kernel.
#
# TX_COVERAGE instruments a build regardless of its name. It defaults to OFF and
# the build-type match is kept, so configuring a single configuration by hand
# behaves exactly as it did before; the regression run turns it on for all five
# and merges the results.
option(TX_COVERAGE "Instrument this build for coverage regardless of build type" OFF)
if(TX_COVERAGE OR CMAKE_BUILD_TYPE MATCHES ".*_coverage")
  target_compile_options(threadx PRIVATE -fprofile-arcs -ftest-coverage)
  target_link_options(threadx PRIVATE -fprofile-arcs -ftest-coverage)
endif()

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
          -Wconversion
          -Wpointer-arith
          # -Wshadow
          -Wlogical-op
          -Waggregate-return
          -Wfloat-equal)
