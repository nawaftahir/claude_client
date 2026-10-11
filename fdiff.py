#!/usr/bin/env python3
"""Instruction diff: our function (by mangled-name substring) vs client code at an address."""
import os, difflib, re, sys
import pefile, capstone

img, mapf, target, pat, caddr = sys.argv[1:6]
caddr = int(caddr, 16)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

def section(path):
    pe = pefile.PE(path)
    t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress

d, base = section(img)
syms = sorted((int(m.group(2), 16), m.group(1)) for m in
              (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open(mapf, errors='replace')) if m and base <= int(m.group(2), 16) < base + len(d))
i = next((k for k, s in enumerate(syms) if s[1] == pat), None)
if i is None:
    i = next(k for k, s in enumerate(syms) if pat in s[1])
va, name = syms[i]
ours = d[va - base:syms[i + 1][0] - base].rstrip(b'\xcc')
cd, cbase = section(target)
theirs = cd[caddr - cbase:caddr - cbase + len(ours) + 64]

def norm(code, start):
    out = []
    for ins in md.disasm(code, start):
        op = ins.op_str
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.size == 5):
            op = 'X'
        op = re.sub(r'0x[0-9a-f]{5,}', 'A', op)  # absolute addresses
        out.append('%s %s' % (ins.mnemonic, op))
        if ins.address - start >= len(ours) - 1:
            break
    return out

a, b = norm(ours, va), norm(theirs, caddr)
print(name, len(ours), 'bytes;', len(a), 'vs', len(b), 'insns')
n = 0
for line in difflib.unified_diff(a, b, 'ours', 'client', n=int(os.environ.get("FDN", 2)), lineterm=''):
    print(line)
    n += 1
    if n > int(sys.argv[6] if len(sys.argv) > 6 else 60):
        break
