#!/usr/bin/env python3
"""datadiff.py dmap.tsv [N]: data layout inventory from datamap.py output.
ORDER = mapped syms outside the longest run in retail order (misplaced);
SIZE  = neighbours in both layouts whose size differs (retail size from the next sym)."""
import bisect, sys

rows = [l.rstrip('\n').split('\t') for l in open(sys.argv[1])]
N = int(sys.argv[2]) if len(sys.argv) > 2 else 20
rows = [(int(r[0], 16), int(r[1], 16), int(r[2]), r[6], r[7]) for r in rows]
SECS = (('rdata', 0x559000, 0x58a000), ('data', 0x58a000, 0x1836000))


def lis(seq):
    # indices of a longest increasing subsequence
    tails, prev, idx = [], [-1] * len(seq), []
    for i, v in enumerate(seq):
        k = bisect.bisect_left(tails, v)
        if k == len(tails):
            tails.append(v); idx.append(i)
        else:
            tails[k] = v; idx[k] = i
        prev[i] = idx[k - 1] if k else -1
    out, i = set(), idx[-1] if idx else -1
    while i >= 0:
        out.add(i); i = prev[i]
    return out


for nm, lo, hi in SECS:
    rs = [r for r in rows if lo <= r[0] < hi]
    keep = lis([r[1] for r in rs])
    bad = [rs[i] for i in range(len(rs)) if i not in keep]
    good = [rs[i] for i in sorted(keep)]
    size = []
    for a, b in zip(good, good[1:]):
        if b[1] == a[1] + a[2] and b[0] - a[0] != a[2]:
            size.append((b[0] - a[0] - a[2], a))
    adj = sum(1 for a, b in zip(rs, rs[1:]) if b[1] - a[1] == b[0] - a[0])
    print('%s: mapped %d, same-gap neighbours %d, misplaced %d (%d B), size diffs %d (net %+d B)' % (
        nm, len(rs), adj, len(bad), sum(r[2] for r in bad), len(size), sum(d for d, _ in size)))
    for d, a in sorted(size, key=lambda t: -abs(t[0]))[:N]:
        print('  SIZE %+8d  %-50s ours %d  %s' % (d, a[3][:50], a[2], a[4]))
    for r in sorted(bad, key=lambda r: -r[2])[:N // 2]:
        print('  ORDER %7d  %-50s %s' % (r[2], r[3][:50], r[4]))
