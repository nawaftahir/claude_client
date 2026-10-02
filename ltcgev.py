#!/usr/bin/env python3
"""Score ltcgsim cuts against binaries: a cut edge F->X forces an out-of-line call to X inside F.
usage (from target/): ltcgev.py [build/ltcgsim.json]  -> per cut edge: ours called?/retail called?
Retail fn addresses: cmp hits, else ghidra names. 'inl' = no call to X anywhere in F's body."""
import json, re, sys
from alldiff_lib import md, d, base, cd, cbase, syms
sim = json.load(open(sys.argv[1] if len(sys.argv) > 1 else 'build/ltcgsim.json'))['cuts']
cmp = {e['name']: e for e in json.load(open('build/cmp_base.json'))}
names = {}
for f in ('names_fix.tsv', 'names_k.tsv', 'names_1.0.tsv', 'names_cg.tsv', 'names_loc.tsv'):
    for l in open('../ghidra/' + f):
        r = l.rstrip('\n').split('\t')
        if len(r) >= 2 and re.fullmatch(r'[0-9a-fA-F]+', r[0]): names.setdefault(r[1], int(r[0], 16))
ostarts = sorted(a for a, _ in syms); oaddr = {n: a for a, n in syms}
import os
rstarts = sorted({int(m.group(1), 16) for f in os.listdir('../ghidra/out') for m in [re.match(r'([0-9a-f]{8})_', f)] if m})
import bisect
def body(code, cb, starts, a):
    i = bisect.bisect_right(starts, a)
    e = starts[i] if i < len(starts) else a + 0x400
    return {int(x.op_str, 16) for x in md.disasm(code[a - cb:e - cb], a) if x.mnemonic in ('call', 'jmp') and x.op_str.startswith('0x')}
bare = lambda k: re.sub(r'^\?([^@]+)@.*', r'\1', k.split(':', 1)[-1])
def raddr(k):
    n = k.split(':', 1)[-1]; e = cmp.get(n)
    if e and len(e['hits']) == 1: return int(e['hits'][0], 16)
    return names.get(bare(k))
tally = {}
for x, callers in sorted(sim.items()):
    for f in callers:
        o = r = '?'
        fo, xo = oaddr.get(f.split(':', 1)[-1]), oaddr.get(x.split(':', 1)[-1])
        if fo and xo: o = 'call' if xo in body(d, base, ostarts, fo) else 'inl'
        fr, xr = raddr(f), raddr(x)
        if fr and xr: r = 'call' if xr in body(cd, cbase, rstarts, fr) else 'inl'
        tally[(o, r)] = tally.get((o, r), 0) + 1
        if '-v' in sys.argv or r == 'inl': print('%-4s %-4s %-34s -> %s' % (o, r, bare(f)[:34], bare(x)))
print(tally)
