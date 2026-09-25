#!/usr/bin/env python3
"""Disassemble the client at an address until int3 padding: cdump.py <addr> [max-insns]"""
import sys
import capstone, vote
pe, cd, base = vote.text('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a = int(sys.argv[1], 16); n = int(sys.argv[2]) if len(sys.argv) > 2 else 60
for k, i in enumerate(md.disasm(cd[a - base:a - base + 4096], a)):
    if i.mnemonic == 'int3' or k >= n:
        break
    print(hex(i.address), i.mnemonic, i.op_str)
