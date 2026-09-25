#!/usr/bin/env python3
"""Union of client .text bytes covered by uniquely-placed matches in a cmp.py result."""
import json, sys
r = json.load(open(sys.argv[1]))
cov = set()
u = [x for x in r if len(x['hits']) == 1]
for x in u:
    a = int(x['hits'][0], 16)
    cov.update(range(a, a + x['size']))
print('fns %d matched %d unique %d | unique bytes %d = %.2f%% of 1,408,683' % (
    len(r), sum(1 for x in r if x['hits']), len(u), len(cov), len(cov) / 14086.83))
