# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR risc-v32)

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "gnu")
set(ARCH_FLAGS "-g -march=rv32gc -mabi=ilp32d -mcmodel=medany -mrelax")
set(CFLAGS "${ARCH_FLAGS}")
set(ASFLAGS "${ARCH_FLAGS}")
set(LDFLAGS "${ARCH_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/riscv32-unknown-elf.cmake)
