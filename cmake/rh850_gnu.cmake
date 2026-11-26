# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR rh850)

set(THREADX_ARCH "rh850")
set(THREADX_TOOLCHAIN "gnu")

set(RH850_FLOAT "hard" CACHE STRING "RH850 float ABI: hard | soft")
set_property(CACHE RH850_FLOAT PROPERTY STRINGS hard soft)
if(RH850_FLOAT STREQUAL "soft")
    set(ARCH_FLAGS "-mv850e3v5 -mrh850-abi -msoft-float")
elseif(RH850_FLOAT STREQUAL "hard")
    set(ARCH_FLAGS "-mv850e3v5 -mrh850-abi")
else()
    message(FATAL_ERROR "RH850_FLOAT must be hard or soft")
endif()
set(CFLAGS "${ARCH_FLAGS}")
set(ASFLAGS "${ARCH_FLAGS}")
set(LDFLAGS "${ARCH_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/v850-elf.cmake)
