#!/usr/bin/env python3
"""Greedy decl-order search: minimise retail-inlined edges (inlcensus edges tsv) that the DFS cuts.
score = 10*bad + new cuts whose caller is matched now (risk). A move = declare callee c first in TU t.
usage: search3.py ord.txt map rsp edges.tsv cmp.json steps [tu:callee ...pre-applied moves]"""
import sys, re, json; sys.path.insert(0, __file__.rsplit('/', 1)[0])
from sim import graph, fnobj, order, dfs
og, mp, rsp, ie, cj, steps = sys.argv[1:7]
g = graph(og); fo = fnobj(mp); nodes = order(rsp, fo, None)
base = lambda n: re.sub(r'^[?_]', '', n).split('@')[0]
want, wcut = set(), set()
for l in open(ie):
    f = l.rstrip('\n').split('\t')
    (wcut if len(f) > 3 else want).add((f[0], f[1]))
matched = {f['name'] for f in json.load(open(cj)) if f['hits']}
tus = {}
for n in g:
    if n in fo: tus.setdefault(fo[n][0], []).append(n)
def moved(gg, tu, c):
    h = dict(gg)
    for n in tus[tu]:
        if c in h[n] and h[n][0] != c:
            e = list(h[n]); e.remove(c); h[n] = [c] + e
    return h
for m in sys.argv[7:]:
    tu, c = m.split(':', 1); g = moved(g, tu, c)
c0 = set(dfs(g, nodes)[0])
def score(gg):
    cuts = set(dfs(gg, nodes)[0])
    bad = sum(1 for a, b in cuts if (a, base(b)) in want) + len(wcut) - sum(1 for a, b in cuts if (a, base(b)) in wcut)
    risk = sum(1 for a, b in cuts - c0 if a in matched)
    return 10 * bad + risk, bad, risk, len(cuts)
cur = score(g); print('start', cur, flush=True)
for step in range(int(steps)):
    best = None
    for tu, ns in tus.items():
        for c in {c for n in ns for c in g[n][1:]}:
            s = score(moved(g, tu, c))
            if s[0] < cur[0] and (best is None or s[0] < best[0][0]): best = (s, tu, c)
    if not best: print('no improving move'); break
    cur, tu, c = best; g = moved(g, tu, c); c0 = set(dfs(g, nodes)[0])
    print('step', step, cur, tu, c, flush=True)
