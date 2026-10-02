#!/usr/bin/env python3
"""Disassemble a client range with string/float annotations: dis.py <start> <end>"""
import struct, sys
import pefile, capstone
pe = pefile.PE(__import__('os').path.dirname(__import__('os').path.abspath(__file__)) + '/target/build/CoD2MP_s.exe')
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a, b = int(sys.argv[1], 16), int(sys.argv[2], 16)
for i in md.disasm(pe.get_data(a - 0x400000, b - a), a):
    note = ''
    for tok in i.op_str.replace('[', ' ').replace(']', ' ').replace(',', ' ').split():
        if tok.startswith('0x') and len(tok) >= 8:
            v = int(tok, 16)
            try:
                raw = pe.get_data(v - 0x400000, 64)
                s = raw.split(b'\0')[0]
                if len(s) >= 3 and all(32 <= c < 127 or c in (9, 10) for c in s):
                    note = repr(s.decode())
                elif i.mnemonic.startswith('f'):
                    note = 'f=%g' % struct.unpack('<f', raw[:4])[0]
            except Exception:
                pass
    print('%x  %-6s %-40s %s' % (i.address, i.mnemonic, i.op_str, note))
