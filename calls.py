#!/usr/bin/env python3
"""Diff the call-target sequence of our function vs the client's (names from the map / names_1.0.tsv).
usage (from target/): calls.py <mangled-prefix> <client-addr>"""
import bisect, difflib, re, sys
import capstone, vote
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
pre, caddr = sys.argv[1], int(sys.argv[2], 16)
syms = {}
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})', l)
    if m:
        syms[int(m.group(2), 16)] = m.group(1)
cn = {}
for f in ('../ghidra/names_cg.tsv', '../ghidra/names_1.0.tsv'):
    for l in open(f):
        a, n = l.split('\t')[:2]
        cn[int(a, 16)] = n
def seq(code, va, names):
    out = []
    for i in md.disasm(code, va):
        if i.mnemonic == 'call' and i.op_str.startswith('0x'):
            t = int(i.op_str, 16)
            n = names.get(t, hex(t))
            out.append(re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', n))
        if i.mnemonic == 'int3':
            break
    return out
import json
for x in json.load(open('build/cmp.json')):
    if len(x['hits']) == 1:
        cn[int(x['hits'][0], 16)] = x['name']
cls = 'build/cls.json'
try:
    for x in json.load(open(cls)):
        if x.get('addr') and x.get('ratio', 0) >= 0.5:
            cn[int(x['addr'], 16)] = x['name']
except OSError:
    pass
for name, obj, va, code, mask in vote.functions('build/CoD2MP_s.exe', 'build/CoD2MP_s.map'):
    if name.startswith(pre):
        break
pe, cd, cbase = vote.text('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
a = seq(code, va, syms)
b = seq(cd[caddr - cbase:caddr - cbase + len(code) + 4096], caddr, cn)
if '-c' in sys.argv:
    import collections
    ca, cb = collections.Counter(a), collections.Counter(b)
    for k in sorted(set(ca) | set(cb)):
        if ca[k] != cb[k]:
            print('%4d %4d %s' % (ca[k], cb[k], k))
else:
    for l in difflib.unified_diff(a, b, 'ours', 'client', n=1, lineterm=''):
        print(l)
