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

# RISC-V regression test build/test runner.
# Usage: run.sh <arch> <build|test> [all|<config> ...]
#   arch: riscv32 or riscv64

function help() {
    echo "Usage: $0 <arch> <command> [all|<build_configuration> ...]"
    echo ""
    echo "  arch:    riscv32 | riscv64"
    echo "  command: build | test"
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
    local arch=$1
    local build=$2
    local toolchain_file
    local build_dir

    if [ "$arch" == "riscv32" ]; then
        toolchain_file="${REPO_ROOT}/cmake/riscv32_gnu.cmake"
    else
        toolchain_file="${REPO_ROOT}/cmake/riscv64_gnu.cmake"
    fi

    build_dir="build/${arch}_${build}"
    cmake -B"$build_dir" -GNinja \
        -DCMAKE_TOOLCHAIN_FILE="$toolchain_file" \
        -DCMAKE_BUILD_TYPE="$build" \
        .
}

function build() {
    local arch=$1
    local build=$2
    cmake --build "build/${arch}_${build}"
}

function run_test() {
    local arch=$1
    local build=$2
    local parallel_jobs=$3

    pushd "build/${arch}_${build}"
    [ -z "${CTEST_PARALLEL_LEVEL}" ] && parallel="-j${parallel_jobs}"
    if [ -z "${CTEST_REPEAT_FAIL}" ]; then
        repeat_fail=2
    else
        repeat_fail=${CTEST_REPEAT_FAIL}
    fi
    ctest $parallel --timeout 1000 \
        -O "${arch}_${build}.txt" \
        -T test --no-compress-output \
        --test-output-size-passed 4194304 \
        --test-output-size-failed 4194304 \
        --output-on-failure \
        --repeat until-pass:${repeat_fail} \
        --output-junit "${arch}_${build}.xml"
    popd
}

# Determine repo root and script directory
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../../.." && pwd)"
cd "$SCRIPT_DIR"

# Parse build configurations from CMakeLists.txt
result=$(sed -n "/(BUILD_CONFIGURATIONS/,/)/p" CMakeLists.txt | sed ':label;N;s/\n/ /;b label' | grep -Pzo "[a-zA-Z0-9_]*build[a-zA-Z0-9_]*\s*" | tr -d '\0')
IFS=' '
read -ra build_configurations <<< "$result"

if [ $# -lt 2 ]; then
    help
fi

arch=$1
shift

if [ "$arch" != "riscv32" ] && [ "$arch" != "riscv64" ]; then
    echo "Error: arch must be 'riscv32' or 'riscv64'"
    help
fi

command=$1
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

if [ "$command" == "build" ]; then
    for item in $builds; do
        generate "$arch" "$item"
        echo ""
    done

    for item in $builds; do
        echo "Building ${arch} ${item}"
        build "$arch" "$item"
        echo ""
    done
elif [ "$command" == "test" ]; then
    cores=$(nproc)
    if [ -z "${CTEST_PARALLEL_LEVEL}" ]; then
        build_counts=$(echo $builds | wc -w)
        parallel_jobs=$(($cores / $build_counts))
        parallel_jobs=$(($parallel_jobs + 2))
        pids=""
        for item in $builds; do
            echo "Testing ${arch} ${item}"
            run_test "$arch" "$item" "$parallel_jobs" &
            pids+=" $!"
        done
        exit_code=0
        for p in $pids; do
            wait $p || exit_code=$?
        done
        exit $exit_code
    else
        for item in $builds; do
            echo "Testing ${arch} ${item}"
            run_test "$arch" "$item" "$parallel_jobs"
        done
    fi
else
    help
fi
