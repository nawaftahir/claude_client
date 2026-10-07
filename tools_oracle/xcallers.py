#!/usr/bin/env python3
"""Retail call sites of a matched fn, grouped by containing fn (names: cmp hits > names_fix/k/1.0).
usage (target/): xcallers.py cmp.json '<mangled>'  (also prints our graph callers from build/ord_i.txt)"""
import json, struct, sys, bisect
CJ, NAME = sys.argv[1], sys.argv[2]
sys.argv = ['x', 'x', 'x', 'x', 'x', '0', '0']
exec(open(__file__.replace('xcallers.py', 'calls.py')).read().split('names = {}')[0])
starts, tgt = {}, None
for f in json.load(open(CJ)):
    for h in f['hits']: starts[int(h, 16)] = f['name'].split('@@')[0]
    if f["name"] == NAME and f['hits']: tgt = int(f['hits'][0], 16)
for t in ('../ghidra/names_fix.tsv', '../ghidra/names_k.tsv', '../ghidra/names_1.0.tsv'):
    for l in open(t):
        p = l.split('\t')
        try: starts.setdefault(int(p[0], 16), p[1].strip())
        except (ValueError, IndexError): pass
ks = sorted(starts); c = pe(RET)(0x401000, 0x160000); out = {}
for i in range(len(c) - 5):
    if c[i] == 0xe8 and 0x401000 + i + 5 + struct.unpack_from('<i', c, i + 1)[0] == tgt:
        k = ks[bisect.bisect_right(ks, 0x401000 + i) - 1]; out['%s@%x' % (starts[k], k)] = out.get('%s@%x' % (starts[k], k), 0) + 1
print('retail', hex(tgt), sum(out.values()), 'sites:', out)
sys.path.insert(0, __file__.rsplit('/', 1)[0]); import sim
g = sim.graph('build/ord_i.txt')
print('ours graph callers:', sorted(n.split('@@')[0] for n in g if NAME in g[n]))
