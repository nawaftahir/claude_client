#!/usr/bin/env python3
"""Greedy decl-order search scored by retail convention evidence (rconv): a cut target keeps the stack
convention. score = |cut targets ∩ want_stack| - |cut targets ∩ retail_reg|.
usage: search2.py ord.txt map rsp rconv_retail.json rconv_ours.json steps"""
import sys, json; sys.path.insert(0, __file__.rsplit('/', 1)[0])
from sim import graph, fnobj, order, dfs
og, mp, rsp, rj, oj, steps = sys.argv[1:7]
g = graph(og); fo = fnobj(mp); nodes = order(rsp, fo, None)
R, O = json.load(open(rj)), json.load(open(oj))
cur_t = {b for a, b in dfs(g, nodes)[0]}
want = {n for n, c in R.items() if c == 'stack' and n in g and (O.get(n) == 'reg' or n in cur_t)}
bad = {n for n, c in R.items() if c == 'reg' and n in g}
def score(gg):
    t = {b for a, b in dfs(gg, nodes)[0]}
    return len(t & want) - len(t & bad), sorted(x.split('@@')[0] for x in t & want), sorted(x.split('@@')[0] for x in t & bad)
tus = {}
for n in g:
    if n in fo: tus.setdefault(fo[n][0], []).append(n)
def moved(gg, tu, c):
    h = dict(gg)
    for n in tus[tu]:
        if c in h[n] and h[n][0] != c:
            e = list(h[n]); e.remove(c); h[n] = [c] + e
    return h
cur = score(g); print('start', cur[0], 'want', len(want), 'hit', cur[1], 'bad', cur[2], flush=True)
for step in range(int(steps)):
    best = None
    for tu, ns in tus.items():
        for c in {c for n in ns for c in g[n][1:]}:
            s = score(moved(g, tu, c))
            if s[0] > cur[0] and (best is None or s[0] > best[0][0]): best = (s, tu, c)
    if not best: print('no improving move'); break
    cur, tu, c = best; g = moved(g, tu, c)
    print('step', step, cur[0], tu, c, '| hit', cur[1], '| bad', cur[2], flush=True)
