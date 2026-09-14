#!/bin/bash

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
    cmake -Bbuild/$build -GNinja -DBUILD_SHARED_LIBS=ON -DCMAKE_TOOLCHAIN_FILE=$(dirname $(realpath $0))/../cmake/linux.cmake -DCMAKE_BUILD_TYPE=$build .
}

function build() {
    # -k 0 keeps Ninja going after a target fails, so one broken target no
    # longer decides whether the targets after it exist. ctest reports a
    # missing binary as a failing test, which turned a single link error into
    # a failure count that varied with build scheduling order.
    cmake --build build/$1 -- -k 0
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
    # Guard the pushd: with the caller capturing this function's status, set -e
    # no longer aborts here, so a missing build directory would otherwise let
    # ctest run in the source tree and report "no tests" as success.
    pushd build/$1 || return 1
    [ -z "${CTEST_PARALLEL_LEVEL}" ] && parallel="-j$2"
    if [ -z "${CTEST_REPEAT_FAIL}" ];
    then
        repeat_fail=2
    else
        repeat_fail=${CTEST_REPEAT_FAIL}
    fi
    # ctest's status is captured rather than allowed to abort the function, and
    # returned at the end. It was previously discarded by the popd that follows,
    # so a configuration with failing tests returned 0 and was reported as a pass.
    local status=0
    ctest $parallel --timeout 1000 -O $1.txt -T test --no-compress-output --test-output-size-passed 4194304 --test-output-size-failed 4194304 --output-on-failure --repeat until-pass:${repeat_fail} --output-junit $1.xml || status=$?
    popd
    # Tolerated because this is a summary for humans, and a ctest that died early
    # enough to leave no matching line must not be what stops the coverage below.
    grep -E "^(\s*[0-9]+|Total)" build/$1/$1.txt >build/$1.txt || true
    sed -i "s/\x1B\[[0-9;]*[JKmsu]//g" build/$1.txt
    if [[ $1 = *"_coverage" ]]; then
        ./coverage.sh $1 || status=$?
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

    # A failing configuration must not stop the ones after it: under set -e
    # the loop would abort and leave them unbuilt, which then reads as a wall
    # of missing-binary test failures. The status is accumulated and returned.
    build_status=0
    for item in $builds; do
        echo "Building $item"
        build $item || build_status=$?
        echo ""
    done
    [ $build_status -eq 0 ] || exit $build_status
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
        exit $exit_code
    else
        # Run builds in serial. The status is collected the same way the parallel
        # branch above collects it, so one failing configuration no longer stops
        # the remaining ones from being tested.
        exit_code=0
        for item in $builds; do
            echo "Testing $item"
            test $item $parallel_jobs || exit_code=$?
        done
        exit $exit_code
    fi
elif [ "$command" == "build_libs" ]; then
    build_libs
else
    help
fi
