#!/usr/bin/env python3
"""Disassemble one of OUR functions by mangled name. usage: odis.py <mangled>  (run in target/)"""
import re, sys, pefile, capstone
syms = sorted((int(m.group(2), 16), m.group(1)) for m in
              (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m)
i = next(k for k, s in enumerate(syms) if s[1] == sys.argv[1])
pe = pefile.PE('build/CoD2MP_s.exe')
t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
base = pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
va = syms[i][0]
code = t.get_data()[va - base:syms[i + 1][0] - base].rstrip(b'\xcc')
for ins in capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32).disasm(code, va):
    print('%x  %-6s %s' % (ins.address, ins.mnemonic, ins.op_str))
