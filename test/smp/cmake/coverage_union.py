#!/usr/bin/env python3
"""Report coverage of the certified source, unioned across build configurations.

gcovr's --add-tracefile merge keys every branch by the basic-block pair gcov
assigned it. Those block numbers are not a property of the source: a file that
compiles to a different amount of code in two configurations gets its blocks
renumbered, so the same source branch arrives under two identities and the merge
counts it twice. Measured over the seven configurations at the time this was
written, 1265 of 1343 branch sites carried more branches in the merged report than
any single configuration had, and the merged denominator read 6334 where the
source carries 3596.

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
"""

import glob
import json
import os
import sys


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

    line_covered, line_total = sum(lines.values()), len(lines)
    branch_covered, branch_total = sum(branches.values()), len(branches)

    line_rate = (100.0 * line_covered) / line_total
    branch_rate = (100.0 * branch_covered) / branch_total if branch_total else 100.0

    print("coverage_union.py: unioned over %d configuration(s):" % len(tracefiles))
    for path in tracefiles:
        print("    %s" % os.path.basename(path)[:-len(".json")])
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
