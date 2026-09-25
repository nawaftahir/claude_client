#!/usr/bin/env python3
"""Functions gained / lost between two cmp.py results."""
import json, sys
a = {x['name']: x for x in json.load(open(sys.argv[1]))}
b = {x['name']: x for x in json.load(open(sys.argv[2]))}
ok = lambda x: x is not None and len(x['hits']) == 1
gain = [n for n in b if ok(b[n]) and not ok(a.get(n))]
lost = [n for n in a if ok(a[n]) and not ok(b.get(n))]
print('gained %d fns %d B, lost %d fns %d B' % (len(gain), sum(b[n]['size'] for n in gain),
                                                 len(lost), sum(a[n]['size'] for n in lost)))
for n in sorted(lost, key=lambda n: -a[n]['size'])[:int(sys.argv[3]) if len(sys.argv) > 3 else 15]:
    print('  lost', a[n]['size'], n, a[n]['obj'])
