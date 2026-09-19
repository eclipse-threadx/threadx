#!/bin/bash

set -e

cd $(dirname $0)

# Both -r and -f have to be absolute. "-r ../../.. -f common_smp/src" produces a
# report containing zero files and exits 0, which is the worst failure mode
# available here: a green run carrying an empty report. Rooting every
# configuration at the repository root rather than at its own build directory
# also makes the file names repo-relative, so the same source file is named
# identically in every configuration's report -- which is what lets --merge
# below union them at all.
repo_root=$(cd ../../.. && pwd)
filter=$repo_root/common_smp/src

# The coverage ratchet. CI fails below these, and they are raised as the gaps
# close, so a change that loses coverage fails here rather than in review. They
# are deliberately not 100%: a gate nothing can pass gets turned off, and the
# point is to hold the floor while it rises.
#
# The gate reads the union figure from coverage_union.py, not the percentage in
# the merged report. gcovr's merge keys each branch by the basic-block pair gcov
# gave it, and those numbers shift when a file compiles to a different amount of
# code, so the same source branch is counted once per configuration that
# renumbers it. Both figures are produced and the merged report is published
# unchanged; the gate uses the union because it is the one that counts branches
# in the source. Over the eight configurations the merged denominator reads 6595
# where the source carries 3610, and adding misra_trace_build alone moved the
# merged figure by 994 branches against the dozen the source really gained.
#
# The branch gate is far below the 83.00 that preceded it and no coverage was
# lost doing it. 83.00 was a percentage of 4773 counted branch instances; this is
# a percentage of the 3610 branches that exist in common_smp/src.
#
# The margin below the measured figure is not slack for regressions. The same
# unchanged tree does not measure identically twice, on either axis.
#
# Branches move by three outcomes and five are unstable over seven CI runs on an
# unchanged tree: ordinals 8 and 10 of the twelve-branch TX_TRACE_IN_LINE_INSERT
# expansion in tx_trace_isr_enter_insert.c and tx_trace_isr_exit_insert.c, which
# are the buffer-wrap arm reached by coincidence rather than on purpose, and the
# timer list wrap in tx_timer_info_get.c. Those seven runs and two clean local
# ones read 3114, 3115, 3115, 3115, 3116, 3116, 3116, 3117 and 3117 of 3610.
#
# That is down from eleven unstable outcomes and a spread of eight. The thread
# trace test closed eight of the eleven -- the same two ordinals on the thread
# resume, thread suspend, system resume and system suspend inserts -- and they are
# now covered in every run rather than in some. The two in
# tx_trace_isr_enter_insert.c came back the other way, from uncovered in seven
# consecutive runs to unstable, which is the same lesson from the other side: what
# a sample calls stable is a property of the sample.
#
# Lines move by two: tx_timer_info_get.c:164 and :166 are hit in most runs and
# missed in some. The same nine runs read 5394 and 5396 of 5450.
#
# The gate sits ten lines and eleven outcomes below the worst observation. The
# floor a run can honestly reach is the set covered in every run -- 5394 lines and
# 3113 outcomes -- and the gate is ten below each of those, which is headroom for
# the outcomes a later sample reclassifies rather than slack for a regression.
min_line=${TX_COVERAGE_MIN_LINE:-98.78}
min_branch=${TX_COVERAGE_MIN_BRANCH:-85.95}

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
objdir=$PWD/build/$1/threadx_smp/CMakeFiles/threadx_smp.dir$repo_root/common_smp/src

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
