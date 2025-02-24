#!/bin/bash

pushd ../../../../../
cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/riscv64_gnu.cmake .
cmake --build ./build/
popd
