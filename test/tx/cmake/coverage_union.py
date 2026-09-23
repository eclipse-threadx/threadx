#!/usr/bin/env python3
"""Report coverage of the certified source, unioned across build configurations.

gcovr's --add-tracefile merge keys every branch by the basic-block pair gcov
assigned it. Those block numbers are not a property of the source: a file that
compiles to a different amount of code in two configurations gets its blocks
renumbered, so the same source branch arrives under two identities and the merge
counts it twice. Measured over the six configurations at the time this was
written, 521 of 1123 branch sites carried more branches in the merged report than
any single configuration had, and the merged denominator read 4627 where the
source carries 3156.

That inflation is symmetric for a branch covered everywhere, so the percentage
stays plausible and the fault is easy to miss. It is not harmless: the figure a
certification report quotes as "branches in the certified source" has to be
branches in the certified source, and the number grows or shrinks when a build
configuration is added for reasons that have nothing to do with the code.

This script reports the union instead. A line is covered if any configuration
executed it; the Nth branch on a line is covered if any configuration took it.
Branches are keyed by their position within the line rather than by block
number, which is stable across configurations because the order gcov emits them
in follows the source expression.

Both figures are kept. gcovr's merged report is still produced and published
unchanged -- it is the tool's own output and nothing here rewrites it. This is
the figure the coverage ratchet gates on, so that adding a build configuration
moves the number only by the code it actually brings in.

The union also drops the code the regression-test hook macros inject into the
certified source. See HOOK_SITES below for what that is and why it is not part
of the denominator; the excluded sites are printed with the figure, because an
exclusion nobody can see in the output is an exclusion nobody can audit.
"""

import glob
import json
import os
import sys


# The regression-test hook code, which is not part of the certified denominator.
#
# CMakeLists.txt puts -DTX_REGRESSION_TEST on every build configuration. That
# opens a block in the Linux port header, ports/linux/gnu/inc/tx_port.h lines
# 186-265, which defines eight *_EXTENSION macros that expand to live code --
# flag tests, state changes and calls -- at eight sites in common/src, carried
# by this list and the one after it. An application does not define the macro,
# tx_api.h's #ifndef fallbacks then expand every one of them to nothing, and
# none of this code exists in a build anyone ships. gcov attributes a macro
# expansion to the file that invokes it, so without this list the harness's own
# scaffolding is counted as certified source.
#
# Only a macro the *test build* defines belongs here. A macro the port header
# defines is in the application's build too, so removing its code would take
# shipped code out of the denominator -- the opposite error. The case to keep
# in mind is TX_TRACE_PORT_EXTENSION: the same header defines it,
# unconditionally, and it expands to a clock_gettime() inside tx_misra.c. It
# ships, so it stays.
#
# The line numbers are stable because the certified baseline is a frozen tag,
# and a site that moves is caught rather than silently skipped: every entry must
# match a line the report carries, or this fails.
HOOK_SITES = [
    ("common/src/tx_byte_allocate.c", 175, "TX_BYTE_ALLOCATE_EXTENSION"),
    ("common/src/tx_byte_release.c", 236, "TX_BYTE_RELEASE_EXTENSION"),
    ("common/src/tx_initialize_kernel_enter.c", 132, "TX_INITIALIZE_KERNEL_ENTER_EXTENSION"),
    ("common/src/tx_mutex_priority_change.c", 235, "TX_MUTEX_PRIORITY_CHANGE_EXTENSION"),
    ("common/src/tx_mutex_put.c", 349, "TX_MUTEX_PUT_EXTENSION_1"),
    ("common/src/tx_mutex_put.c", 600, "TX_MUTEX_PUT_EXTENSION_2"),
    ("common/src/tx_thread_stack_analyze.c", 145, "TX_THREAD_STACK_ANALYZE_EXTENSION"),
]

# One site where the hook displaces shipped code rather than occupying an empty
# line, so only its branches come out.
#
# Seven of the eight macros have an empty #ifndef fallback in tx_api.h, so
# without the hook their invocation line holds nothing at all and the whole line
# goes. TX_TIMER_INITIALIZE_EXTENSION is the exception: under TX_MISRA_ENABLE
# tx_api.h defines it as "status = _tx_misra_status_get((a));", a statement with
# no branches. A shipped misra build therefore still executes this line, and
# dropping it would remove shipped code from the denominator. The line stays and
# the hook's two branch outcomes go.
HOOK_BRANCH_SITES = [
    ("common/src/tx_timer_initialize.c", 275, "TX_TIMER_INITIALIZE_EXTENSION"),
]


def exclude_hook_sites(lines, branches):
    """Drop the hook expansions from the union, and report what was dropped.

    Returns one row per site: the macro, and the covered/total it took out of
    each axis. Covered/total rather than a count, so the output shows on its
    face that the exclusion removed nothing that was uncovered -- which would
    raise the figure for the wrong reason.
    """

    report = []
    missing = []

    for path, number, macro, drop_line in (
            [(p, n, m, True) for p, n, m in HOOK_SITES] +
            [(p, n, m, False) for p, n, m in HOOK_BRANCH_SITES]):

        outcomes = sorted(k for k in branches if k[0] == path and k[1] == number)
        if (path, number) not in lines:
            missing.append((path, number, macro))
            continue

        line_covered = line_total = 0
        if drop_line:
            line_covered, line_total = lines.pop((path, number)), 1

        outcome_covered = sum(branches.pop(k) for k in outcomes)
        report.append((path, number, macro, drop_line,
                       line_covered, line_total, outcome_covered, len(outcomes)))

    return report, missing


def union(tracefiles):
    """Union line and branch coverage across per-configuration tracefiles."""

    lines = {}
    branches = {}

    for path in tracefiles:
        with open(path, encoding="utf-8") as handle:
            data = json.load(handle)

        for entry in data.get("files", []):
            name = entry["file"]
            for line in entry.get("lines", []):
                number = line["line_number"]
                key = (name, number)
                lines[key] = lines.get(key, 0) or (1 if line["count"] > 0 else 0)

                for index, branch in enumerate(line.get("branches", [])):
                    key = (name, number, index)
                    branches[key] = branches.get(key, 0) or (1 if branch["count"] > 0 else 0)

    return lines, branches


def main():
    if len(sys.argv) < 4:
        print("usage: coverage_union.py <tracefile-dir> <min-line> <min-branch>", file=sys.stderr)
        return 2

    directory, min_line, min_branch = sys.argv[1], float(sys.argv[2]), float(sys.argv[3])

    tracefiles = sorted(glob.glob(os.path.join(directory, "*.json")))
    if not tracefiles:
        print("coverage_union.py: no JSON in %s." % directory, file=sys.stderr)
        print("Run the suites with TX_COVERAGE=ON first.", file=sys.stderr)
        return 1

    lines, branches = union(tracefiles)
    if not lines:
        print("coverage_union.py: the tracefiles contain no files.", file=sys.stderr)
        return 1

    excluded, missing = exclude_hook_sites(lines, branches)
    if missing:
        for path, number, macro in missing:
            print("coverage_union.py: %s:%d is not in the report -- %s has moved."
                  % (path, number, macro), file=sys.stderr)
        print("coverage_union.py: re-derive HOOK_SITES from the port header.", file=sys.stderr)
        return 1

    line_covered, line_total = sum(lines.values()), len(lines)
    branch_covered, branch_total = sum(branches.values()), len(branches)

    line_rate = (100.0 * line_covered) / line_total
    branch_rate = (100.0 * branch_covered) / branch_total if branch_total else 100.0

    print("coverage_union.py: unioned over %d configuration(s):" % len(tracefiles))
    for path in tracefiles:
        print("    %s" % os.path.basename(path)[:-len(".json")])

    print("    excluded, regression-test hook expansions (-DTX_REGRESSION_TEST):")
    hook_lines_covered = hook_lines_total = 0
    hook_outcomes_covered = hook_outcomes_total = 0
    for path, number, macro, drop_line, lc, lt, oc, ot in excluded:
        print("        %-36s lines %d/%d, outcomes %2d/%-2d  %s%s"
              % ("%s:%d" % (path[len("common/src/"):], number),
                 lc, lt, oc, ot, macro, "" if drop_line else ", branches only"))
        hook_lines_covered += lc
        hook_lines_total += lt
        hook_outcomes_covered += oc
        hook_outcomes_total += ot
    print("        %-36s lines %d/%d, outcomes %2d/%-2d"
          % ("total", hook_lines_covered, hook_lines_total,
             hook_outcomes_covered, hook_outcomes_total))

    print("    lines    %d/%d - %.2f%%" % (line_covered, line_total, line_rate))
    print("    branches %d/%d - %.2f%%" % (branch_covered, branch_total, branch_rate))

    status = 0
    if line_rate < min_line:
        print("coverage_union.py: failed minimum line coverage (got %.2f%%, minimum %.2f%%)"
              % (line_rate, min_line), file=sys.stderr)
        status = 1
    if branch_rate < min_branch:
        print("coverage_union.py: failed minimum branch coverage (got %.2f%%, minimum %.2f%%)"
              % (branch_rate, min_branch), file=sys.stderr)
        status = 1

    return status


if __name__ == "__main__":
    sys.exit(main())
