#!/bin/bash
set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
THREADX_ROOT="${SCRIPT_DIR}/../../../../.."
cmake -B "${SCRIPT_DIR}/build" \
      -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="${THREADX_ROOT}/cmake/riscv64-gcc-rv32imc.cmake" \
      "${SCRIPT_DIR}"
cmake --build "${SCRIPT_DIR}/build"
