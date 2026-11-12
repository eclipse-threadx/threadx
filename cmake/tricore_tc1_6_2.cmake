# Copyright (c) 2026 safecubed GmbH
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT

# Portions of this file were generated with AI assistance.

# Toolchain file for the TriCore TC1.6.2 port (Infineon AURIX TC3) with the
# AURIX GCC.
#
# The port uses no driver library. See
# ports/tricore_tc1_6_2/gnu/readme_threadx.txt.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR tricore)

set(THREADX_ARCH "tricore_tc1_6_2")
set(THREADX_TOOLCHAIN "gnu")

set(CMAKE_C_COMPILER    tricore-elf-gcc)
set(CMAKE_CXX_COMPILER  tricore-elf-g++)
set(AS                  tricore-elf-as)
set(AR                  tricore-elf-ar)
set(OBJCOPY             tricore-elf-objcopy)
set(OBJDUMP             tricore-elf-objdump)
set(SIZE                tricore-elf-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS "-mcpu=tc38xx -std=c99 -fno-common -fstrict-volatile-bitfields -fdata-sections -ffunction-sections" CACHE INTERNAL "c compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "-mcpu=tc38xx -Wl,--gc-sections" CACHE INTERNAL "exe link flags")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3" CACHE INTERNAL "c debug compiler flags")
set(CMAKE_C_FLAGS_RELEASE "-O0" CACHE INTERNAL "c release compiler flags")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O0 -g3" CACHE INTERNAL "c relwithdebinfo compiler flags")
set(CMAKE_C_FLAGS_MINSIZEREL "-O0" CACHE INTERNAL "c minsizerel compiler flags")
