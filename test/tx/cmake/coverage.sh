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

cd $(dirname $0)
mkdir -p coverage_report/$1

# gcov reads a data format tied to the compiler that produced it, so gcov has to match
# the gcc that built the objects. Since cmake/linux.cmake began honouring CC, taking
# whatever gcov happens to be first on PATH is no longer safe: building with gcc-14 and
# reading with gcov-13 gives "version 'B42*', prefer 'B33*'" from gcov, and gcovr turns
# that into "GCOV returncode was 3" and exits 64. The tests pass and then the coverage
# step fails with a Python traceback, which reads like a coverage bug rather than a
# toolchain mismatch.
#
# So derive gcov from CC rather than asking the caller to remember both. GCOV still
# overrides, for a toolchain that does not follow the gcc/gcov naming.
: "${CC:=gcc}"
if [ -z "${GCOV:-}" ]; then
    cc_base=$(basename "$CC")
    case "$cc_base" in
        gcc*) GCOV="gcov${cc_base#gcc}" ;;
        *)    GCOV="gcov" ;;
    esac
fi

if ! command -v "$GCOV" >/dev/null 2>&1; then
    echo "coverage.sh: '$GCOV' not found, derived from CC='$CC'." >&2
    echo "Install it, or set GCOV to the gcov matching that compiler." >&2
    exit 1
fi

gcovr --gcov-executable "$GCOV" --object-directory=build/$1/threadx/CMakeFiles/threadx.dir/common/src -r build/$1 -f ../../../common/src --xml-pretty --output coverage_report/$1.xml
gcovr --gcov-executable "$GCOV" --object-directory=build/$1/threadx/CMakeFiles/threadx.dir/common/src -r build/$1 -f ../../../common/src --html --html-details --output coverage_report/$1/index.html
