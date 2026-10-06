#!/usr/bin/env python3
# usage (in target/): gapsplit.py -- classify uncovered client .text bytes: named retail fn present in our map / named but absent / unnamed
import json, re, bisect, sys
sys.path.insert(0, '..')
d = json.load(open('build/cmp.json'))
lo, hi = 0x401000, 0x5417b0
cov = bytearray(hi - lo)
for e in d:
    if len(e['hits']) == 1 or e.get('paired'):
        a = int(e['hits'][0], 16)
        for x in range(a, min(a + e['size'], hi)): cov[x - lo] = 1
names = {}
for f in ('names_fix.tsv', 'names_k.tsv', 'names_1.0.tsv', 'names_cg.tsv', 'names_loc.tsv'):
    for l in open('../ghidra/' + f):
        r = l.rstrip('\n').split('\t')
        if len(r) >= 2 and not (len(r) > 2 and r[2] == 'drop'):
            a = int(r[0], 16)
            if a % 16 == 0: names.setdefault(a, r[1])
ours = set()
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:\S+\s+(\S+)\s+[0-9a-f]{8} f', l)
    if m:
        n = m.group(1)
        ours.add(n[1:].split('@@')[0].split('@')[0] if n.startswith('?') else n.lstrip('_'))
starts = sorted(names)
cls = {'present': 0, 'absent': 0, 'unnamed': 0}; absent = {}
for x in range(lo, hi):
    if cov[x - lo]: continue
    i = bisect.bisect_right(starts, x) - 1
    if i < 0 or x - starts[i] > 0x3000: cls['unnamed'] += 1; continue
    n = names[starts[i]].split('::')[-1]
    if n in ours: cls['present'] += 1
    else: cls['absent'] += 1; absent[names[starts[i]]] = absent.get(names[starts[i]], 0) + 1
print(cls)
for n, b in sorted(absent.items(), key=lambda t: -t[1])[:60]: print(b, n)
