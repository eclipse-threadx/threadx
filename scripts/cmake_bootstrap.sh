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

function generate() {
    build=$1
    cmake -Bbuild/$build -GNinja -DBUILD_SHARED_LIBS=ON -DCMAKE_TOOLCHAIN_FILE=$(dirname $(realpath $0))/../cmake/linux.cmake -DCMAKE_BUILD_TYPE=$build .
}

function build() {
    cmake --build build/$1
}

function build_libs() {
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
    ctest $parallel --timeout 1000 -O $1.txt -T test --no-compress-output --test-output-size-passed 4194304 --test-output-size-failed 4194304 --output-on-failure --repeat until-pass:${repeat_fail} --output-junit $1.xml
    popd
    grep -E "^(\s*[0-9]+|Total)" build/$1/$1.txt >build/$1.txt
    sed -i "s/\x1B\[[0-9;]*[JKmsu]//g" build/$1.txt
    if [[ $1 = *"_coverage" ]]; then
        ./coverage.sh $1
    fi
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
        exit $exit_code
    else
        # Run builds in serial
        for item in $builds; do
            echo "Testing $item"
            test $item $parallel_jobs
        done
    fi
elif [ "$command" == "build_libs" ]; then
    build_libs
else
    help
fi
