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

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
REPO_ROOT=$(CDPATH= cd -- "${SCRIPT_DIR}/../../../../../" && pwd)
BUILD_DIR=${BUILD_DIR:-"${REPO_ROOT}/build/riscv64-qemu"}

cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" -GNinja \
    -DCMAKE_TOOLCHAIN_FILE="${REPO_ROOT}/cmake/riscv64_gnu.cmake"
cmake --build "${BUILD_DIR}" --target kernel.elf

printf 'Built %s\n' "${BUILD_DIR}/ports/risc-v64/gnu/example_build/qemu_virt/kernel.elf"
