# /***************************************************************************
#  * Copyright (C) 2026 Eclipse ThreadX contributors
#  *
#  * This program and the accompanying materials are made available under the
#  * terms of the MIT License which is available at
#  * https://opensource.org/licenses/MIT.
#  *
#  * SPDX-License-Identifier: MIT
#  ***************************************************************************/

# CMake toolchain file for CORE-V MCU (CV32E40P, RV32IMC)
#
# Uses the dedicated riscv32-unknown-elf-gcc bare-metal toolchain from
# riscv-collab (riscv32-elf-ubuntu-24.04-gcc.tar.xz, installs to /opt/riscv).
# This is the correct toolchain for RV32 bare-metal targets — analogous to
# arm-none-eabi-gcc for Cortex-M.  It ships a native rv32/ilp32 libgcc with
# all soft-float and integer helpers (__clzsi2, __muldf3, etc.) built for the
# correct ABI.
#
# Do NOT use riscv64-unknown-elf-gcc for this target: the riscv-collab
# riscv64-elf toolchain is built without rv32 multilib and will produce
# missing-symbol linker errors with -nodefaultlibs.
#
# Install both toolchains with:  bash scripts/install_riscv.sh
#
# Target ISA : rv32imc_zicsr  (integer, multiply, compressed, Zicsr)
# ABI        : ilp32           (32-bit int/long/ptr, no hardware FP)
# Code model : medlow          (addresses in [0, 2 GiB))

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "gnu")

set(ARCH_FLAGS "-march=rv32imc_zicsr -mabi=ilp32 -mcmodel=medlow")
set(CFLAGS   "${ARCH_FLAGS}")
set(ASFLAGS  "${ARCH_FLAGS}")
set(LDFLAGS  "${ARCH_FLAGS}")

# Dedicated riscv32 bare-metal toolchain (riscv-collab riscv32-elf release).
set(CMAKE_C_COMPILER    riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER  riscv32-unknown-elf-g++)
set(AS                  riscv32-unknown-elf-as)
set(AR                  riscv32-unknown-elf-ar)
set(OBJCOPY             riscv32-unknown-elf-objcopy)
set(OBJDUMP             riscv32-unknown-elf-objdump)
set(SIZE                riscv32-unknown-elf-size)

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
