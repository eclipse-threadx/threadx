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
rm -rf build
cmake -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=ports/risc-v32/gnu/example_build/xuantie_smartl_fpga/xuantie_e906_gnu.cmake .
cmake --build ./build/
popd
