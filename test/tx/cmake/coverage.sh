#!/bin/bash

set -e

cd $(dirname $0)

# Both -r and -f have to be absolute. "-r ../../.. -f common/src" produces a
# report containing zero files and exits 0, which is the worst failure mode
# available here: a green run carrying an empty report. Rooting every
# configuration at the repository root rather than at its own build directory
# also makes the file names repo-relative, so the same source file is named
# identically in every configuration's report -- which is what lets --merge
# below union them at all.
repo_root=$(cd ../../.. && pwd)
filter=$repo_root/common/src

# The coverage ratchet. CI fails below these, and they are raised as the gaps
# close, so a change that loses coverage fails here rather than in review. They
# are deliberately not 100%: a gate nothing can pass gets turned off, and the
# point is to hold the floor while it rises.
#
# The margin below the measured figure is not slack for regressions. Branches on
# the resume-from-ISR path are reached or missed depending on how the host
# schedules the run, so the same tree measures a little differently each time,
# and a gate set at the best observation fails on a tree nobody changed.
#
# The denominator is the whole of common/src. The misra_build configuration
# compiles common/src/tx_misra.c and the six *_initialize.c files that
# TX_INLINE_INITIALIZATION empties on this port, so a figure taken from the other
# five configurations counts 119 fewer lines and 519 fewer branches and is not
# comparable with this one. Measured over the six configurations: 4309/4339 lines
# and 4482/4627 branches, from a clean tree.
min_line=${TX_COVERAGE_MIN_LINE:-99.20}
min_branch=${TX_COVERAGE_MIN_BRANCH:-96.50}

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
    echo "coverage.sh --merge: ThreadX, ${#tracefiles[@]} configuration(s):"
    for t in "${tracefiles[@]}"; do
        echo "    $(basename "$t" .json)"
    done

    # Run after the reports are written, so a failed gate still leaves behind the
    # report that explains it. stdout is the per-file table, which the reports
    # already carry; the reason for a failure goes to stderr.
    gcovr -r "$repo_root" "${add_args[@]}" \
          --fail-under-line "$min_line" --fail-under-branch "$min_branch" >/dev/null || exit $?
    exit 0
fi

# The positional search path is what scopes the report to one build
# configuration. --object-directory does not: with -r at the repository root
# gcovr still walks the whole tree, finds every configuration's gcda and reports
# the union, so all five configurations produce identical reports and --merge
# unions five copies of the same thing. It has to be absolute for the same
# reason -r does, since gcovr resolves it against the root.
objdir=$PWD/build/$1/threadx/CMakeFiles/threadx.dir/common/src

# Per-configuration output is kept in a subdirectory of its own, and the merged
# report sits alongside it at the top. That is not tidiness: the Pages deploy
# uploads coverage_report wholesale and merges the ThreadX and SMP artifacts
# into one tree, and every configuration directory has the same name in both
# suites. Left at the top level, default_build_coverage/ from one suite would
# overwrite the other's on the published site. Nested here, the top level still
# holds exactly the one suite directory the deploy expects.
mkdir -p coverage_report/per_configuration/$1
gcovr -r "$repo_root" -f "$filter" "$objdir" \
      --json coverage_report/per_configuration/$1.json \
      --xml-pretty --output coverage_report/per_configuration/$1.xml
gcovr -r "$repo_root" -f "$filter" "$objdir" --html --html-details --output coverage_report/per_configuration/$1/index.html

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
