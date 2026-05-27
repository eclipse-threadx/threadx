#!/bin/bash
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
