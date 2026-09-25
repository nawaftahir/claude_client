#!/usr/bin/env python3
"""Call edges the client has but our located functions lack (and the reverse), by callee name.
usage (from target/): edges.py [min-ratio]"""
import collections, json, re, sys
import capstone, vote
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
ratio = float(sys.argv[1]) if len(sys.argv) > 1 else 0.5
short = lambda n: re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', n)
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
for x in json.load(open('build/cmp.json')):
    if len(x['hits']) == 1:
        cn[int(x['hits'][0], 16)] = x['name']
cls = json.load(open('build/cls.json'))
for x in cls:
    if x.get('addr') and x['ratio'] >= 0.5:
        cn[int(x['addr'], 16)] = x['name']
def callees(code, va, names):
    out = collections.Counter()
    for i in md.disasm(code, va):
        if i.mnemonic == 'int3':
            break
        if i.mnemonic == 'call' and i.op_str.startswith('0x'):
            t = int(i.op_str, 16)
            out[short(names.get(t, hex(t)))] += 1
    return out
import bisect
starts = sorted(set(cn) | {int(a, 16) for a in (l.split('\t')[0] for l in open('../ghidra/names_1.0.tsv'))})
loc = {x['name']: x for x in cls if x.get('addr') and x['ratio'] >= ratio}
pe, cd, cbase = vote.text('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
extra = collections.defaultdict(list); missing = collections.defaultdict(list)
for name, obj, va, code, mask in vote.functions('build/CoD2MP_s.exe', 'build/CoD2MP_s.map'):
    x = loc.get(name)
    if not x:
        continue
    off = int(x['addr'], 16) - cbase
    a = callees(code, va, syms)
    ca = cbase + off
    nxt = starts[bisect.bisect_right(starts, ca)] if bisect.bisect_right(starts, ca) < len(starts) else ca + len(code) * 2
    b = callees(cd[off:off + min(nxt - ca, len(code) * 2 + 64)], ca, cn)
    for k in b:
        if k not in a and not k.startswith('0x'):
            extra[k].append(short(name))
    for k in a:
        if k not in b:
            missing[k].append(short(name))
print('client calls, ours does not:')
for k, v in sorted(extra.items(), key=lambda kv: -len(kv[1]))[:40]:
    print('  %3d %-32s %s' % (len(v), k, ' '.join(v[:6])))
print('ours calls, client does not:')
for k, v in sorted(missing.items(), key=lambda kv: -len(kv[1]))[:40]:
    print('  %3d %-32s %s' % (len(v), k, ' '.join(v[:6])))
