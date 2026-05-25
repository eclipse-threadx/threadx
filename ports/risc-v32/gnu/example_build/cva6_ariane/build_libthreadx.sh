#!/bin/bash

pushd ../../../../../
cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32_gnu.cmake -DSOFT_FLOAT=1 .
cmake --build ./build/
popd
