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

function help() {
    echo "Usage: $0 [build|test] [all|<build_configuration> <build_configuration>...]"
    echo "Available build_configuration:"
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

# CMake records the compiler it detected inside the build directory and keeps using it on
# every later configure. Changing CC would otherwise be ignored without a word: the build
# reports success while still using the compiler the directory was first configured with,
# so anyone verifying a change against a second compiler would be reading stale results.
# Only the C compiler is consulted, because these test trees declare LANGUAGES C.
function compiler_changed() {
    local build=$1
    local recorded requested

    [ -d "build/$build" ] || return 1

    recorded=$(sed -n 's/^set(CMAKE_C_COMPILER "\(.*\)")$/\1/p' build/$build/CMakeFiles/*/CMakeCCompiler.cmake 2>/dev/null | head -1)
    [ -n "$recorded" ] || return 1

    requested=$(command -v "${CC:-gcc}" 2>/dev/null)
    [ -n "$requested" ] || return 1

    [ "$recorded" != "$requested" ]
}

function generate() {
    build=$1
    if compiler_changed $build; then
        echo "Compiler changed since build/$build was configured. Reconfiguring from scratch."
        rm -rf build/$build
    fi
    cmake -Bbuild/$build -GNinja -DBUILD_SHARED_LIBS=ON -DCMAKE_TOOLCHAIN_FILE=$(dirname $(realpath $0))/../cmake/linux.cmake -DCMAKE_BUILD_TYPE=$build -DTX_COVERAGE=${TX_COVERAGE:-OFF} .
}

function build() {
    cmake --build build/$1
}

function build_libs() {
    if compiler_changed libs; then
        echo "Compiler changed since build/libs was configured. Reconfiguring from scratch."
        rm -rf build/libs
    fi
    cmake -Bbuild/libs -GNinja -DBUILD_SHARED_LIBS=ON -DCMAKE_TOOLCHAIN_FILE=$(dirname $(realpath $0))/../cmake/linux.cmake libs
    cmake --build build/libs
}

function test() {
    pushd build/$1
    [ -z "${CTEST_PARALLEL_LEVEL}" ] && parallel="-j$2"
    if [ -z "${CTEST_REPEAT_FAIL}" ];
    then
        repeat_fail=2
    else
        repeat_fail=${CTEST_REPEAT_FAIL}
    fi
    # ctest's status is captured rather than allowed to abort the function, and
    # returned at the end. set -e would otherwise stop here on the first failing
    # test, and every configuration after it would go untested -- and, before
    # collection moved out of this function, uncovered as well. The gcda files
    # exist by that point, so a failing run threw away coverage it had already
    # collected, and the run whose behaviour changed is exactly the one whose
    # coverage is worth reading. Measured: the failing run of 2026-08-18 produced
    # test_reports artifacts and no coverage_report artifact at all. Collection
    # itself now happens in collect_all_coverage, after every configuration has
    # been tested, and it does not stop at the first failure either.
    local status=0
    ctest $parallel --timeout 1000 -O $1.txt -T test --no-compress-output --test-output-size-passed 4194304 --test-output-size-failed 4194304 --output-on-failure --repeat until-pass:${repeat_fail} --output-junit $1.xml || status=$?
    popd
    # Tolerated because this is a summary for humans, and a ctest that died early
    # enough to leave no matching line must not be what stops the coverage below.
    grep -E "^(\s*[0-9]+|Total)" build/$1/$1.txt >build/$1.txt || true
    sed -i "s/\x1B\[[0-9;]*[JKmsu]//g" build/$1.txt
    return $status
}

# Coverage is collected for any configuration that was instrumented, which is
# what TX_COVERAGE decides. The build-type match is kept for the one
# configuration instrumented by its name, so building a single configuration by
# hand behaves exactly as it did before.
#
# Three of the test trees -- freertos, posix and tx/cmake/riscv -- have no
# coverage.sh at all, and this script serves them too. Say so rather than
# failing, so that TX_COVERAGE=ON is harmless anywhere it does not apply.
function collect_coverage() {
    if [[ $1 = *"_coverage" ]] || [ "${TX_COVERAGE:-OFF}" = "ON" ]; then
        if [ ! -x ./coverage.sh ]; then
            echo "No coverage.sh in $(pwd); skipping coverage for $1."
            return 0
        fi
        ./coverage.sh $1
    fi
}

# Collection runs after every configuration has been tested, and strictly one at
# a time.
#
# It used to sit inside test(), which was safe only while a single configuration
# was instrumented. gcov writes its intermediate .gcov files into the directory
# gcovr is rooted at, and coverage.sh roots every configuration at the repository
# root so that the report can name files the way the repository does. Five
# concurrent gcovr processes therefore share one scratch directory and delete
# each other's output. The symptom is a gcovr SanityCheckError naming a .gcov
# file that "doesn't exist but no error from GCOV detected", and it cost the
# report for three of the five configurations while all 480 tests still passed --
# a green suite with most of its coverage missing.
#
# Measured both ways: two gcovr invocations rooted at the repository root fail
# when run concurrently and both succeed when run in sequence. CI happens to be
# safe already, because test_tx.sh sets CTEST_PARALLEL_LEVEL=1 and takes the
# serial branch below, but that is a coincidence of one caller rather than a
# property of this script.
#
# Collection deliberately does not stop at the first failure, for the reason
# given in test(): the configurations that did produce data should still report.
#
# The trade this makes, recorded rather than discovered later: a configuration
# that hangs long enough to hit the job's own timeout now costs the reports for
# the configurations that already finished, where collecting inside test() would
# have kept them. That is judged the smaller risk -- a per-test timeout is a
# ctest failure and the loop carries on, the job timeout sits at 60 minutes
# against a suite that takes 5 to 25, and the alternative loses most of the
# coverage on every parallel run instead of on a hang.
function collect_all_coverage() {
    local item status=0
    for item in $builds; do
        collect_coverage $item || status=$?
    done

    # Union the per-configuration reports. Only the union is a coverage figure:
    # each configuration compiles a different set of TX_ feature macros, so a
    # line one of them compiles out is absent from its denominator rather than
    # uncovered in it, and an average of five percentages means nothing.
    #
    # Done here rather than as a separate workflow step so a local run produces
    # the same merged report CI reads.
    if [ "${TX_COVERAGE:-OFF}" = "ON" ] && [ -x ./coverage.sh ]; then
        ./coverage.sh --merge || status=$?
    fi
    return $status
}

cd $(dirname $0)

result=$(sed -n "/(BUILD_CONFIGURATIONS/,/)/p" CMakeLists.txt|sed ':label;N;s/\n/ /;b label'|grep -Pzo "[a-zA-Z0-9_]*build[a-zA-Z0-9_]*\s*"| tr -d '\0')
IFS=' '
read -ra build_configurations <<< "$result"

if [ $# -lt 1 ]; then
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
        validate $item
    done
    builds=$*
fi

if [ "$command" == "build" ]; then
    for item in $builds; do
        generate $item
        echo ""
    done

    for item in $builds; do
        echo "Building $item"
        build $item
        echo ""
    done
elif [ "$command" == "test" ]; then
    cores=$(nproc)
    if [ -z "${CTEST_PARALLEL_LEVEL}" ];
    then
        # Run builds in parallel
        build_counts=$(echo $builds | wc -w)
        parallel_jobs=$(($cores / $build_counts))
        parallel_jobs=$(($parallel_jobs + 2))
        pids=""
        for item in $builds; do
            echo "Testing $item"
            test $item $parallel_jobs &
            pids+=" $!"
        done
        exit_code=0
        for p in $pids; do
            wait $p || exit_code=$?
        done
        # A coverage failure turns the run red, but must not overwrite a test
        # failure's status with its own.
        coverage_status=0
        collect_all_coverage || coverage_status=$?
        [ $exit_code -ne 0 ] || exit_code=$coverage_status
        exit $exit_code
    else
        # Run builds in serial. The status is collected the same way the parallel
        # branch above collects it, so one failing configuration no longer stops
        # the remaining ones from being tested. That mattered more after the
        # suites moved to serial execution: a failure in the first configuration
        # meant the other four never ran, and their coverage was never collected
        # either.
        exit_code=0
        for item in $builds; do
            echo "Testing $item"
            test $item $parallel_jobs || exit_code=$?
        done
        # A coverage failure turns the run red, but must not overwrite a test
        # failure's status with its own.
        coverage_status=0
        collect_all_coverage || coverage_status=$?
        [ $exit_code -ne 0 ] || exit_code=$coverage_status
        exit $exit_code
    fi
elif [ "$command" == "build_libs" ]; then
    build_libs
else
    help
fi
