#!/bin/bash
##############################################################################
# Copyright (c) 2026 safecubed GmbH
# Copyright (c) 2026 Eclipse ThreadX contributors
#
# This program and the accompanying materials are made available under the
# terms of the MIT License which is available at
# https://opensource.org/licenses/MIT.
#
# SPDX-License-Identifier: MIT
##############################################################################

# Portions of this file were generated with AI assistance.

# Builds the ThreadX library tx.a and sample_threadx.elf for the AURIX TC375
# Lite Kit without a driver library. The start-up code is crt0-tc38xx.o of
# the AURIX GCC: the compiler has no TC37x device, and the TC38x start-up
# code applies to the TC375 unchanged. sample_threadx.ld replaces the memory
# description of the TC38x.
#
# CROSS_COMPILE selects the toolchain prefix. The default is the AURIX GCC
# (tricore-elf-), which shall be on PATH.

set -e
cd "$(dirname "$0")"

: "${CROSS_COMPILE:=tricore-elf-}"
CC="${CROSS_COMPILE}gcc"
AR="${CROSS_COMPILE}ar"

THREADX_ROOT="../../../.."
BUILD_DIR="build"

CFLAGS="-mcpu=tc38xx -std=c99 -O0 -g3 -Wall -fmessage-length=0 -fno-common \
-fdata-sections -ffunction-sections"
INCLUDES="-I${THREADX_ROOT}/common/inc -I../inc"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}/tx" "${BUILD_DIR}/sample"
rm -f tx.a sample_threadx.elf sample_threadx.map

for src in ../src/*.c "${THREADX_ROOT}"/common/src/*.c; do
    "${CC}" ${CFLAGS} ${INCLUDES} -c "${src}" -o "${BUILD_DIR}/tx/$(basename "${src%.c}").o"
done
"${AR}" -r tx.a "${BUILD_DIR}"/tx/*.o

for src in sample_threadx.c tx_initialize_low_level.c; do
    obj="${BUILD_DIR}/sample/$(basename "${src%.c}").o"
    "${CC}" ${CFLAGS} ${INCLUDES} -c "${src}" -o "${obj}"
done

"${CC}" -mcpu=tc38xx -T sample_threadx.ld -Wl,--gc-sections -Wl,-Map=sample_threadx.map \
    -o sample_threadx.elf "${BUILD_DIR}"/sample/*.o tx.a
