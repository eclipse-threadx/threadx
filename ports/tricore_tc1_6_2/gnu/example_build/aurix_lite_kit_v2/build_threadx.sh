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

# Builds the ThreadX library tx.a for the TriCore TC1.6.2 port. See build_env.sh for
# the required environment.

set -e
cd "$(dirname "$0")"
. ./build_env.sh

OBJ_DIR="${BUILD_DIR}/tx"
rm -rf "${OBJ_DIR}"
mkdir -p "${OBJ_DIR}"
rm -f tx.a

for src in ../../src/*.c "${THREADX_ROOT}"/common/src/*.c; do
    obj="${OBJ_DIR}/$(basename "${src%.c}").o"
    "${CC}" ${CFLAGS} ${INCLUDES} -c "${src}" -o "${obj}"
done

"${AR}" -r tx.a "${OBJ_DIR}"/*.o
