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
retry "${TIMEOUT[@]}" pip3 install --retries 3 --timeout 30 gcovr==4.1 || exit 1

# Upgrade cmake to the latest version.
retry "${TIMEOUT[@]}" pip install --retries 3 --timeout 30 --upgrade cmake || exit 1
