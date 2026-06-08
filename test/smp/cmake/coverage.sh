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

cd $(dirname $0)
threadx_smp=$(realpath ../../../common_smp/src)
mkdir -p coverage_report/$1
gcovr --object-directory=build/$1/threadx_smp/CMakeFiles/threadx_smp.dir/$threadx_smp -r build/$1 -f ../../../common_smp/src --xml-pretty --output coverage_report/$1.xml
gcovr --object-directory=build/$1/threadx_smp/CMakeFiles/threadx_smp.dir/$threadx_smp -r build/$1 -f ../../../common_smp/src --html --html-details --output coverage_report/$1/index.html
