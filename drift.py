#!/usr/bin/env python3
"""Layout drift vs retail: drift.py exp.json ours.map [n]
For matched fns with one retail hit, delta = our VA - retail VA. Walks retail order and prints each
point where delta changes (a size/order/padding defect between the two fns), largest first."""
import json, re, sys

exp = json.load(open(sys.argv[1]))
ours = {}
for line in open(sys.argv[2], errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})\s', line)
    if m:
        ours.setdefault(m.group(1), int(m.group(2), 16))
pts = sorted((int(e['hits'][0], 16), ours[e['name']], e['name'], e['size'])
             for e in exp if len(e.get('hits', [])) == 1 and e['name'] in ours)
print('matched fns placed: %d; at the retail address: %d' % (len(pts), sum(1 for r, o, n, s in pts if r == o)))
steps, prev = [], None
for r, o, n, s in pts:
    d = o - r
    if prev is not None and d != prev[0]:
        steps.append((d - prev[0], prev[1], r, prev[2], n))
    prev = (d, r, n)
print('drift points: %d (first at retail %s)' % (len(steps), hex(steps[0][1]) if steps else '-'))
for k, (dd, ra, rb, na, nb) in enumerate(steps[:int(sys.argv[3]) if len(sys.argv) > 3 else 40]):
    print('%+6d  retail %06x..%06x  %s -> %s' % (dd, ra, rb, na[:45], nb[:45]))
