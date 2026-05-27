#!/usr/bin/env bash
# /***************************************************************************/
# /* Copyright (C) 2026 Eclipse ThreadX contributors
#  *
#  * This program and the accompanying materials are made available under the
#  * terms of the MIT License which is available at
#  * https://opensource.org/licenses/MIT.
#  *
#  * SPDX-License-Identifier: MIT
#  ***************************************************************************/

# install_deps.sh -- Install Linux build/debug dependencies for the CORE-V MCU ThreadX port.
#
# Supported: Ubuntu 20.04 / 22.04 / 24.04 and Debian derivatives.
# Run as a regular user; sudo is invoked internally where needed.
#
# Installs:
#   - xPack riscv-none-elf-gcc  (multilib bare-metal toolchain, rv32imc/ilp32 capable)
#   - cmake, ninja-build, gdb-multiarch, openocd, usbutils
#
# Usage:
#   bash install_deps.sh

set -euo pipefail

die()  { echo "[ERROR] $*" >&2; exit 1; }
info() { echo "[INFO]  $*"; }

command -v apt-get >/dev/null 2>&1 || die "This script requires apt-get (Ubuntu/Debian)."

# ---------------------------------------------------------------------------
# xPack riscv-none-elf-gcc
# Cross-platform multilib toolchain (Linux/macOS/Windows).
# Includes rv32imc/ilp32 (soft-float) libgcc — required for CV32E40P.
# See: https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack
# ---------------------------------------------------------------------------
XPACK_VERSION="15.2.0-1"
XPACK_TARBALL="xpack-riscv-none-elf-gcc-${XPACK_VERSION}-linux-x64.tar.gz"
XPACK_URL="https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases/download/v${XPACK_VERSION}/${XPACK_TARBALL}"
XPACK_INSTALL_DIR="/opt/xpack-riscv-none-elf-gcc-${XPACK_VERSION}"
XPACK_BIN="${XPACK_INSTALL_DIR}/bin"

APT_PACKAGES=(
    cmake
    ninja-build
    gdb-multiarch
    openocd
    usbutils
    git
)

info "Updating package lists ..."
sudo apt-get update -qq

info "Installing apt packages: ${APT_PACKAGES[*]}"
sudo apt-get install -y "${APT_PACKAGES[@]}"

if [ -x "${XPACK_BIN}/riscv-none-elf-gcc" ]; then
    info "xPack riscv-none-elf-gcc ${XPACK_VERSION} already installed at ${XPACK_INSTALL_DIR}"
else
    info "Downloading xPack riscv-none-elf-gcc ${XPACK_VERSION} ..."
    wget --no-verbose "${XPACK_URL}" -O "/tmp/${XPACK_TARBALL}"
    sudo mkdir -p "${XPACK_INSTALL_DIR}"
    sudo tar xzf "/tmp/${XPACK_TARBALL}" -C /opt
    rm "/tmp/${XPACK_TARBALL}"
    info "Installed to ${XPACK_INSTALL_DIR}"
fi

info ""
info "Add the toolchain to your PATH:"
info "  export PATH=${XPACK_BIN}:\$PATH"
info ""
info "All dependencies installed."
info ""
info "Next steps:"
info "  1. Run scripts/setup_opella.sh to configure udev rules for the Ashling Opella LD."
info "  2. (WSL only) Follow the usbipd-win instructions printed by setup_opella.sh."

