include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/threadx_riscv_port.cmake)

threadx_add_riscv_port(
    SRC_DIR     ${CMAKE_CURRENT_LIST_DIR}/src
    INC_DIR     ${CMAKE_CURRENT_LIST_DIR}/inc
)
