#!/usr/bin/env python3
"""Re-pair poorly matching named fns: for each ours fn whose named-pair diff ratio is high, find an
unclaimed retail fn start of similar size with a much smaller normalized diff (wrong-DB-address hunt).
usage (in target/): repair.py <alldiff_scored.txt> -> cur best size ourname oldaddr newaddr"""
import sys, json, difflib, bisect
sys.path.insert(0, '..')
from alldiff_lib import nxt, ours, client, norm, cstarts
r = json.load(open('build/cmp.json'))
claimed = {int(h, 16) for x in r for h in x['hits']}
cs = cstarts(); free = [a for a in cs if a not in claimed]
sz = {a: b - a for a, b in zip(cs, cs[1:])}
for l in open(sys.argv[1]):
    p = l.split()
    if len(p) < 4: continue
    dl, size, ca, name = int(p[0]), int(p[1]), int(p[2], 16), p[3]
    if name not in nxt or size < 48: continue
    n_ins = size // 3
    if dl < max(12, n_ins * 0.25): continue
    va, end = nxt[name]; oc = ours(va, end)
    a = norm(oc, va, len(oc))
    best = (dl, ca)
    for c in free:
        s = sz.get(c, 0)
        if c == ca or not (0.85 * len(oc) <= s <= 1.25 * len(oc) + 16): continue
        b = norm(client(c, s), c, s)
        sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
        if sm.real_quick_ratio() < 0.8 or sm.quick_ratio() < 0.8: continue
        dd = sum(max(i2 - i1, j2 - j1) for t, i1, i2, j1, j2 in sm.get_opcodes() if t != 'equal') * 2
        if dd < best[0]: best = (dd, c)
    if best[1] != ca and best[0] < dl * 0.5:
        print(dl, best[0], size, name, hex(ca), hex(best[1]), flush=True)
