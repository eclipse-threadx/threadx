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

# retry, TIMEOUT, TIMEOUT_LONG, APT_OPTIONS and fetch. Shared with install.sh,
# which is where they were written and where the reasons for their shape are
# recorded.
. "$(dirname "$(realpath "$0")")/tx_ci_common.sh"

# The release tag is pinned so that what CI compiles with is answerable from the
# repository, and the digests pin the bytes behind the tag. A tag can be deleted
# and re-pushed and a release asset can be replaced; either would be picked up
# silently without these. Both digests were taken from the releases API and then
# checked against the bytes the CDN actually serves, 10 Sep 2026. Moving the tag
# means refreshing all three lines together.
RELEASE_TAG="2026.04.26"
BASE_URL="https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/${RELEASE_TAG}"
# Use ubuntu-24.04 binaries to match ubuntu-latest runners.
RV32_TARBALL="riscv32-elf-ubuntu-24.04-gcc.tar.xz"
RV32_SHA256="73c9a5adbb38b779312e5b3fc10d624484364ef1f870314571ae92c486ba9917"
RV64_TARBALL="riscv64-elf-ubuntu-24.04-gcc.tar.xz"
RV64_SHA256="4a66a329653c9cfb869b826cae6d70c603bedcc1e13d419d3e3e92b99e0816b1"

echo "=== Installing QEMU and build tools ==="

# The update warns rather than gates, for the reason install.sh sets out at
# length: apt-get update fails if ANY configured repository serves a bad index,
# including the third-party ones the runner image carries and this project does
# not use. The install below is the gate, and it still fails hard on a package
# it cannot get. Under set -e a bare update would have ended the job -- and with
# the RISC-V suite running on every pull request, ended it for everybody.
if ! retry sudo "${TIMEOUT[@]}" apt-get "${APT_OPTIONS[@]}" update; then
    echo ""
    echo "install_riscv.sh: apt-get update did not fully succeed."
    echo "install_riscv.sh: continuing, because a repository this project does"
    echo "install_riscv.sh: not use can fail an update.  The install below is"
    echo "install_riscv.sh: the real gate."
    echo ""
fi
retry sudo "${TIMEOUT[@]}" apt-get "${APT_OPTIONS[@]}" install -y \
    qemu-system-misc \
    ninja-build \
    cmake || exit 1

echo "=== Downloading RISC-V GCC toolchain (${RELEASE_TAG}) ==="

# Both tarballs extract into riscv/ with non-overlapping prefixes
# (riscv32-unknown-elf-* and riscv64-unknown-elf-*).
for spec in "${RV32_TARBALL}:${RV32_SHA256}" "${RV64_TARBALL}:${RV64_SHA256}"; do
    tarball="${spec%%:*}"
    sha256="${spec##*:}"
    echo "Downloading ${tarball} ..."
    fetch "${BASE_URL}/${tarball}" "/tmp/${tarball}" "${sha256}" || exit 1
    sudo tar xJf "/tmp/${tarball}" -C /opt
    rm "/tmp/${tarball}"
done

TOOLCHAIN_BIN=/opt/riscv/bin
echo "$TOOLCHAIN_BIN" >> "$GITHUB_PATH"

echo "=== Verifying installation ==="
"$TOOLCHAIN_BIN/riscv32-unknown-elf-gcc" --version | head -1
"$TOOLCHAIN_BIN/riscv64-unknown-elf-gcc" --version | head -1
# Both QEMU system emulators are used -- run.sh selects one per architecture --
# so both are worth failing on here rather than at the first test.
qemu-system-riscv32 --version | head -1
qemu-system-riscv64 --version | head -1
