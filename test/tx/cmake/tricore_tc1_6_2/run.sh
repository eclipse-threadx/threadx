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

set -e

# Regression test build runner for the TriCore TC1.6.2 port.
# Usage: run.sh build [all|<config> ...]
# The AURIX GCC (tricore-elf-) shall be on PATH. The images run on the TC375
# Lite Kit, so this script only builds them. See README.md for programming and running them.

function help() {
    echo "Usage: $0 build [all|<build_configuration> ...]"
    echo ""
    echo "Available build configurations:"
    for build in ${build_configurations[*]}; do
        echo "  $build"
    done
    exit 1
}

function validate() {
    for build in ${build_configurations[*]}; do
        if [ "$1" == "$build" ]; then
            return
        fi
    done
    help
}

function generate() {
    local build=$1
    cmake -B"build/${build}" -GNinja \
        -DCMAKE_TOOLCHAIN_FILE="${REPO_ROOT}/cmake/tricore_tc1_6_2.cmake" \
        -DCMAKE_BUILD_TYPE="${build}" \
        .
}

function build() {
    cmake --build "build/$1"
}

# Determine repo root and script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../../.." && pwd)"
cd "$SCRIPT_DIR"

# Parse build configurations from CMakeLists.txt
result=$(sed -n "/(BUILD_CONFIGURATIONS/,/)/p" CMakeLists.txt | sed ':label;N;s/\n/ /;b label' | grep -Pzo "[a-zA-Z0-9_]*build[a-zA-Z0-9_]*\s*" | tr -d '\0')
IFS=' '
read -ra build_configurations <<< "$result"

if [ $# -lt 1 ] || [ "$1" != "build" ]; then
    help
fi
shift

if [ "$#" == "0" ]; then
    builds=${build_configurations[0]}
elif [ "$*" == "all" ]; then
    builds=${build_configurations[@]}
else
    for item in $*; do
        validate "$item"
    done
    builds=$*
fi

# A failing configuration does not stop the ones after it. The status is
# accumulated and returned.
build_status=0
for item in $builds; do
    echo "Building ${item}"
    { generate "$item" && build "$item"; } || build_status=$?
    echo ""
done
exit $build_status
