#!/usr/bin/env python3
"""Which node does an address-order DFS enter a call-graph SCC by? (LTCG bottom-up model)
usage (from target/): sccentry.py <retailVA> <ourVA>   (any member of the SCC on each side)
Builds the post-inline call graph (call/jmp to function starts) of each binary, roots in address
order, children in call-site order, and prints the SCC size, the entry node and the root path."""
import os, re, sys
sys.setrecursionlimit(100000)
from alldiff_lib import md, d, base, cd, cbase, syms
here = os.path.dirname(os.path.abspath(__file__))
rnames = {}
for f in ('names_fix.tsv', 'names_k.tsv', 'names_1.0.tsv', 'names_cg.tsv', 'names_loc.tsv'):
    for l in open(os.path.join(here, 'ghidra', f)):
        r = l.rstrip('\n').split('\t')
        if len(r) >= 2 and re.fullmatch(r'[0-9a-fA-F]+', r[0]):
            rnames.setdefault(int(r[0], 16), r[1])
rstarts = sorted({int(m.group(1), 16) for f in os.listdir(os.path.join(here, 'ghidra', 'out'))
                  for m in [re.match(r'([0-9a-f]{8})_', f)] if m})
def calltargets(code, cb):
    t = set()
    for i in md.disasm(code, cb):
        if i.mnemonic == 'call' and i.op_str.startswith('0x'):
            v = int(i.op_str, 16)
            if cb <= v < cb + len(code): t.add(v)
    return t
rstarts = sorted(set(rstarts) | calltargets(cd, cbase))
ostarts = sorted({a for a, _ in syms})
onames = {a: n for a, n in syms}

def graph(code, cb, starts):
    ss = set(starts)
    g = {}
    for k, a in enumerate(starts):
        e = starts[k + 1] if k + 1 < len(starts) else a + 16
        out = []
        for i in md.disasm(code[a - cb:e - cb], a):
            if i.mnemonic in ('call', 'jmp') and i.op_str.startswith('0x'):
                t = int(i.op_str, 16)
                if t in ss and t != a and (i.mnemonic == 'call' or not (a <= t < e)):
                    if t not in out:
                        out.append(t)
        g[a] = out
    return g

def tarjan(g, v0):
    idx, low, st, on, comp, c = {}, {}, [], set(), {}, [0]
    def sc(v):
        idx[v] = low[v] = c[0]; c[0] += 1; st.append(v); on.add(v)
        for w in g.get(v, ()):
            if w not in idx:
                sc(w); low[v] = min(low[v], low[w])
            elif w in on:
                low[v] = min(low[v], idx[w])
        if low[v] == idx[v]:
            s = set()
            while True:
                w = st.pop(); on.discard(w); s.add(w)
                if w == v: break
            for w in s: comp[w] = s
    for v in g:
        if v not in idx: sc(v)
    return comp[v0]

def entry(g, starts, scc):
    seen = set()
    def dfs(v, path):
        seen.add(v)
        if v in scc: return path + [v]
        for w in g.get(v, ()):
            if w not in seen:
                r = dfs(w, path + [v])
                if r: return r
        return None
    for r in starts:
        if r not in seen:
            p = dfs(r, [])
            if p: return p

import json
SC = {}
for label, code, cb, starts, nm, v in (('retail', cd, cbase, rstarts, rnames, int(sys.argv[1], 16)),
                                        ('ours', d, base, ostarts, onames, int(sys.argv[2], 16))):
    g = graph(code, cb, starts)
    scc = tarjan(g, v)
    SC[label] = (g, scc)
    p = entry(g, starts, scc)
    show = lambda a: '%x %s' % (a, nm.get(a, '?')[:60])
    print('%s: SCC %d nodes; entry %s' % (label, len(scc), show(p[-1])))
    for a in p[-6:]:
        print('   ', show(a))

if os.environ.get('SCCDIFF'):
    o2r = {}
    for e in json.load(open('build/cmp.json')):
        oa = [a for a, n in syms if n == e['name']]
        if oa and e['hits']:
            o2r[oa[0]] = int(e['hits'][0], 16)
    rn = {n: a for a, n in rnames.items()}
    for a, n in syms:
        if a not in o2r:
            p = re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', n)
            if p in rn: o2r[a] = rn[p]
    og, oscc = SC['ours']; rg, rscc = SC['retail']
    rows = []
    for a in oscc:
        r = o2r.get(a)
        if r is not None and r not in rscc:
            rows.append((a, r, onames.get(a, '?')))
    print('ours-in-SCC, retail-not:', len(rows), 'unmapped:', sum(1 for a in oscc if a not in o2r))
    for a, r, n in sorted(rows, key=lambda x: x[2])[:400]:
        print('%x %x %s' % (a, r, n[:70]))
