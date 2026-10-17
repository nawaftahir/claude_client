#!/usr/bin/env python3
"""Unmatched pairs whose float-constant VALUES differ from retail (wrong literal class).
usage (in target/): constscan.py scored.txt"""
import sys, re, struct, pefile, capstone
from alldiff_lib import d, base, cd, cbase, nxt
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
po = pefile.PE('build/CoD2MP_s.exe'); pr = pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
mo, mr = po.get_memory_mapped_image(), pr.get_memory_mapped_image()
ib = po.OPTIONAL_HEADER.ImageBase
def consts(code, va, img):
    out = []
    for i in md.disasm(code, va):
        if not i.mnemonic.startswith('f'): continue
        m = re.search(r'(dword|qword) ptr \[(0x[0-9a-f]+)\]', i.op_str)
        if not m: continue
        a = int(m.group(2), 16) - ib
        try: v = struct.unpack('<f', img[a:a+4])[0] if m.group(1) == 'dword' else struct.unpack('<d', img[a:a+8])[0]
        except struct.error: continue
        if v == v and abs(v) < 1e9: out.append(round(v, 6))
    return out
for l in open(sys.argv[1]):
    p = l.split()
    if len(p) < 4 or p[3] not in nxt: continue
    size, ca, name = int(p[1]), int(p[2], 16), p[3]
    va, end = nxt[name]
    o = consts(d[va-base:end-base], va, mo); r = consts(cd[ca-cbase:ca-cbase+size], ca, mr)
    so, sr = sorted(set(o)), sorted(set(r))
    if so != sr:
        print(p[0], size, hex(ca), name[:50], 'ours-only', [x for x in so if x not in sr], 'retail-only', [x for x in sr if x not in so])
