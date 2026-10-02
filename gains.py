#!/usr/bin/env python3
"""gains.py [base.json] [new.json]: functions gained/lost between two cmp.json runs (run from target/)."""
import json, sys
a = {x['name']: x for x in json.load(open(sys.argv[1] if len(sys.argv) > 1 else 'build/cmp_base.json'))}
b = {x['name']: x for x in json.load(open(sys.argv[2] if len(sys.argv) > 2 else 'build/cmp.json'))}
ok = lambda x: x is not None and len(x['hits']) == 1
for n in b:
    if ok(b[n]) and not ok(a.get(n)) and not n.startswith('$L'): print('gain', b[n]['size'], n)
for n in a:
    if ok(a[n]) and not ok(b.get(n)) and not n.startswith('$L'): print('lost', a[n]['size'], n)
