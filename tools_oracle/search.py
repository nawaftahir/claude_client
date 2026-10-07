#!/usr/bin/env python3
"""Greedy search: under an obj order, find "declare callee c first in TU t" moves that bring the simulated
cut set closest to a target cut set. usage: search.py ord.txt map rsp target_cuts.txt steps -> log lines"""
import sys; sys.path.insert(0, __file__.rsplit('/', 1)[0])
from sim import graph, fnobj, order, dfs
og, mp, rsp, tgt, steps = sys.argv[1:6]
g = graph(og); fo = fnobj(mp); nodes = order(rsp, fo, None)
T = set(tuple(l.rstrip('\n').split(' -> ')) for l in open(tgt))
def score(gg):
    c = set(dfs(gg, nodes)[0]); return len(c & T) - len(c - T), len(c & T), len(c)
tus = {}
for n in g:
    if n in fo: tus.setdefault(fo[n][0], []).append(n)
def moved(gg, tu, c):
    h = dict(gg)
    for n in tus[tu]:
        if c in h[n] and h[n][0] != c:
            e = list(h[n]); e.remove(c); h[n] = [c] + e
    return h
cur = score(g); print('start', cur, flush=True)
for step in range(int(steps)):
    best = None
    for tu, ns in tus.items():
        cands = {c for n in ns for c in g[n][1:]}
        for c in cands:
            s = score(moved(g, tu, c))
            if s[0] > cur[0] and (best is None or s[0] > best[0][0]): best = (s, tu, c)
    if not best: print('no improving move'); break
    cur, tu, c = best; g = moved(g, tu, c)
    print('step', step, cur, tu, c, flush=True)
