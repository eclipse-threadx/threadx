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

# Everything below reaches the network, and on this runner pool that is not
# dependable. apt-get update stalled seven times in a single day, once for more
# than two hours, each time with the Azure mirror returning nothing and the
# fallback to archive.ubuntu.com then going silent. Nothing here bounded a fetch
# and nothing retried one, so a mirror being down cost a whole run rather than a
# few seconds. Worse, this script has no set -e, so a failed update did not stop
# the install that follows: it went on to install from whatever index it already
# had, and the run failed later somewhere less obvious.
#
# Each command is wrapped in timeout rather than left to bound itself. apt's own
# Acquire timeouts were tried first and did not help: a run still sat inside a
# single apt-get update for nine and a half minutes without producing a line,
# having got as far as fetching noble-security InRelease, so the retry loop never
# got a turn and the step timeout was what eventually killed it. Whatever apt is
# waiting on there, it is not something Acquire::http::Timeout covers. timeout
# does not care where the wait is.
#
# The Acquire options are kept anyway, since they make a slow mirror give up
# sooner. The loop covers a mirror that is down rather than merely slow. The
# explicit exits stop a failed fetch from being carried forward into a build.
APT_OPTIONS=(-o Acquire::Retries=3
             -o Acquire::http::Timeout=20
             -o Acquire::https::Timeout=20)

# Two minutes per attempt, killed outright if it ignores the first signal. Three
# attempts plus backoff bounds a command at about six and a half minutes, and a
# command that exhausts its attempts exits rather than letting the next one run.
#
# timeout goes under sudo, not over it, so that it signals apt itself. Signalling
# sudo instead risks the kill landing on sudo while apt carries on holding the
# dpkg lock, which would leave every retry failing for a different reason than
# the one being retried.
TIMEOUT=(timeout --kill-after=10 120)

retry() {
    local attempt
    for attempt in 1 2 3; do
        if "$@"; then
            return 0
        fi
        echo "install.sh: '$*' failed or timed out on attempt ${attempt}"
        sleep $((attempt * 10))
    done
    echo "install.sh: '$*' failed after 3 attempts"
    return 1
}

retry sudo "${TIMEOUT[@]}" apt-get "${APT_OPTIONS[@]}" update || exit 1
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
