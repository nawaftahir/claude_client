#!/usr/bin/env python3
# a2n.py <addr>...: name 1.0 addresses via our matched functions (cmp.json) and ghidra names
import json, os, re, sys
d = os.path.dirname(os.path.abspath(__file__))
hit = {}
for e in json.load(open(d + '/target/build/cmp.json')):
    for h in e['hits']:
        hit.setdefault(int(h, 16), []).append(e['name'])
gh = {}
for f in os.listdir(d + '/ghidra/out'):
    m = re.match(r'([0-9a-f]{8})_(\w+)\.c$', f)
    if m:
        gh[int(m.group(1), 16)] = m.group(2)
for a in sys.argv[1:]:
    a = int(a, 16)
    print(hex(a), hit.get(a, ['-'])[0], '|', gh.get(a, '-'))
