#!/usr/bin/env python3
"""Raw operand diff (calls/addresses that fdiff normalizes away), our fn vs retail.
usage (from target/): rawops.py <mangled-prefix> <client-addr>"""
import re, sys, bisect, pefile, capstone
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def section(path):
    pe = pefile.PE(path); t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
mp = [(int(m.group(2), 16), m.group(1)) for m in (re.match(r'\s*\S+\s+(\S+)\s+([0-9a-f]{8})', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m]
mp.sort(); va_ = [a for a, _ in mp]
def oname(a):
    k = bisect.bisect_right(va_, a) - 1
    return '%s+%x' % (mp[k][1][:40], a - mp[k][0]) if k >= 0 else hex(a)
fns = [x for x in mp if x[1].startswith(sys.argv[1])]
va, name = fns[0]
nx = mp[bisect.bisect_right(va_, va)][0]
d, b = section('build/CoD2MP_s.exe'); cd, cb = section('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
ca = int(sys.argv[2], 16)
o = list(md.disasm(d[va - b:nx - b], va)); t = list(md.disasm(cd[ca - cb:ca - cb + nx - va], ca))
for x, y in zip(o, t):
    if x.bytes != y.bytes:
        print('%-40s | %-40s | %s' % (x.mnemonic + ' ' + x.op_str, y.mnemonic + ' ' + y.op_str,
              ' '.join(oname(int(h, 16)) for h in re.findall(r'0x[0-9a-f]{6,}', x.op_str))))
