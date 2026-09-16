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

# The coverage gate. CI fails below these, and they are the figures the tree
# achieves: every line of common/src, and every branch outcome but the twenty
# defensive ones no test can reach, which are carried as documented deviations.
#
# There is no margin below the measurement. A gate below what the tree achieves
# is a gate a regression passes, and none is needed here: the same seven
# configurations produce the same figure on a workstation and on a clean runner.
#
# The branch figure is 3150 of 3170 outcomes, 99.369%, written here to the two
# decimal places the report prints. That reads a hundredth of a point low, which
# is under a third of one outcome, so losing a single branch still fails.
#
# The denominator is the whole of common/src, over every build configuration.
#
# The gate reads the union figure from coverage_union.py, not the percentage in
# the merged report. gcovr's merge keys each branch by the basic-block pair gcov
# gave it, and those numbers shift when a file compiles to a different amount of
# code, so the same source branch is counted once per configuration that
# renumbers it. Both figures are produced and the merged report is published
# unchanged; the gate uses the union because it is the one that counts
# branches in the source, and therefore the one that does not lurch when a
# configuration is added. Adding misra_trace_build moves the merged branch
# denominator by roughly 830 and the union by the dozen outcomes it really
# brings in.
min_line=${TX_COVERAGE_MIN_LINE:-100.00}
min_branch=${TX_COVERAGE_MIN_BRANCH:-99.36}

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
    # reports that explain it.
    python3 "$(dirname "$(realpath "$0")")/coverage_union.py" \
            coverage_report/per_configuration "$min_line" "$min_branch" || exit $?
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
