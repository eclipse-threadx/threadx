# /***************************************************************************
#  * Copyright (C) 2026 Eclipse ThreadX contributors
#  *
#  * This program and the accompanying materials are made available under the
#  * terms of the MIT License which is available at
#  * https://opensource.org/licenses/MIT.
#  *
#  * SPDX-License-Identifier: MIT
#  ***************************************************************************/

# CMake toolchain file for bare-metal RV32IMC targets (e.g. CORE-V MCU / CV32E40P)
#
# Uses the xPack riscv-none-elf-gcc toolchain:
#   https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack
#
# This is a cross-platform (Linux/macOS/Windows), multilib-capable toolchain
# analogous to arm-none-eabi-gcc.  Its multilib includes rv32imc/ilp32
# (soft-float), so -lgcc resolves correctly without any distro package.
#
# Install via:  bash ports/risc-v32/gnu/example_build/core_v_mcu/install_deps.sh
# (downloads the latest xPack release to /opt/xpack-riscv-none-elf-gcc/)
#
# Do NOT use riscv32-unknown-elf-gcc (riscv-collab riscv32-elf release) for
# this target: that toolchain is built --disable-multilib --with-abi=ilp32d,
# so its libgcc only provides ilp32d (hardware FP) helpers and cannot link
# rv32imc/ilp32 soft-float objects.
#
# Target ISA : rv32imc_zicsr  (integer, multiply, compressed, Zicsr)
# ABI        : ilp32           (32-bit int/long/ptr, soft-float)
# Code model : medlow          (addresses in [0, 2 GiB))

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "gnu")

set(ARCH_FLAGS "-march=rv32imc_zicsr -mabi=ilp32 -mcmodel=medlow")
set(CFLAGS   "${ARCH_FLAGS}")
set(ASFLAGS  "${ARCH_FLAGS}")
set(LDFLAGS  "${ARCH_FLAGS}")

# xPack riscv-none-elf multilib toolchain.
set(CMAKE_C_COMPILER    riscv-none-elf-gcc)
set(CMAKE_CXX_COMPILER  riscv-none-elf-g++)
set(AS                  riscv-none-elf-as)
set(AR                  riscv-none-elf-ar)
set(OBJCOPY             riscv-none-elf-objcopy)
set(OBJDUMP             riscv-none-elf-objdump)
set(SIZE                riscv-none-elf-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Use static library for compiler feature probing (no linker script yet)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS   "${CFLAGS}"  CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${CFLAGS}"  CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "${ASFLAGS} -D__ASSEMBLER__" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS}" CACHE INTERNAL "exe link flags")

set(CMAKE_C_FLAGS_DEBUG   "-Og -g -ggdb3" CACHE INTERNAL "c debug flags")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g -ggdb3" CACHE INTERNAL "cxx debug flags")
set(CMAKE_ASM_FLAGS_DEBUG "-g -ggdb3"     CACHE INTERNAL "asm debug flags")

set(CMAKE_C_FLAGS_RELEASE   "-Os" CACHE INTERNAL "c release flags")
set(CMAKE_CXX_FLAGS_RELEASE "-Os" CACHE INTERNAL "cxx release flags")
set(CMAKE_ASM_FLAGS_RELEASE ""    CACHE INTERNAL "asm release flags")
