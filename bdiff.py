#!/usr/bin/env python3
"""Raw-byte diff of one of our fns vs a client address, reloc-masked. usage: bdiff.py name 0xaddr"""
import sys, pefile
sys.path.insert(0, '..')
import alldiff_lib as L
pe = pefile.PE('build/CoD2MP_s.exe'); base = pe.OPTIONAL_HEADER.ImageBase
rel = {base + e.rva for b in pe.DIRECTORY_ENTRY_BASERELOC for e in b.entries if e.type == 3}
name, ca = sys.argv[1], int(sys.argv[2], 16)
k = next(k for k in L.nxt if sys.argv[1] in k)
va, end = L.nxt[k]; o = L.ours(va, end); c = L.client(ca, len(o))
mask = {i for r in rel if va <= r < end for i in range(r - va, r - va + 4)}
for ins in L.md.disasm(o, va):
    if ins.bytes[0] in (0xe8, 0xe9) and ins.size == 5: mask |= set(range(ins.address - va + 1, ins.address - va + 5))
bad = [i for i in range(len(o)) if i not in mask and o[i] != c[i]]
print(k, len(o), 'bad', len(bad), [hex(i) for i in bad[:20]])
for ins in L.md.disasm(o, va):
    i = ins.address - va
    if any(i <= b < i + ins.size for b in bad): print(hex(i), ins.mnemonic, ins.op_str, '| client', c[i:i+ins.size].hex())
