#!/bin/bash
# Copyright (c) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

set -e

function help() {
    echo "Usage: $0 <float> <command> [all|<build_configuration> ...]"
    echo ""
    echo "  float:   hard | soft"
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
    local float=$1
    local build=$2

    cmake -B"build/${float}_${build}" -GNinja \
        -DCMAKE_TOOLCHAIN_FILE="${REPO_ROOT}/cmake/rh850_gnu.cmake" \
        -DRH850_FLOAT="$float" \
        -DCMAKE_BUILD_TYPE="$build" \
        .
}

function build() {
    cmake --build "build/$1_$2" -- -k 0
}

function run_test() {
    local float=$1
    local build=$2
    local parallel_jobs=$3

    pushd "build/${float}_${build}" || return 1
    [ -z "${CTEST_PARALLEL_LEVEL}" ] && parallel="-j${parallel_jobs}"
    if [ -z "${CTEST_REPEAT_FAIL}" ]; then
        repeat_fail=2
    else
        repeat_fail=${CTEST_REPEAT_FAIL}
    fi
    local status=0
    ctest $parallel --timeout 1000 \
        -O "${float}_${build}.txt" \
        -T test --no-compress-output \
        --test-output-size-passed 4194304 \
        --test-output-size-failed 4194304 \
        --output-on-failure \
        --repeat until-pass:${repeat_fail} \
        --output-junit "${float}_${build}.xml" || status=$?
    popd
    return $status
}

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../../.." && pwd)"
cd "$SCRIPT_DIR"

result=$(sed -n "/(BUILD_CONFIGURATIONS/,/)/p" CMakeLists.txt | sed ':label;N;s/\n/ /;b label' | grep -Pzo "[a-zA-Z0-9_]*build[a-zA-Z0-9_]*\s*" | tr -d '\0')
IFS=' '
read -ra build_configurations <<< "$result"

if [ $# -lt 2 ]; then
    help
fi

float=$1
shift
if [ "$float" != "hard" ] && [ "$float" != "soft" ]; then
    echo "Error: float must be 'hard' or 'soft'"
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
        generate "$float" "$item"
        echo ""
    done
    build_status=0
    for item in $builds; do
        echo "Building ${float} ${item}"
        build "$float" "$item" || build_status=$?
        echo ""
    done
    [ $build_status -eq 0 ] || exit $build_status
elif [ "$command" == "test" ]; then
    cores=$(nproc)
    if [ -z "${CTEST_PARALLEL_LEVEL}" ]; then
        build_counts=$(echo $builds | wc -w)
        parallel_jobs=$(($cores / $build_counts))
        parallel_jobs=$(($parallel_jobs + 2))
        pids=""
        for item in $builds; do
            echo "Testing ${float} ${item}"
            run_test "$float" "$item" "$parallel_jobs" &
            pids+=" $!"
        done
        exit_code=0
        for p in $pids; do
            wait $p || exit_code=$?
        done
        exit $exit_code
    else
        exit_code=0
        for item in $builds; do
            echo "Testing ${float} ${item}"
            run_test "$float" "$item" "$parallel_jobs" || exit_code=$?
        done
        exit $exit_code
    fi
else
    help
fi
