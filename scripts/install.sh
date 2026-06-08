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

#
# Install necessary softwares for Ubuntu.

# Remove large folder to save space
rm -rf /opt/hostedtoolcache

sudo apt-get update
sudo apt-get install -y \
    gcc-multilib \
    git \
    g++ \
    python3-pip \
    ninja-build \
    unifdef \
    p7zip-full \
    tofrodos \
    gawk \
    cmake \
    software-properties-common

python3 -m pip install --upgrade pip
pip3 install gcovr==4.1

# Upgrade cmake to the latest version.
pip install --upgrade cmake
