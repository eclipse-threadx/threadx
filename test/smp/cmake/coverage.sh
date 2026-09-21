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
# in the source. Over the eight configurations the merged denominator reads more
# than 6500 where the source carries 3612, and adding misra_trace_build alone
# moved the merged figure by 994 branches against the dozen the source really
# gained.
#
# The branch gate is far below the 83.00 that preceded it and no coverage was
# lost doing it. 83.00 was a percentage of 4773 counted branch instances; this is
# a percentage of the 3612 branches that exist in common_smp/src.
#
# The branch denominator is 3612 rather than 3610 from the fault-injection sweep
# onwards. TX_BYTE_ALLOCATE_EXTENSION in the Linux SMP port gained a second
# clause, and the macro expands inside tx_byte_allocate.c, so the clause's `if`
# is certified-source branch and is counted. Both of its outcomes are covered,
# and nothing else in ports_smp enters the denominator -- the gcovr -f filter
# excludes it.
#
# The margin below the measured figure is not slack for regressions. It is there
# because the same unchanged tree has not always measured identically twice.
#
# The line axis has been closed for seven consecutive samples, forty-nine CI
# runs: every line is covered in all seven runs of a sample or in none of them.
#
# The branch axis is closed for a second consecutive sample. All seven runs read
# 3588 of 3612, with no spread at all. The seven samples read 3194, 3195, 3195,
# 3195, 3197, 3199, 3199 -- then 3194, 3197 x6 -- then 3195, 3195, 3195, 3197,
# 3197, 3197, 3197 -- then 3395, 3395, 3397, 3397, 3397, 3399, 3399 -- then 3527,
# 3527, 3529, 3529, 3529, 3529, 3531 -- then 3583 x7 -- then 3588 x7. The
# always-covered total went 3194, 3194, 3195, 3395, 3527, 3583, 3588.
#
# One outcome in this figure is reached by two cores racing rather than by a
# single thread doing something in order, and it is the only one in the suite
# that is. tx_trace_user_event_insert.c:101's null arm needs the buffer pointer
# cleared between the service's own check of it and the insert's, which is what
# a tx_trace_disable on another core does -- that service takes no lock at all.
# The test drives 300,000 inserts against a writer on a core of its own and
# takes the arm 49 to 132 times on this workstation and 521 to 1298 times on a
# CI runner, measured over three and seven runs. It is counted in the
# always-covered set on that evidence, not on one sample.
#
# So the unstable set is empty: zero outcomes and zero lines, in two consecutive
# samples on two different trees.
#
# The margin stays anyway, and the reason is not the size of the set today. Five
# earlier samples moved an outcome in every direction there is: unstable to
# never, never back to unstable, always to unstable, and unstable to always and
# back again -- that last one twice, on the ISR insert pair, which one sample
# recorded as closed and the next two found moving again. A set measured empty
# twice is a thing two samples saw. An outcome is closed when a test reaches it
# on purpose.
#
# The evidence step settled whether the margin can go, and it stays. The reason
# is measured on this tree rather than carried forward: the union is stable
# because the eight configurations cover for each other, not because the suite
# is deterministic. Read per configuration over sixteen runs, eight branch
# outcomes and four lines move between runs inside at least one configuration,
# seven of the eight configurations carry at least one mover, and the largest
# movement in any one configuration is eight outcomes, in
# stack_checking_rand_fill_build. The union absorbs all of it.
#
# What the union owes that redundancy is visible in the report published beside
# it. gcovr's merged figure reads 6561 to 6564 of 6601 over the same runs, where
# the union reads 3588 of 3612 in every one, because block-identity keying
# cannot absorb a miss in one configuration when another covers the same source
# construct.
#
# Thirty-five outcomes in the figure are compiled by two configurations and
# covered by only one of them in at least one run. Almost all are
# TX_THREAD_STACK_CHECK fence ordinals that stack_checking_build holds alone,
# because its random-fill sibling fills the stack with values that are not
# TX_STACK_FILL and short-circuits before reaching them. A further 894 outcomes
# are compiled by exactly one configuration at all -- 872 of them trace_build's
# trace macros -- which is structural rather than fragile: a construct that
# exists in one configuration is covered there or nowhere. The exception is the
# racing outcome above, the one member of that set whose single carrier covers
# it by winning a race rather than by executing in order.
#
# The gate is therefore still set ten lines and ten outcomes below the set
# covered in *every* run -- 5449 lines and 3588 outcomes -- rather than below the
# lowest total any run reported. The two coincide in the last two samples and
# have not in any earlier one. Ten is a measured size now rather than an
# inherited one: it covers the largest per-configuration movement observed,
# eight outcomes, with headroom, against a set that has no second configuration
# to fall back on.
min_line=${TX_COVERAGE_MIN_LINE:-99.79}
min_branch=${TX_COVERAGE_MIN_BRANCH:-99.05}

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
