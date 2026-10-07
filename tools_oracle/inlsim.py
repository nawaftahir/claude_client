#!/usr/bin/env python3
"""Score edge-order variants by retail-inlined edges (inl_edges.tsv) that the DFS cuts.
usage (target/): inlsim.py ord map rsp inl_edges.tsv"""
import sys, re, itertools
sys.path.insert(0, __file__.rsplit('/', 1)[0])
import sim
ordf, mp, rsp, ie = sys.argv[1:5]
g = sim.graph(ordf); nodes = sim.order(rsp, sim.fnobj(mp), None)
base = lambda n: re.sub(r'^[?_]', '', n).split('@')[0]
want = set()
for l in open(ie):
    c, k, v = l.rstrip('\n').split('\t'); want.add((c, k))
def score(g):
    cuts, _ = sim.dfs(g, nodes)
    bad = [(a, b) for a, b in cuts if (a, base(b)) in want]
    return len(bad), len(cuts), set(cuts)
b0, n0, c0 = score(g)
print('base bad', b0, 'cuts', n0)
n = '?UI_LoadArenas@@YAXXZ'
E = g[n]
for p in [[x] + [y for y in E if y != x] for x in E]:
    g2 = dict(g); g2[n] = list(p)
    b, nc, c = score(g2)
    print(b, nc, '+%d -%d' % (len(c - c0), len(c0 - c)), [base(x) for x in p])
