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

# gcov reads a data format tied to the compiler that produced it, so gcov has to match
# the gcc that built the objects. Since cmake/linux.cmake began honouring CC, taking
# whatever gcov happens to be first on PATH is no longer safe: building with gcc-14 and
# reading with gcov-13 gives "version 'B42*', prefer 'B33*'" from gcov, and gcovr turns
# that into "GCOV returncode was 3" and exits 64. The tests pass and then the coverage
# step fails with a Python traceback, which reads like a coverage bug rather than a
# toolchain mismatch.
#
# So derive gcov from CC rather than asking the caller to remember both. GCOV still
# overrides, for a toolchain that does not follow the gcc/gcov naming.
: "${CC:=gcc}"
if [ -z "${GCOV:-}" ]; then
    cc_base=$(basename "$CC")
    case "$cc_base" in
        gcc*) GCOV="gcov${cc_base#gcc}" ;;
        *)    GCOV="gcov" ;;
    esac
fi

if ! command -v "$GCOV" >/dev/null 2>&1; then
    echo "coverage.sh: '$GCOV' not found, derived from CC='$CC'." >&2
    echo "Install it, or set GCOV to the gcov matching that compiler." >&2
    exit 1
fi

# The repository root, needed by every mode below. It has to be absolute: see the
# note on the per-configuration paths further down.
repo_root=$(cd ../../.. && pwd)
filter=$repo_root/common_smp/src

# --merge unions the per-configuration reports into the one number that means
# something. Each configuration writes an intermediate JSON beside its XML, and
# this pass adds them all together.
#
# Reporting five separate percentages instead would invite a reader to average
# them, and an average is not a coverage figure -- a line covered only by
# trace_build is covered, and only the union says so.
#
# Do not be alarmed when the merged percentage is lower than the single
# configuration this used to report. That is the point: the denominator now
# includes code the old report never counted at all, because it was compiled out
# of the only instrumented build.
if [ "$1" = "--merge" ]; then
    shopt -s nullglob
    tracefiles=(coverage_report/per_configuration/*.json)
    shopt -u nullglob
    if [ ${#tracefiles[@]} -eq 0 ]; then
        echo "coverage.sh --merge: no JSON in coverage_report/per_configuration/." >&2
        echo "Run the suites with TX_COVERAGE=ON first." >&2
        exit 1
    fi

    add_args=()
    for t in "${tracefiles[@]}"; do
        add_args+=(--add-tracefile "$t")
    done

    mkdir -p coverage_report/merged
    gcovr -r "$repo_root" "${add_args[@]}" --xml-pretty --output coverage_report/merged.xml
    gcovr -r "$repo_root" "${add_args[@]}" --html --html-details --output coverage_report/merged/index.html

    if ! grep -q "<class " coverage_report/merged.xml; then
        echo "coverage.sh --merge: the merged report contains no files." >&2
        exit 1
    fi

    # Named, not just counted. coverage_report/ is not cleaned between runs, so a
    # tracefile left by an earlier run of a different set of configurations would
    # otherwise be merged in without anything saying so.
    echo "coverage.sh --merge: SMP, ${#tracefiles[@]} configuration(s):"
    for t in "${tracefiles[@]}"; do
        echo "    $(basename "$t" .json)"
    done
    exit 0
fi

# gcovr is given three paths below, and each of them has to be absolute, for a
# different reason.
#
# -r is the repository root rather than the build directory, so the report names
# files the way the repository does -- "common_smp/src/tx_block_allocate.c"
# instead of "/home/runner/work/threadx/threadx/common_smp/src/tx_block_allocate.c".
# With -r inside build/, gcovr cannot express the sources relative to it, because
# they are outside it, and falls back to absolute paths. Those paths then differ
# on every machine and disagree with the <source> element written beside them in
# the same file, so anything that maps coverage back to the repository -- PR
# annotations, Codecov, SonarQube -- cannot follow them.
#
# Both -r and -f must be absolute. "-r ../../.. -f common_smp/src" produces a
# report containing zero files and exits 0, which is the worst failure mode
# available here: a green run carrying an empty report. Measured, not assumed.
repo_root=$(cd ../../.. && pwd)

# This is what actually scopes the report to one build configuration, and it is
# the positional search path -- not --object-directory, which used to be here
# and was doing nothing at all. That flag tells gcovr how to get from a gcda
# file back to the compiler's working directory; it does not restrict which gcda
# files are found. Pointed at an empty directory it still produced the full
# report, because gcovr searches -r as well.
#
# That matters more now than it did before. While -r was build/$1 it happened to
# constrain the search to this configuration by accident. -r is the repository
# root now, and every configuration's gcda lies somewhere under it, so without
# an explicit search path the report would silently merge all five.
#
# The odd shape of this path is CMake's, not ours: common_smp/src sits outside
# this directory's source tree, so CMake mangles each source file's absolute
# path into the object path, minus the leading slash. Hence the concatenation
# rather than a join -- $repo_root already begins with one.
objdir=$PWD/build/$1/threadx_smp/CMakeFiles/threadx_smp.dir$repo_root/common_smp/src

# The Linux port's own sources are deliberately outside -f. gcno files exist for
# two directories -- common_smp/src and ports_smp/linux/gnu/src -- and only the
# first is reported. The kernel is what this suite is here to cover; the
# architecture ports are validated functionally rather than structurally, and
# linux/gnu is a development host port that nothing ships on. Written down
# because a filter argument on its own is not a decision the next reader can see.

# Per-configuration output is kept in a subdirectory of its own, and the merged
# report sits alongside it at the top. That is not tidiness: the Pages deploy
# uploads coverage_report wholesale and merges the ThreadX and SMP artifacts
# into one tree, and every configuration directory has the same name in both
# suites. Left at the top level, default_build_coverage/ from one suite would
# overwrite the other's on the published site. Nested here, the top level still
# holds exactly the one suite directory the deploy expects.
mkdir -p coverage_report/per_configuration/$1
gcovr --gcov-executable "$GCOV" -r "$repo_root" -f "$filter" "$objdir" \
      --json coverage_report/per_configuration/$1.json \
      --xml-pretty --output coverage_report/per_configuration/$1.xml
gcovr --gcov-executable "$GCOV" -r "$repo_root" -f "$filter" "$objdir" --html --html-details --output coverage_report/per_configuration/$1/index.html

# An empty report is not an error as far as gcovr is concerned: it warns and
# exits 0. Worse, it advertises line-rate="1.0" alongside lines-valid="0", so
# every downstream consumer reads "no data at all" as "100% covered". A coverage
# threshold cannot catch that, because an empty report passes any threshold. So
# the assertion belongs here, next to the paths that would cause it.
if ! grep -q "<class " coverage_report/per_configuration/$1.xml; then
    echo "coverage.sh: the report for '$1' contains no files." >&2
    echo "Expected gcda files under $objdir." >&2
    exit 1
fi
