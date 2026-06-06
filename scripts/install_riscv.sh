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

# Install RISC-V bare-metal cross-compiler toolchain and QEMU for CI.
set -e

RELEASE_TAG="2026.04.26"
BASE_URL="https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/${RELEASE_TAG}"
# Use ubuntu-24.04 binaries to match ubuntu-latest runners.
RV32_TARBALL="riscv32-elf-ubuntu-24.04-gcc.tar.xz"
RV64_TARBALL="riscv64-elf-ubuntu-24.04-gcc.tar.xz"

echo "=== Installing QEMU and build tools ==="
sudo apt-get update -qq
sudo apt-get install -y -qq qemu-system-misc ninja-build cmake

echo "=== Downloading RISC-V GCC toolchain (${RELEASE_TAG}) ==="

# Both tarballs extract into riscv/ with non-overlapping prefixes
# (riscv32-unknown-elf-* and riscv64-unknown-elf-*).
for tarball in "$RV32_TARBALL" "$RV64_TARBALL"; do
    echo "Downloading ${tarball} ..."
    wget --no-verbose "${BASE_URL}/${tarball}" -O "/tmp/${tarball}"
    sudo tar xJf "/tmp/${tarball}" -C /opt
    rm "/tmp/${tarball}"
done

TOOLCHAIN_BIN=/opt/riscv/bin
echo "$TOOLCHAIN_BIN" >> "$GITHUB_PATH"

echo "=== Verifying installation ==="
"$TOOLCHAIN_BIN/riscv32-unknown-elf-gcc" --version | head -1
"$TOOLCHAIN_BIN/riscv64-unknown-elf-gcc" --version | head -1
qemu-system-riscv64 --version | head -1
