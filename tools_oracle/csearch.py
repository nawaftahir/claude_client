#!/usr/bin/env python3
"""Decl-move search: put a DFS cut into each target callee (ours reg, retail stack).
A move (t, y) = declare y first in TU t (y goes first in every edge list of t's fns).
Candidates: for each caller p of target c that c reaches, every DFS ancestor a of p and each child y
of a that reaches c. Score = unmatched caller bytes of fixed targets - matched caller bytes of
callees whose cut status flips (collateral).
usage: csearch.py ord.txt map rsp cmp.json target..."""
import sys, json
sys.path.insert(0, __file__.rsplit('/', 1)[0])
from sim import graph, fnobj, order
sys.setrecursionlimit(100000)
og, mp, rsp, cj = sys.argv[1:5]
targets = sys.argv[5:]
g = graph(og); fo = fnobj(mp); nodes = order(rsp, fo, None)
tu = {n: fo[n][0] for n in fo}
cmp = {f['name']: f for f in json.load(open(cj))}
callers = {}
for p, cs in g.items():
    for c in cs: callers.setdefault(c, set()).add(p)

def run(gr):
    st, par, cutin = {}, {}, set()
    def visit(n):
        st[n] = 2
        for c in gr.get(n, ()):
            s = st.get(c, 0)
            if s == 2: cutin.add(c)
            elif s == 0 and c in gr: par[c] = n; visit(c)
        st[n] = 1
    for n in nodes + list(gr):
        if n in gr and not st.get(n): visit(n)
    return cutin, par

def move(gr, t, y):
    h = dict(gr)
    for f, cs in gr.items():
        if tu.get(f) == t and y in cs: h[f] = [y] + [x for x in cs if x != y]
    return h

def reach(start, nxt):
    seen, todo = {start}, [start]
    while todo:
        for m in nxt(todo.pop()):
            if m not in seen: seen.add(m); todo.append(m)
    return seen

def cbytes(c, matched):
    return sum(cmp[p]['size'] for p in callers.get(c, ()) if p in cmp and bool(cmp[p]['hits']) == matched)

base, par = run(g)
print('base cuts-in', len(base))
for c in targets:
    if c in base: print('ALREADY', c); continue
    fwd = reach(c, lambda n: g.get(n, ()))
    back = reach(c, lambda n: callers.get(n, ()))
    ps = [p for p in callers.get(c, ()) if p in fwd]
    if not ps: print('NOPATH', c); continue
    cand = set()
    for p in ps:
        a = p
        while a in par:
            a = par[a]
            for y in g.get(a, ()):
                if y in back and a in tu: cand.add((tu[a], y))
    res = []
    for t, y in cand:
        ci, _ = run(move(g, t, y))
        if c not in ci: continue
        flips = (ci ^ base) - {c}
        loss = sum(cbytes(f, True) for f in flips)
        res.append((cbytes(c, False) - loss, t, y, len(flips), sorted(flips)[:4]))
    res.sort(reverse=True)
    print('==', c, len(cand), 'cands', len(res), 'hits')
    for r in res[:4]: print('  ', r[0], r[1], r[2], 'flips', r[3], r[4])
