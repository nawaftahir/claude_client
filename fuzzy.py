#!/usr/bin/env python3
"""Score unmatched fns with no client name against unclaimed client fn starts near their neighbour-located address.
usage: fuzzy.py (run in target/ after make measure) -> diffs size addr name"""
import difflib, json, sys
sys.path.insert(0, '..')
import alldiff_lib as L
from locate import locate
r = json.load(open('build/cmp.json'))
claimed = {int(h, 16) for x in r for h in x['hits']}
starts = sorted(a for a in L.cstarts() if a not in claimed)
import bisect
out = []
for x in r:
    if x['hits'] or x['name'] not in L.nxt or L.names.get(L.plain(x['name'])) is not None: continue
    try: a = locate('build/cmp.json', x['name'])
    except StopIteration: a = None
    if a is None: continue
    va, end = L.nxt[x['name']]
    ours = L.ours(va, end)
    A = L.norm(ours, va, len(ours))
    best = None
    for c in starts[bisect.bisect_left(starts, a - 0x3000):bisect.bisect_right(starts, a + 0x3000)]:
        B = L.norm(L.client(c, len(ours) + 64), c, len(ours))
        if abs(len(B) - len(A)) > max(4, len(A) // 8): continue
        n = sum(1 for l in difflib.unified_diff(A, B, n=0, lineterm='') if l[:1] in '+-' and l[:3] not in ('+++', '---'))
        if best is None or n < best[0]: best = (n, c)
    if best: out.append((best[0], len(ours), best[1], x['name']))
for o in sorted(out): print(o[0], o[1], hex(o[2]), o[3])
