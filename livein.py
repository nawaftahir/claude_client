#!/usr/bin/env python3
"""Calling-convention mismatches: registers read before written at function entry (LTCG register
args), ours vs retail, for every named pair. run in target/: python3 ../livein.py [cmp.json]"""
import json, sys
from capstone import x86
from alldiff_lib import md, syms, nxt, d, base, cd, cbase, names, plain
md.detail = True
REGS = {'eax', 'ecx', 'edx', 'ebx', 'esi', 'edi', 'ebp'}
SUB = {'al': 'eax', 'ah': 'eax', 'ax': 'eax', 'cl': 'ecx', 'ch': 'ecx', 'cx': 'ecx', 'dl': 'edx', 'dh': 'edx',
       'dx': 'edx', 'bl': 'ebx', 'bh': 'ebx', 'bx': 'ebx', 'si': 'esi', 'di': 'edi', 'bp': 'ebp'}

def livein(code, start):
    written, live = set(), set()
    for ins in md.disasm(code, start):
        if ins.mnemonic == 'push' and ins.op_str in REGS | {'ebp'} and ins.op_str not in written:
            written.add(ins.op_str)  # callee-save spill, not a use
            continue
        rd, wr = ins.regs_access()
        if ins.mnemonic in ('xor', 'sub') and len(ins.operands) == 2 and ins.op_str.split(', ')[0] == ins.op_str.split(', ')[-1]:
            rd = ()
        for r in rd:
            n = ins.reg_name(r); n = SUB.get(n, n)
            if n in REGS and n not in written: live.add(n)
        for r in wr:
            n = ins.reg_name(r); n = SUB.get(n, n)
            if n in REGS: written.add(n)
        if ins.mnemonic in ('call', 'ret', 'jmp') or ins.mnemonic.startswith('j') or ins.address - start > 400:
            break
    return live

cmp = json.load(open(sys.argv[1] if len(sys.argv) > 1 else 'build/cmp.json'))
matched = {e['name'] for e in cmp if e['hits']}
out = []
for va, m in syms:
    if m in matched or m not in nxt: continue
    ca = names.get(plain(m))
    if ca is None or ca % 16: continue
    s, e = nxt[m]
    if e - s > 0x20000: continue
    a = livein(d[va-base:e-base], va)
    b = livein(cd[ca-cbase:ca-cbase+600], ca)
    if a != b:
        out.append((e - s, m, hex(ca), sorted(a), sorted(b)))
for r in sorted(out, reverse=True):
    print('%6d %s %s ours=%s retail=%s' % (r[0], r[2], r[1], ','.join(r[3]) or '-', ','.join(r[4]) or '-'))
