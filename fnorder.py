#!/usr/bin/env python3
"""Reorder a TU's function definitions to retail address order.
usage (from target/): fnorder.py <src.cpp> [-n]   (-n = dry run, prints the plan)
Chunks = top-level function defs (with leading comments); other top-level text keeps its relative
order ahead of the moved block. Fns without a retail address stay glued to their predecessor."""
import json, re, sys
p = sys.argv[1]
L = open(p).read().split('\n')
names = {}
for f in ('names_fix.tsv', 'names_k.tsv', 'names_1.0.tsv', 'names_cg.tsv', 'names_loc.tsv'):
    for l in open('../ghidra/' + f):
        r = l.rstrip('\n').split('\t')
        if len(r) >= 2 and re.fullmatch(r'[0-9a-fA-F]+', r[0]):
            names.setdefault(r[1], int(r[0], 16))
for e in json.load(open('build/cmp.json')):
    if len(e['hits']) == 1:
        names[re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', e['name'])] = int(e['hits'][0], 16)
# find function defs: a line '{' at col 0 preceded by a signature ending in ')'
chunks, i, first = [], 0, None
while i < len(L):
    if L[i] == '{' and i > 0 and L[i - 1].rstrip().endswith(')') and not L[i - 1].startswith((' ', '\t', '#')):
        s = i - 1
        while s > 0 and L[s - 1] and not L[s - 1].rstrip().endswith((';', '}', ')')) and not L[s - 1].startswith(('//', '#')) \
                and L[s - 1].startswith((' ', '\t')):
            s -= 1  # multi-line signature continuation
        while s > 0 and not L[s - 1].startswith(' ') and re.match(r'^[A-Za-z_].*[,(]\s*$', L[s - 1]):
            s -= 1
        while s > 0 and L[s - 1].startswith('//'):
            s -= 1
        e = i
        while L[e] != '}':
            e += 1
        m = re.search(r'([A-Za-z_][A-Za-z_0-9]*)\s*\(', ' '.join(L[s:i]).split('//')[-1] if False else ' '.join(x for x in L[s:i] if not x.startswith('//')))
        chunks.append((s, e, m.group(1)))
        i = e + 1
    else:
        i += 1
start = chunks[0][0]
end = chunks[-1][1] + 1
other, fns, pos = [], [], start
for s, e, n in chunks:
    if pos < s:
        other.append(L[pos:s])
    fns.append([n, L[s:e + 1]])
    pos = e + 1
# glue fns without an address to the previous one
import statistics
med = statistics.median([names[n] for n, _ in fns if names.get(n, 0) > 0])
groups, far = [], []
for n, body in fns:
    a = names.get(n)
    if a is not None and a > 0 and abs(a - med) > 0x30000:
        far.append(body)  # shared helper placed elsewhere in retail: keep it ahead
        continue
    if not a:
        a = None
    if a is None and groups:
        groups[-1][2].append((n, body))
    else:
        groups.append([a if a is not None else -1, n, [(n, body)]])
order = sorted(groups, key=lambda g: g[0])
if '-n' in sys.argv:
    for a, n, mem in order:
        print('%x %s %s' % (a & 0xffffffff, n, ' '.join(m for m, _ in mem[1:])))
    sys.exit()
out = L[:start]
for o in other:
    o = [x for x in o if x.strip()]
    if o:
        out += o + ['']
for body in far:
    out += body + ['']
for a, n, mem in order:
    for m, body in mem:
        out += body + ['']
out += L[end:]
open(p, 'w').write('\n'.join(out))
