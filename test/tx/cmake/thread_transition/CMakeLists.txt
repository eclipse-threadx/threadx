cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0057 NEW)

project(thread_transition_test LANGUAGES C)

set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../../..)
set(SOURCE_DIR ${REPO_ROOT}/test/tx/thread_transition)

# TX_NOT_INTERRUPTABLE and TX_DISABLE_ERROR_CHECKING are not among the five build
# configurations this tree compiles, and the tree builds the whole ThreadX library
# once per configuration, so neither can be reached from inside the regression
# suite. Both matter to the completion and termination transitions:
#
#   - TX_NOT_INTERRUPTABLE selects a different transition, in which the exit
#     notification callback runs with interrupts disabled and the interruptable
#     suspension path -- which is what sets and clears the transition marker in the
#     configurations the tree does build -- is not used at all. The marker is set
#     and cleared there by stores that exist only in that configuration, and a
#     marker left set would make a normally completed thread permanently
#     undeletable.
#
#   - TX_DISABLE_ERROR_CHECKING removes the _txe_ wrappers, so the public names bind
#     straight to the core services. The refusal lives in the core services for that
#     reason, and the way to show it is to call the public names in a build with no
#     wrappers.
#
# So the four sources that change are compiled directly into a test executable, once
# per combination, with recorders standing behind the scheduler services they call.
# That is the same technique the module manager tests in this tree use, for the same
# reason: the code under test cannot be reached through the library the suite links.
#
# These executables are deliberately left out of the coverage instrumentation. The
# same source compiled under different feature macros has a different line set, and
# merging those into the union the coverage report takes would confuse the figure
# rather than add to it. The coverage figure belongs to the five configurations the
# tree builds, and every line this change adds outside a TX_NOT_INTERRUPTABLE guard
# is in them.

set(transition_kernel_sources
    ${REPO_ROOT}/common/src/tx_thread_shell_entry.c
    ${REPO_ROOT}/common/src/tx_thread_terminate.c
    ${REPO_ROOT}/common/src/tx_thread_delete.c
    ${REPO_ROOT}/common/src/tx_thread_reset.c
    ${REPO_ROOT}/common/src/txe_thread_delete.c
    ${REPO_ROOT}/common/src/txe_thread_reset.c
    ${REPO_ROOT}/common/src/txe_thread_terminate.c
    ${REPO_ROOT}/common/src/tx_thread_initialize.c)

set(transition_sources
    ${SOURCE_DIR}/threadx_thread_transition_configuration_test.c
    ${transition_kernel_sources})

# The shim goes on the kernel sources only, not on the test. It defines
# TX_SOURCE_CODE, which is what tells tx_api.h to leave the public service names
# unmapped so that internal sources can use the core names; the test needs the
# opposite, because calling the public names and letting tx_api.h decide what they
# bind to is exactly what the TX_DISABLE_ERROR_CHECKING configuration is here to
# demonstrate.
set_source_files_properties(
  ${transition_kernel_sources}
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  PROPERTIES COMPILE_OPTIONS
             "-include;${SOURCE_DIR}/threadx_thread_transition_host_test_port.h")

# Each entry is a test-name suffix, a colon, and the feature macros that define the
# configuration, separated by "|". A semicolon cannot be used as that separator: it
# is CMake's own list separator, so the foreach below would iterate the macros
# instead of the configurations.
set(transition_configurations
    "ni:TX_NOT_INTERRUPTABLE"
    "ni_no_error_checking:TX_NOT_INTERRUPTABLE|TX_DISABLE_ERROR_CHECKING"
    "ni_no_notify:TX_NOT_INTERRUPTABLE|TX_DISABLE_NOTIFY_CALLBACKS"
    "no_error_checking:TX_DISABLE_ERROR_CHECKING")

foreach(configuration ${transition_configurations})

  string(REPLACE ":" ";" configuration_parts ${configuration})
  list(GET configuration_parts 0 configuration_name)
  list(GET configuration_parts 1 configuration_macro_text)
  string(REPLACE "|" ";" configuration_macros ${configuration_macro_text})

  set(test_name threadx_thread_transition_${configuration_name}_test)

  add_executable(${test_name} ${transition_sources})

  target_include_directories(
    ${test_name}
    PRIVATE ${SOURCE_DIR}
            ${REPO_ROOT}/common/inc
            ${REPO_ROOT}/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN}/inc)

  target_compile_definitions(${test_name} PRIVATE ${configuration_macros})

  # This directory is configured once per build configuration of the tree, so
  # these executables inherit whichever feature macros that configuration sets --
  # which is useful, since it means the two configurations above are also seen in
  # combination with stack checking and with the other four. The one that cannot
  # be inherited is event tracing: the trace macros in these sources reference the
  # trace component's buffer and registry, and this harness links four kernel
  # sources rather than the library, so the references would not resolve. The
  # trace insertions in these same sources are covered by the tree's own
  # trace_build configuration, where the whole library is linked, so nothing is
  # lost by turning tracing off here. Event logging is turned off for the same
  # reason; no configuration of the tree enables it.
  #
  # The -U flags have to reach the test source as well as the kernel sources,
  # because TX_ENABLE_EVENT_TRACE is visible to tx_api.h and the two must agree on
  # what the headers declare. Target compile options land after the directory's
  # -D flags on the command line, which is what makes the -U effective.
  target_compile_options(${test_name} PRIVATE -UTX_ENABLE_EVENT_TRACE
                                              -UTX_ENABLE_EVENT_LOG)

  add_test(${CMAKE_BUILD_TYPE}::${test_name} ${test_name})

endforeach()
