#!/bin/bash
#
# Install necessary softwares for Ubuntu.

apt-get update
apt-get install -y \
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

