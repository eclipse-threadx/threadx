#!/bin/bash
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
