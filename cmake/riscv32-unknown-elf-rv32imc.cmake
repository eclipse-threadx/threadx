# /***************************************************************************
#  * Copyright (C) 2026 Eclipse ThreadX contributors
#  *
#  * This program and the accompanying materials are made available under the
#  * terms of the MIT License which is available at
#  * https://opensource.org/licenses/MIT.
#  *
#  * SPDX-License-Identifier: MIT
#  ***************************************************************************/

# CMake toolchain file for RV32GC targets (QEMU virt, etc.)
#
# Uses the dedicated riscv32-unknown-elf-gcc bare-metal toolchain from
# riscv-collab (riscv32-elf-ubuntu-24.04-gcc.tar.xz, installs to /opt/riscv
# via scripts/install_riscv.sh).
#
# IMPORTANT: the riscv-collab riscv32-elf toolchain is built
# --disable-multilib --with-abi=ilp32d, so its libgcc only provides
# ilp32d (hardware double-FP) helpers.  This cmake file therefore targets
# rv32gc/ilp32d, which matches the toolchain's built-in ABI and works
# correctly with QEMU virt (which emulates hardware FP).
#
# Do NOT use this file for bare-metal targets that lack hardware FP (e.g.
# CV32E40P, rv32imc).  For those, use cmake/riscv64-ubuntu-rv32imc.cmake
# which calls the Ubuntu gcc-riscv64-unknown-elf package — a multilib build
# that includes the rv32im/ilp32 (soft-float) libgcc variant.
#
# Target ISA : rv32gc  (integer, multiply, atomics, FP, compressed, Zicsr)
# ABI        : ilp32d  (32-bit int/long/ptr, hardware double-precision FP)
# Code model : medlow  (addresses in [0, 2 GiB))

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

set(THREADX_ARCH "risc-v32")
set(THREADX_TOOLCHAIN "gnu")

set(ARCH_FLAGS "-march=rv32gc -mabi=ilp32d -mcmodel=medlow")
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
