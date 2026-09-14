cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
cmake_policy(SET CMP0057 NEW)

project(module_manager_test LANGUAGES C)

set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../../..)
set(SOURCE_DIR ${REPO_ROOT}/test/tx/module_manager)

# The Module Manager is not part of the ThreadX library this test tree builds, and
# its control blocks come from a module port rather than a base port. These tests
# therefore compile the manager sources they exercise directly against one module
# port's headers, and stand stubs behind the kernel services those sources call, so
# that they run on the host like every other test here. It is the same technique the
# thread_transition directory uses, for the same reason: the code under test cannot
# be reached through the library the suite links.
#
# Every module port is an embedded architecture, so the interrupt primitives those
# sources use are inline assembly the host assembler cannot take. The port shim
# header is force-included ahead of every translation unit here and replaces the
# three of them with host equivalents that count.

set(module_manager_dir ${REPO_ROOT}/common_modules/module_manager)
set(cortex_a7_module_dir ${REPO_ROOT}/ports_module/cortex_a7/gnu)

add_executable(
  threadx_module_manager_thread_kernel_stack_test
  ${SOURCE_DIR}/threadx_module_manager_thread_kernel_stack_test.c
  ${module_manager_dir}/src/txm_module_manager_util.c
  ${module_manager_dir}/src/txm_module_manager_object_allocate.c
  ${module_manager_dir}/src/txm_module_manager_object_deallocate.c)

target_include_directories(
  threadx_module_manager_thread_kernel_stack_test
  PRIVATE ${SOURCE_DIR}
          ${REPO_ROOT}/common/inc
          ${REPO_ROOT}/common_modules/inc
          ${module_manager_dir}/inc
          ${cortex_a7_module_dir}/inc)

# The dispatch layer is one header of static functions, one per kernel service. A
# test reaches the dispatcher it exercises by including that header, which defines
# every other dispatcher too, and an unoptimised build emits them all -- so their
# references to services this harness stands behind nothing for would reach the
# linker.
#
# The header guards each dispatcher with its own TXM_<SERVICE>_CALL_NOT_USED macro,
# which is the supported way to trim the dispatch table. Defining every guard except
# the ones a test needs leaves that test's translation unit holding only the
# dispatcher under test. The list is read out of the header rather than written down
# here, so a service added later is excluded without anyone having to remember.
file(STRINGS ${module_manager_dir}/inc/txm_module_manager_dispatch.h
     dispatch_guard_lines REGEX "^#ifndef TXM_[A-Z0-9_]+_CALL_NOT_USED")

set(dispatch_guards "")
foreach(guard_line ${dispatch_guard_lines})
  string(REGEX MATCH "TXM_[A-Z0-9_]+_CALL_NOT_USED" guard "${guard_line}")
  list(APPEND dispatch_guards ${guard})
endforeach()
list(REMOVE_DUPLICATES dispatch_guards)

# The dispatchers this test calls, which therefore must stay in.
set(kernel_stack_dispatchers TXM_THREAD_DELETE_CALL_NOT_USED)

set(kernel_stack_guards ${dispatch_guards})
list(REMOVE_ITEM kernel_stack_guards ${kernel_stack_dispatchers})

target_compile_definitions(threadx_module_manager_thread_kernel_stack_test
                           PRIVATE ${kernel_stack_guards})

# The port shim is force-included ahead of every translation unit, the test included,
# because the manager sources and the dispatch header both reach for the port's
# interrupt primitives.
target_compile_options(
  threadx_module_manager_thread_kernel_stack_test
  PRIVATE -Wall
          -Wextra
          -include ${SOURCE_DIR}/threadx_module_manager_host_test_port.h)

add_test(${CMAKE_BUILD_TYPE}::threadx_module_manager_thread_kernel_stack_test
         threadx_module_manager_thread_kernel_stack_test)
