# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR risc-v32)

IF(DEFINED $ENV{GCC_INSTALL_PREFIX})
    SET(GCC_INSTALL_PREFIX "$ENV{GCC_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
ELSE()
    SET(GCC_INSTALL_PREFIX "/opt/riscv_rv32ima" CACHE INTERNAL "" FORCE)
ENDIF()

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "clang")
set(ARCH_FLAGS "--sysroot=${GCC_INSTALL_PREFIX}/riscv32-unknown-elf --target=riscv32 -g -march=rv32ima_zicsr -mabi=ilp32")
set(CFLAGS "${ARCH_FLAGS}")
set(ASFLAGS "${ARCH_FLAGS}")
set(LDFLAGS "--no-dynamic-linker -m elf32lriscv -static -nostdlib")

include(${CMAKE_CURRENT_LIST_DIR}/riscv32-clang-unknown-elf.cmake)
