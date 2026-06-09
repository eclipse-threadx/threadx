include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/threadx_riscv_port.cmake)

threadx_add_riscv_port(
    SRC_DIR     ${CMAKE_CURRENT_LIST_DIR}/src
    INC_DIR     ${CMAKE_CURRENT_LIST_DIR}/inc
)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/example_build/qemu_virt/CMakeLists.txt)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/example_build/qemu_virt)
endif()
