set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../../..)
set(SOURCE_DIR ${REPO_ROOT}/test/tx/low_power)

add_executable(
  threadx_low_power_test
  ${SOURCE_DIR}/threadx_low_power_test.c
  ${REPO_ROOT}/utility/low_power/tx_low_power.c)

target_include_directories(
  threadx_low_power_test
  PRIVATE ${SOURCE_DIR}
          ${REPO_ROOT}/common/inc
          ${REPO_ROOT}/ports/linux/gnu/inc
          ${REPO_ROOT}/utility/low_power)

target_compile_definitions(threadx_low_power_test
                           PRIVATE TX_INCLUDE_USER_DEFINE_FILE)

target_compile_options(threadx_low_power_test PRIVATE -Wall -Wextra -Werror)

add_test(${CMAKE_BUILD_TYPE}::threadx_low_power_test threadx_low_power_test)
