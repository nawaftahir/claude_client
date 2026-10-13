#!/usr/bin/env python3
"""Diff-line count for every unmatched fn with a named client address. usage: alldiff.py (run in target/; needs build/cmp.json from make measure)"""
import difflib, re, json, sys, pefile, capstone
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def section(path):
    pe = pefile.PE(path)
    t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
syms = sorted((int(m.group(2), 16), m.group(1)) for m in
              (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m)
nxt = {s[1]: (s[0], syms[k+1][0]) for k, s in enumerate(syms[:-1])}
d, base = section('build/CoD2MP_s.exe')
cd, cbase = section('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
from alldiff_lib import names
def plain(m):
    if m.startswith('?'):
        p = m[1:].split('@@')[0].split('@')
        if len(p) >= 2 and p[1] and (p[1] + '::' + p[0]) in names: return p[1] + '::' + p[0]
        return p[0]
    return m.lstrip('_')
def norm(code, start, lim):
    out = []
    for ins in md.disasm(code, start):
        op = ins.op_str
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.size == 5): op = 'X'
        op = re.sub(r'0x[0-9a-f]{5,}', 'A', op)
        out.append('%s %s' % (ins.mnemonic, op))
        if ins.address - start >= lim - 1: break
    return out
res = []
for x in json.load(open(sys.argv[1] if len(sys.argv)>1 else 'build/cmp.json')):
    if x.get('hits') or x['name'] not in nxt: continue
    ca = names.get(plain(x['name']))
    if ca is None: continue
    va, end = nxt[x['name']]
    ours = d[va-base:end-base].rstrip(b'\xcc')
    a = norm(ours, va, len(ours)); b = norm(cd[ca-cbase:ca-cbase+len(ours)+64], ca, len(ours))
    n = sum(1 for l in difflib.unified_diff(a, b, n=0, lineterm='') if l[:1] in '+-' and l[:3] not in ('+++', '---'))
    res.append((n, len(ours), ca, x['name']))
for r in sorted(res): print(r[0], r[1], hex(r[2]), r[3])
