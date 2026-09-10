cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

if((DEFINED THREADX_ARCH) AND ((THREADX_ARCH STREQUAL "win32") OR (THREADX_ARCH STREQUAL "win64")))
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

project(threadx_test LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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
set(default_build_coverage TX_QUEUE_MESSAGE_MAX_SIZE=32)
set(disable_notify_callbacks_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_DISABLE_NOTIFY_CALLBACKS)
set(stack_checking_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_ENABLE_STACK_CHECKING)
set(stack_checking_rand_fill_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_ENABLE_STACK_CHECKING TX_ENABLE_RANDOM_NUMBER_STACK_FILLING)
set(trace_build TX_QUEUE_MESSAGE_MAX_SIZE=32 TX_ENABLE_EVENT_TRACE)

add_compile_definitions(
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
    -fdiagnostics-color
    -Werror)
  add_link_options(-m32)
endif()

enable_testing()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../.. threadx)
add_subdirectory(regression)
add_subdirectory(samples)

# The two configurations this change has to reach, TX_NOT_INTERRUPTABLE and
# TX_DISABLE_ERROR_CHECKING, are not among the five above and cannot be reached
# from the regression suite, which links one library per configuration. This
# directory compiles the sources concerned directly. It is listed last rather than
# beside regression on purpose: every branch in the current fix set adds an
# add_subdirectory line immediately after regression, and putting a second one
# there turns a set of clean merges into a set of one-line conflicts.
add_subdirectory(thread_transition)

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
  if(NOT MSVC)
    target_compile_options(threadx PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_options(threadx PRIVATE -fprofile-arcs -ftest-coverage)
  endif()
endif()

if(MSVC)
  target_compile_options(threadx PRIVATE /W3)
else()
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
endif()
