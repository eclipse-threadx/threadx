# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

set(CMAKE_C_COMPILER    v850-elf-gcc)
set(CMAKE_CXX_COMPILER  v850-elf-g++)
set(CMAKE_ASM_COMPILER  v850-elf-gcc)
set(AS                  v850-elf-as)
set(AR                  v850-elf-ar)
set(OBJCOPY             v850-elf-objcopy)
set(OBJDUMP             v850-elf-objdump)
set(SIZE                v850-elf-size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS   "${CFLAGS}" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "${CFLAGS}" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS "${ASFLAGS}" CACHE INTERNAL "asm compiler flags")
set(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS}" CACHE INTERNAL "exe link flags")

set(CMAKE_C_FLAGS_DEBUG "-Og -g" CACHE INTERNAL "c debug compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm debug compiler flags")
set(CMAKE_C_FLAGS_RELEASE "-O2" CACHE INTERNAL "c release compiler flags")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm release compiler flags")
