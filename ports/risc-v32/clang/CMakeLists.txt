# RISC-V32 Clang port.
#
# Clang and GNU use identical RISC-V assembly sources; this port
# compiles from gnu/src/ so fixes and features are shared automatically.
include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/threadx_riscv_port.cmake)

threadx_add_riscv_port(
    SRC_DIR     ${CMAKE_CURRENT_LIST_DIR}/../gnu/src
    INC_DIR     ${CMAKE_CURRENT_LIST_DIR}/inc
    EXAMPLE_DIR ${CMAKE_CURRENT_LIST_DIR}/example_build/qemu_virt
)
