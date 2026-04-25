# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR risc-v32)

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "gnu")
# -mrelax enables linker relaxation, in particular GP-relative
# addressing for symbols within +/- 2 KiB of __global_pointer$ (small
# data window). This requires the runtime to load the global pointer
# explicitly under .option norelax (see entry.s in the QEMU virt demo)
# and the linker script to PROVIDE __global_pointer$ centred in the
# small-data window (see link.lds). Without those two pieces in place
# -mrelax produces broken GP-relative loads on bare-metal targets.
set(ARCH_FLAGS "-g -march=rv32gc -mabi=ilp32d -mcmodel=medany -mrelax")
set(CFLAGS "${ARCH_FLAGS}")
set(ASFLAGS "${ARCH_FLAGS}")
set(LDFLAGS "${ARCH_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/riscv32-unknown-elf.cmake)
