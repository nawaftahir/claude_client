#!/usr/bin/env python3
"""List unmatched pairs whose incoming-register set or ret imm differs (param conv class).
usage (in target/): convscan.py scored.txt"""
import sys, re, capstone
from alldiff_lib import d, base, cd, cbase, nxt
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
REGS = {'eax', 'ecx', 'edx', 'ebx', 'esi', 'edi'}
def conv(code, va):
    seen, ins_in, rets = set(), set(), set()
    for k, i in enumerate(md.disasm(code, va)):
        if i.mnemonic == 'ret': rets.add(i.op_str or '0')
        if k < 40 and i.mnemonic not in ('push', 'pop'):
            r, w = i.regs_access()
            for x in r:
                n = i.reg_name(x)
                if n in REGS and n not in seen: ins_in.add(n)
            if i.mnemonic == 'xor' and len(i.operands) == 2 and i.op_str.split(', ')[0] == i.op_str.split(', ')[-1]:
                seen.add(i.op_str.split(', ')[0])
            for x in w: seen.add(i.reg_name(x))
    return ins_in, rets
for l in open(sys.argv[1]):
    p = l.split()
    if len(p) < 4 or p[3] not in nxt: continue
    n, size, ca, name = int(p[0]), int(p[1]), int(p[2], 16), p[3]
    va, end = nxt[name]
    o = conv(d[va-base:end-base], va); r = conv(cd[ca-cbase:ca-cbase+size], ca)
    if o != r: print(n, size, hex(ca), name[:60], 'ours', sorted(o[0]), sorted(o[1]), 'retail', sorted(r[0]), sorted(r[1]))
