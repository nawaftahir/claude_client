#!/usr/bin/env python3
"""Rank callees whose inline decision differs across near-miss functions (why.py inline-diff bucket).
usage (from target/, after why.py): inlcause.py [build/why_inline-diff.txt] [all_scored.txt] [callee: per-function rows]
Columns: affected-bytes fns ours-calls retail-calls callee-matched callee. ours>retail = retail inlines it."""
import collections, json, re, sys
from alldiff_lib import nxt, md, ours, client
short = lambda n: re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', n)
r2o = {}
matched = set()
for e in json.load(open('build/cmp.json')):
    for h in e['hits']:
        r2o[int(h, 16)] = short(e['name'])
    if e['hits']:
        matched.add(short(e['name']))
for f in ('../ghidra/names_fix.tsv', '../ghidra/names_k.tsv', '../ghidra/names_cg.tsv', '../ghidra/names_1.0.tsv'):
    for l in open(f):
        p = l.split('\t')
        if len(p) > 1 and re.fullmatch(r'[0-9a-fA-F]+', p[0]):
            r2o.setdefault(int(p[0], 16), p[1].strip())
o2n = {}
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})', l)
    if m:
        o2n[int(m.group(2), 16)] = short(m.group(1))
def calls(code, va, names):
    c = collections.Counter()
    for i in md.disasm(code, va):
        if i.mnemonic == 'call' and i.op_str.startswith('0x'):
            c[names.get(int(i.op_str, 16), i.op_str)] += 1
    return c
agg = collections.defaultdict(lambda: [0, 0, 0, 0])
scored = {l.split()[3]: l.split()[2] for l in open(sys.argv[2] if len(sys.argv) > 2 else '/dev/null')} if len(sys.argv) > 2 else {}
for l in open(sys.argv[1] if len(sys.argv) > 1 else 'build/why_inline-diff.txt'):
    sz, d, n = l.split()
    va, end = nxt[n]
    o = ours(va, end)
    ca = int(scored[n], 16) if n in scored else None
    if ca is None:
        continue
    c = client(ca, len(o) + 64)
    a, b = calls(o, va, o2n), calls(c[:len(o)], ca, r2o)
    for k in set(a) | set(b):
        if a[k] != b[k]:
            g = agg[k]; g[0] += int(sz); g[1] += 1; g[2] += a[k]; g[3] += b[k]
            if len(sys.argv) > 3 and k == sys.argv[3]:
                print('%6s %3d %3d %s %s' % (sz, a[k], b[k], hex(ca), n))
for k, (s, f, x, y) in sorted(agg.items(), key=lambda kv: -kv[1][0])[:60]:
    print('%7d %3d %3d %3d %s %s' % (s, f, x, y, 'M' if k in matched else '-', k))
