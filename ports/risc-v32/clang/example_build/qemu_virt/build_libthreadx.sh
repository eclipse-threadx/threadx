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


pushd ../../../../../
#cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32_clang.cmake .
cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32_clang.cmake .
cmake --build ./build/
popd
