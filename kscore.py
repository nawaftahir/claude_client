#!/usr/bin/env python3
"""Score kmatch_ours.txt pairs: normalized insn similarity of our fn vs the client FUN_."""
import difflib, sys
from alldiff_lib import nxt, norm, ours, client, plain
byplain = {}
for m in nxt: byplain.setdefault(plain(m), m)
for l in open('../kmatch_ours.txt'):
    sc, sz, a, n = l.split()[:4]
    m = byplain.get(n)
    if not m: continue
    va, end = nxt[m]
    o = ours(va, end)
    a = int(a, 16)
    c = client(a, max(len(o), int(sz)))
    no, nc = norm(o, 0, len(o)), norm(c, 0, int(sz))
    r = difflib.SequenceMatcher(None, no, nc, autojunk=False).ratio()
    print('%.3f %5d %5d %x %s' % (r, len(o), int(sz), a, m))
