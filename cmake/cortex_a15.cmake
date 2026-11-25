# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT
# Portions of this file were generated with AI assistance.
#
# Toolchain file for Arm Cortex-A15 using GNU tools.

# Name of the target
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-a15)

set(THREADX_ARCH "cortex_a15")
set(THREADX_TOOLCHAIN "gnu")

set(MCPU_FLAGS "-marm -mcpu=cortex-a15")
set(VFP_FLAGS "")
set(SPEC_FLAGS "--specs=nosys.specs")

include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi.cmake)
