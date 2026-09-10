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

# The network helpers -- retry, TIMEOUT, TIMEOUT_LONG and APT_OPTIONS -- live in
# tx_ci_common.sh, alongside the comments recording why each of them is shaped
# the way it is. They were defined here until the RISC-V suite was enabled in
# CI, which put a second install script on every pull request's critical path
# with none of them.
. "$(dirname "$(realpath "$0")")/tx_ci_common.sh"

# THE UPDATE IS NOT THE GATE, AND IT MUST NOT BE.  apt-get update fails if ANY
# configured repository serves a bad index, including ones this project does
# not use.  On a GitHub runner the image carries Google's and Microsoft's
# repositories, and a Hash Sum mismatch from Google's -- their CDN caught
# mid-publish, index and Release file eight hours apart -- failed this script
# three attempts running and turned a build red over a browser nobody was
# installing.
#
# The alternative of disabling third-party sources before updating is wrong
# here: this script also runs on a contributor's own machine, where silently
# rewriting their apt configuration would be a far worse thing to do than
# tolerating a stale index.
#
# So a failed update WARNS and the install below is the gate.  Nothing is
# weakened by that: apt-get install still fails hard on a package it cannot
# find, so an archive that is genuinely unreachable still stops the script --
# one step later, and saying which package it could not get.
if ! retry sudo "${TIMEOUT[@]}" apt-get "${APT_OPTIONS[@]}" update; then
    echo ""
    echo "install.sh: apt-get update did not fully succeed."
    echo "install.sh: continuing, because a repository this project does not"
    echo "install.sh: use can fail an update.  The install below is the real"
    echo "install.sh: gate and fails if any package needed is unavailable."
    echo ""
fi
retry sudo "${TIMEOUT[@]}" apt-get "${APT_OPTIONS[@]}" install -y \
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
    software-properties-common || exit 1

retry "${TIMEOUT[@]}" python3 -m pip install --retries 3 --timeout 30 --upgrade pip || exit 1
# gcovr was pinned to 4.1, released in 2018. That version cannot do what merging
# the coverage of several build configurations needs: it has no --json and no
# --add-tracefile, both of which arrived later. The pin is exact rather than
# floating so the coverage percentage stays comparable between runs -- the
# denominator is a property of the tool as much as of the tree -- and it is moved
# by hand, because it lives in a shell script that Dependabot cannot parse.
#
# Measured before bumping, on the default_build_coverage tree of test/tx, over
# the same gcda with the same gcov, varying only the gcovr version: 4.1, 7.0,
# 8.3 and 8.6 all report lines-valid 3827 across 177 files and branches-valid
# 1994. The denominator does not move with the tool, so this bump moves no
# number and any movement in a later run belongs to a later change.
#
# The numerator does move, but not because of gcovr: tx_thread_system_resume.c
# line 529 is executed on some runs of the suite and not others, so the same
# tree reports 3826 or 3827 covered lines with every test passing either way.
# That is a property of the suite, not of this pin.
retry "${TIMEOUT[@]}" pip3 install --retries 3 --timeout 30 gcovr==8.6 || exit 1

# Upgrade cmake to the latest version.
retry "${TIMEOUT[@]}" pip install --retries 3 --timeout 30 --upgrade cmake || exit 1
