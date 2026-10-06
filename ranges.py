#!/usr/bin/env python3
# usage (in target/): ranges.py [minB]
# uncovered client .text ranges (>=N B) with matched neighbours and our unmatched fns between them
import json, re, sys, bisect
N = int(sys.argv[1]) if len(sys.argv) > 1 else 256
d = json.load(open('build/cmp.json'))
cm = open('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe', 'rb').read()
lo, hi = 0x401000, 0x5417b0
cov = bytearray(hi - lo); owner = {}
for e in d:
    if len(e['hits']) == 1 or e.get('paired'):
        a = int(e['hits'][0], 16)
        for x in range(a, min(a + e['size'], hi)): cov[x - lo] = 1
        owner[a] = e['name']
syms = sorted((int(m.group(2), 16), m.group(1)) for m in (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m)
ourva = {n: a for a, n in syms}; order = [n for a, n in syms]; idx = {n: i for i, n in enumerate(order)}
hitset = {e['name'] for e in d if e['hits']}
oa = sorted(owner)
out = []
x = 0
while x < hi - lo:
    if cov[x]: x += 1; continue
    s = x
    while x < hi - lo and not cov[x]: x += 1
    a, b = s + lo, x + lo
    raw = sum(1 for k in range(a, b) if cm[k - 0x400000 + 0x0] != 0xcc) if False else b - a
    if raw < N: continue
    i = bisect.bisect_left(oa, a) - 1; j = bisect.bisect_left(oa, b)
    pv, nx = owner.get(oa[i]) if i >= 0 else None, owner.get(oa[j]) if j < len(oa) else None
    mid = []
    if pv in idx and nx in idx and 0 < idx[nx] - idx[pv] < 40:
        mid = [n for n in order[idx[pv] + 1:idx[nx]] if n not in hitset]
    out.append((b - a, hex(a), (pv or '')[:40], (nx or '')[:40], [m.split('@')[0][:40] for m in mid]))
tot = sum(o[0] for o in out)
print('ranges', len(out), 'bytes', tot)
for o in sorted(out, reverse=True): print(*o[:2], '|', o[2], '..', o[3], '|', ' '.join(o[4]))
