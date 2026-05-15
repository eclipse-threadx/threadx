#!/bin/bash
# Build RISC-V regression tests for both RV32 and RV64.
# Usage: build_tx_riscv.sh [all|<config>]

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
RUN_SH="${SCRIPT_DIR}/../test/tx/cmake/riscv/run.sh"

ARGS="${@:-all}"

echo "=== Building RISC-V32 ==="
"$RUN_SH" riscv32 build $ARGS

echo ""
echo "=== Building RISC-V64 ==="
"$RUN_SH" riscv64 build $ARGS
