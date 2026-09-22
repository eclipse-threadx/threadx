# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT
# Portions of this file were generated with AI assistance.
#
# Toolchain file for Arm Cortex-A75 using GNU tools.

# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-a75)

set(THREADX_ARCH "cortex_a75")
set(THREADX_TOOLCHAIN "gnu")

set(MCPU_FLAGS "-mcpu=cortex-a75")
set(VFP_FLAGS "")
set(SPEC_FLAGS "--specs=nosys.specs")

include(${CMAKE_CURRENT_LIST_DIR}/aarch64-none-elf.cmake)
