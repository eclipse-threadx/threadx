set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../../..)
set(SOURCE_DIR ${REPO_ROOT}/test/tx/execution_profile)

add_executable(
  threadx_execution_profile_test
  ${SOURCE_DIR}/threadx_execution_profile_test.c
  ${REPO_ROOT}/utility/execution_profile_kit/tx_execution_profile.c)

target_include_directories(
  threadx_execution_profile_test
  PRIVATE ${SOURCE_DIR}
          ${REPO_ROOT}/common/inc
          ${REPO_ROOT}/ports/linux/gnu/inc
          ${REPO_ROOT}/utility/execution_profile_kit)

target_compile_definitions(threadx_execution_profile_test
                           PRIVATE TX_INCLUDE_USER_DEFINE_FILE)

target_compile_options(threadx_execution_profile_test PRIVATE -Wall -Wextra -Werror)

add_test(${CMAKE_BUILD_TYPE}::threadx_execution_profile_test threadx_execution_profile_test)
