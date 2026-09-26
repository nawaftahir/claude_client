#!/usr/bin/env python3
"""Client functions not covered and not in our source, split by whether KisakCOD defines them."""
import json, os, re, subprocess, collections
d = os.path.dirname(os.path.abspath(__file__))
K = d + '/../refrences/KisakCOD/src'
starts = sorted(int(m.group(1), 16) for f in os.listdir(d + '/ghidra/out') for m in [re.match(r'([0-9a-f]{8})_', f)] if m)
size = {a: b - a for a, b in zip(starts, starts[1:])}
names = {}
for f in ('names_1.0.tsv', 'names_cg.tsv'):
    for l in open(d + '/ghidra/' + f):
        r = l.rstrip('\n').split('\t')
        if len(r) >= 2: names.setdefault(int(r[0], 16), r[1])
hit = set()
for e in json.load(open(d + '/target/build/cmp.json')):
    hit.update(int(h, 16) for h in e['hits'])
ours = set()
for l in open(d + '/target/build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:\S+\s+(\S+)\s+[0-9a-f]{8} f', l)
    if m:
        s = m.group(1)
        ours.add(s[1:].split('@')[0] if s.startswith('?') else s.lstrip('_'))
# kisak definitions: name -> file
kdef = {}
out = subprocess.run(['grep', '-rnE', '--include=*.cpp', '--include=*.c', r'^[A-Za-z_][\w \*:<>,]*[ \*]([A-Za-z_]\w*)\s*\(', K], capture_output=True, text=True).stdout
for l in out.splitlines():
    f, _, src = l.split(':', 2)
    if src.rstrip().endswith(';'): continue
    m = re.search(r'([A-Za-z_]\w*)\s*\(', src)
    if m: kdef.setdefault(m.group(1), os.path.relpath(f, K))
rows = collections.defaultdict(list)
for a, n in names.items():
    if a in hit: continue
    cat = 'ours' if n in ours else ('kisak' if n in kdef else 'none')
    rows[cat].append((size.get(a, 0), a, n, kdef.get(n, '')))
for cat in ('ours', 'kisak', 'none'):
    print('%-6s %5d fns %8d B' % (cat, len(rows[cat]), sum(r[0] for r in rows[cat])))
with open(d + '/kgap.txt', 'w') as o:
    for s, a, n, f in sorted(rows['kisak'], reverse=True):
        o.write('%6d %x %s %s\n' % (s, a, n, f))
byf = collections.Counter()
for s, a, n, f in rows['kisak']: byf[f.split('/')[0] + '/' + (f.split('/')[1] if '/' in f else '')] += s
for f, s in byf.most_common(25): print('%8d %s' % (s, f))
