#!/bin/bash
##############################################################################
# Copyright (c) 2024 Microsoft Corporation
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
##############################################################################

# Build the bananapi-f3 (SpacemiT K1) ThreadX kernel.
#
# Boot flow: FSBL -> OpenSBI (M-mode) -> U-Boot (S-mode) -> ThreadX (S-mode).
#
# OpenSBI runs in M-mode and delegates S-mode to the next stage.
# U-Boot runs in S-mode; any code launched from U-Boot also runs in S-mode.
#
# Use the TX_RISCV_SMODE CMake option to build libthreadx.a for S-mode.
#
# This libthreadx.a (S-mode), then links the BSP objects
# to produce kernel.elf / kernel.bin.

set -e

# Where ThreadX is loaded by U-Boot (${loadaddr}). Must match the link.lds origin.
LOAD_ADDR=0x00200000

# printf "y\n" | rm -rf ../../../../../build/
rm -f kernel.elf kernel.bin kernel.uImage

pushd ../../../../../
cmake -Bbuild -GNinja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv64_gnu.cmake \
  -DTX_USER_FILE="" \
  -DTX_RISCV_SMODE=ON \
  .
cmake --build ./build/
popd

riscv64-unknown-elf-gcc \
  -march=rv64gc -mabi=lp64d \
  -mcmodel=medany -O0 -g3 -Wall \
  -DTX_RISCV_SMODE \
  -ffunction-sections -fdata-sections \
  -I../../../../../common/inc \
  -I../../inc \
  entry.S \
  tx_initialize_low_level.S \
  board.c uart.c hwtimer.c plic.c trap.c demo_threadx.c \
  -L../../../../../build -lthreadx \
  -T link.lds -nostartfiles \
  -o kernel.elf

# Strip ELF metadata down to the loadable bytes.
riscv64-unknown-elf-objcopy -O binary kernel.elf kernel.bin

echo "== Build artifacts =="
ls -la kernel.elf kernel.bin 2>/dev/null || true
echo
riscv64-unknown-elf-size kernel.elf || true
echo

# Run on Bananapi BPI-F3
# Stop the boot at U-Boot (press reset button and press "s" key continuously to stop autoboot).
# at "=>" prompt, load the kernel using the following commands:
#
# "  ELF:  tftpboot ${loadaddr} kernel.elf && bootelf ${loadaddr}"
# "  BIN:  tftpboot 0x200000 kernel.bin && go 0x200000"
