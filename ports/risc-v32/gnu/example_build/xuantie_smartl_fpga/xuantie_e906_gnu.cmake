# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR xuantie_e906)

set(THREADX_ARCH "xuantie_e906")
set(THREADX_TOOLCHAIN "gnu")
set(ARCH_FLAGS "-g -mcpu=e906fd -mcmodel=medlow")
set(CFLAGS "${ARCH_FLAGS}")
set(ASFLAGS "${ARCH_FLAGS}")
set(LDFLAGS "${ARCH_FLAGS}")

set(TX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/tx_user.h)
set(THREADX_CUSTOM_PORT ${CMAKE_CURRENT_LIST_DIR}/../../)

include(${CMAKE_CURRENT_LIST_DIR}/../../../../../cmake/riscv64-unknown-elf.cmake)