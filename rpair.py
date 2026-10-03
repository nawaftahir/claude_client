#!/usr/bin/env python3
"""Reverse pairing: for each unclaimed retail fn start (>= minsize), find the best-matching ours fn by
normalized diff among size-similar candidates; flags ours fns whose current pair is worse.
usage (in target/): rpair.py <alldiff_scored.txt> [minsize] -> retail_addr size best_diff ourname cur_diff cur_addr"""
import sys, json, difflib, bisect
sys.path.insert(0, '..')
from alldiff_lib import nxt, ours, client, norm, cstarts
r = json.load(open('build/cmp.json'))
claimed = {int(h, 16) for x in r for h in x['hits']}
cs = cstarts(); sz = {a: b - a for a, b in zip(cs, cs[1:])}
cur = {}
for l in open(sys.argv[1]):
    p = l.split()
    if len(p) >= 4: cur[p[3]] = (int(p[0]), p[2])
minsize = int(sys.argv[2]) if len(sys.argv) > 2 else 200
pool = []
for name, (va, end) in nxt.items():
    if cur.get(name, (1,))[0] == 0: continue
    oc = ours(va, end)
    if len(oc) >= 32: pool.append((len(oc), name, va, oc))
pool.sort(key=lambda t: t[0]); keys = [t[0] for t in pool]
nc = {}
for c in cs:
    s = sz.get(c, 0)
    if c in claimed or s < minsize: continue
    b = client(c, s).rstrip(b'\xcc'); B = norm(b, c, len(b))
    lo, hi = bisect.bisect_left(keys, int(len(b) * 0.85)), bisect.bisect_right(keys, int(len(b) * 1.15) + 8)
    best = None
    for L, name, va, oc in pool[lo:hi]:
        A = nc.get(name)
        if A is None: A = nc[name] = norm(oc, va, len(oc))
        sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
        if sm.quick_ratio() < 0.8: continue
        n = sum(1 for l in difflib.unified_diff(A, B, n=0, lineterm='') if l[:1] in '+-' and l[:3] not in ('+++', '---'))
        if best is None or n < best[0]: best = (n, name)
    if best and best[0] < max(8, len(B) // 3):
        cd, ca = cur.get(best[1], (-1, '-'))
        print(hex(c), len(b), best[0], best[1], cd, ca, flush=True)
