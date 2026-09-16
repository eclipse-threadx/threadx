cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0057 NEW)

project(threadx_test LANGUAGES C)

# Set build configurations
set(BUILD_CONFIGURATIONS default_build_coverage disable_notify_callbacks_build
                         stack_checking_build stack_checking_rand_fill_build trace_build
                         misra_build misra_trace_build)
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

# TX_MISRA_ENABLE routes every pointer conversion in the kernel through the
# shim functions in common/src/tx_misra.c. None of the configurations above
# defines it, so that file's whole body is compiled out of all of them and the
# shim -- certified source -- carries no coverage at all. It is also the source
# the MISRA analysis is run against, so without this configuration the build
# that is analysed and the build that is covered are different builds of the
# same tree.
set(misra_build -DTX_MISRA_ENABLE)

# TX_MISRA_ENABLE and TX_ENABLE_EVENT_TRACE together reach a region of
# common/src/tx_misra.c that no other configuration compiles: the MISRA form of
# the in-line trace insert and the five pointer conversions the trace sources
# use. misra_build defines the first macro alone and trace_build the second
# alone, so that region was absent from the coverage denominator rather than
# uncovered in it -- the same reason misra_build itself exists.
#
# It needs TX_SOURCE_CODE as well, which is applied to the kernel target further
# down rather than here -- see the comment there for why.
set(misra_trace_build -DTX_MISRA_ENABLE -DTX_ENABLE_EVENT_TRACE)

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

# tx_misra.c is the only one of the 185 files in common/src that does not define
# TX_SOURCE_CODE for itself, and the prototypes for the five trace conversions it
# defines sit behind that macro in tx_trace.h. Without it the file defines five
# functions with no previous declaration, -Wmissing-declarations -Werror rejects
# it, and misra_trace_build does not build at all. The empty value matches the
# plain "#define TX_SOURCE_CODE" the other 184 files carry, which is why adding
# it changes no object code in any configuration.
#
# It goes on the kernel target rather than in add_compile_options above, because
# TX_SOURCE_CODE is what tells tx_api.h it is compiling ThreadX rather than an
# application: it suppresses the public tx_* API mappings, which the tests and the
# sample need. Applied globally it takes the API away from every caller.
#
# The defect itself is in the header and is reported upstream. This keeps our
# build independent of when it is fixed.
if(CMAKE_BUILD_TYPE STREQUAL "misra_trace_build")
  target_compile_definitions(threadx PRIVATE TX_SOURCE_CODE=)
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
