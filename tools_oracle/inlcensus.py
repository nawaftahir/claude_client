#!/usr/bin/env python3
"""Census of call-site differences over unmatched pairs (alldiff rows): callees ours calls where retail
inlines (+ours) and vice versa (+retail). usage (from target/): inlcensus.py exe map cmp.json scored.txt"""
import sys, re, json, sys as _s
from collections import Counter
_s.argv, a = _s.argv[:1] + ['x', 'x', 'x', 'x', '0', '0'], _s.argv
exe, mp, cj, sc = a[1:5]
EDGES = open(a[5], 'w') if len(a) > 5 else None
src = open(__file__.replace('inlcensus.py', 'calls.py')).read().split('names = {}')[0]
exec(src)
exe, mp, cj, sc = a[1:5]
names = {}
for l in open(mp):
    m = re.match(r' 0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8}) f', l)
    if m: names[int(m.group(2), 16)] = m.group(1)
byname = {v: k for k, v in names.items()}
rn = {}
for f in json.load(open(cj)):
    for h in f['hits']: rn[int(h, 16)] = f['name']
def base(n): return re.sub(r'^[?_]', '', n).split('@')[0]
names = {k: base(v) for k, v in names.items()}
rn = {k: base(v) for k, v in rn.items()}
for t in ('../ghidra/names_fix.tsv', '../ghidra/names_k.tsv', '../ghidra/names_1.0.tsv'):
    for l in open(t):
        f = l.split('\t')
        try: rn.setdefault(int(f[0], 16), f[1].strip())
        except (ValueError, IndexError): pass
po, pr = pe(exe), pe(RET)
plus_o, plus_r, where = Counter(), Counter(), {}
seen = set()
for l in open(sc):
    d, size, ca, nm = l.split()[:4]
    if nm not in byname or ca in seen: continue
    seen.add(ca)
    o = Counter(names.get(t, hex(t)) for t in calls(po, byname[nm], int(size)))
    r = Counter(rn.get(t, hex(t)) for t in calls(pr, int(ca, 16), int(size)))
    if sum(o.values()) == sum(r.values()): continue  # same call count: naming noise
    for k, v in (o - r).items():
        plus_o[k] += v; where.setdefault(k, []).append(nm[:30])
        if EDGES: EDGES.write('%s\t%s\t%d\n' % (nm, k, v))
    for k, v in (r - o).items():
        plus_r[k] += v; where.setdefault(k, []).append(nm[:30])
        if EDGES: EDGES.write('%s\t%s\t%d\tR\n' % (nm, k, v))
for k in set(plus_o) | set(plus_r):
    if k.startswith('0x'): continue
    print(plus_o[k], plus_r[k], k[:60], ' '.join(sorted(set(where[k]))[:6]))
