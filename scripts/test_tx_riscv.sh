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

# Run RISC-V regression tests for both RV32 and RV64 on QEMU.
# Usage: test_tx_riscv.sh [all|<config>]

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
RUN_SH="${SCRIPT_DIR}/../test/tx/cmake/riscv/run.sh"

ARGS="${@:-all}"

exit_code=0

echo "=== Testing RISC-V32 ==="
CTEST_PARALLEL_LEVEL=4 "$RUN_SH" riscv32 test $ARGS || exit_code=$?

echo ""
echo "=== Testing RISC-V64 ==="
CTEST_PARALLEL_LEVEL=4 "$RUN_SH" riscv64 test $ARGS || exit_code=$?

exit $exit_code
