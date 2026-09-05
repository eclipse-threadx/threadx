#
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
#

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# Prevent CMake from executing default compiler test executables during configuration
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Cross-Compilation Toolchain Binary Names
set(CMAKE_C_COMPILER "riscv32-esp-elf-gcc")
set(CMAKE_CXX_COMPILER "riscv32-esp-elf-g++")
set(CMAKE_ASM_COMPILER "riscv32-esp-elf-gcc")
set(CMAKE_AR "riscv32-esp-elf-ar")
set(CMAKE_OBJCOPY "riscv32-esp-elf-objcopy")
set(CMAKE_OBJDUMP "riscv32-esp-elf-objdump")
set(CMAKE_SIZE "riscv32-esp-elf-size")

# Target Architecture Flags for ESP32-C6 (RV32IMAC Zicsr Zifencei)
set(ESP32C6_TARGET_FLAGS "-march=rv32imac_zicsr_zifencei -mabi=ilp32 -mcmodel=medany -ffunction-sections -fdata-sections")

set(CMAKE_C_FLAGS_INIT "${ESP32C6_TARGET_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_INIT "${ESP32C6_TARGET_FLAGS} -Wall -Wextra")
set(CMAKE_ASM_FLAGS_INIT "${ESP32C6_TARGET_FLAGS}")

# Search mode policies for bare-metal toolchains
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
