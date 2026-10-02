#!/usr/bin/env python3
"""tuidx.py: link index of each obj + near-miss rows annotated with it (run from target/).
Usage: python3 ../tuidx.py all_scored.txt  -> 'idx difflines size addr name obj'"""
import re, sys
idx = {}
for n, l in enumerate(x.split()[0] for x in open('src/tus.txt') if x.strip() and not x.startswith('#')):
    idx[re.sub(r'\.(cpp|c)$', '.obj', l.split('/')[-1])] = n
obj = {}
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:\S+\s+(\S+)\s+[0-9a-f]{8} f\s+(?:i\s+)?(\S+)', l)
    if m: obj[m.group(1)] = m.group(2)
for l in open(sys.argv[1]):
    d, s, a, n = l.split()[:4]
    o = obj.get(n, '?')
    print(idx.get(o, -1), d, s, a, n, o)
