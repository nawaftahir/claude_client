#!/usr/bin/env python3
"""Rank near-misses by unmatched retail helpers called only from that function (likely hand-inlined in ours)."""
import bisect, collections, os, re, sys
import capstone, vote
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.skipdata = True
starts = sorted(int(m.group(1), 16) for f in os.listdir('../ghidra/out') for m in [re.match(r'([0-9a-f]{8})_', f)] if m)
pe, cd, cbase = vote.text('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
callers = collections.defaultdict(set)
for i in md.disasm(cd, cbase):
    if i.mnemonic == 'call' and i.op_str.startswith('0x'):
        t = int(i.op_str, 16); callers[t].add(starts[bisect.bisect_right(starts, i.address) - 1])
ours = {n: (len(code), len(set(t for i in md.disasm(code, va) if i.mnemonic == 'call' and i.op_str.startswith('0x') for t in [int(i.op_str, 16)]))) for n, obj, va, code, mask in vote.functions('build/CoD2MP_s.exe', 'build/CoD2MP_s.map')}
for l in open(sys.argv[1]):
    d, sz, ca, m = l.split()[:4]
    ca = int(ca, 16)
    for t in sorted(callers):
        pass
    break
import json
cn = {}
for f in ('../ghidra/names_cg.tsv', '../ghidra/names_1.0.tsv'):
    for l in open(f):
        a, n = l.split('\t')[:2]
        cn[int(a, 16)] = n.strip()
try:
    for x in json.load(open('build/cls.json')):
        if x.get('addr') and x.get('ratio', 0) >= 0.5: cn[int(x['addr'], 16)] = x['name']
except OSError: pass
def short(n): return re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', n)
oursn = set(short(n) for n in ours)
rcalls = collections.defaultdict(set)
for t, cs in callers.items():
    for c in cs: rcalls[c].add(t)
rows = []
own = collections.defaultdict(list)
for t, cs in callers.items():
    if len(cs) == 1: own[next(iter(cs))].append(t)
for l in open(sys.argv[1]):
    d, sz, ca, m = l.split()[:4]
    ca = int(ca, 16)
    hs = []
    for t in own.get(ca, []):
        j = bisect.bisect_left(starts, t)
        if j < len(starts) and starts[j] == t and t != ca:
            n = short(cn.get(t, ''))
            if n in oursn: continue
            hs.append((t, starts[j + 1] - t, n or '?'))
    if hs:
        o = ours.get(m, (0, 0))
        if len(rcalls[ca]) > o[1]: rows.append((int(sz), m, ca, '%d %d/%d' % (o[0], o[1], len(rcalls[ca])), hs))
for sz, m, ca, osz, hs in sorted(rows, key=lambda r: -r[0]):
    print('%5d ours %s %x %s %s' % (sz, osz, ca, m[:55], ' '.join('%x/%d/%s' % h for h in hs)))
