# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR risc-v64)

set(THREADX_ARCH "risc-v64")
set(THREADX_TOOLCHAIN "gnu")
set(ARCH_FLAGS "-g -march=rv64gc -mabi=lp64d -mcmodel=medany")
set(CFLAGS "${ARCH_FLAGS}")
set(ASFLAGS "${ARCH_FLAGS}")
set(LDFLAGS "${ARCH_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/riscv64-unknown-elf.cmake)
