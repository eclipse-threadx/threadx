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

set -e

XPACK_GCC_DIR=/opt/xpack-riscv-none-elf-gcc-15.2.0-1
if [ -d "${XPACK_GCC_DIR}/bin" ]; then
    export PATH="${XPACK_GCC_DIR}/bin:${PATH}"
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
THREADX_ROOT="${SCRIPT_DIR}/../../../../.."
cmake -B "${SCRIPT_DIR}/build" \
      -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="${THREADX_ROOT}/cmake/riscv-none-elf-rv32imc.cmake" \
      "${SCRIPT_DIR}"
cmake --build "${SCRIPT_DIR}/build"
