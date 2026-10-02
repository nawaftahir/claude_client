#!/usr/bin/env python3
"""Callers of a function in retail and ours, with frame-alignment flag.
usage (from target/): callers.py <client-callee-hex> <our-callee-hex>"""
import bisect, re, struct, sys
def load(path):
    d = open(path, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    base = struct.unpack_from('<I', d, pe + 0x34)[0]
    ns = struct.unpack_from('<H', d, pe + 6)[0]
    so = pe + 0x18 + struct.unpack_from('<H', d, pe + 0x14)[0]
    for i in range(ns):
        va, sz, ro = struct.unpack_from('<III', d, so + i * 40 + 12)
        if d[so + i * 40:so + i * 40 + 5] == b'.text':
            return d[ro:ro + sz], base + va
def callers(path, starts, names, tgt):
    code, va = load(path)
    out = {}
    for i in range(len(code) - 5):
        if code[i] == 0xe8 and va + i + 5 + struct.unpack_from('<i', code, i + 1)[0] == tgt:
            k = bisect.bisect_right(starts, va + i) - 1
            s = starts[k]
            o = s - va
            al = code[o:o + 6] == b'\x55\x8b\xec\x83\xe4\xf8'
            out[s] = (names.get(s, '?'), al)
    return out
cn = {}
for f in ('../ghidra/names_fix.tsv', '../ghidra/names_k.tsv', '../ghidra/names_cg.tsv', '../ghidra/names_1.0.tsv'):
    for l in open(f):
        p = l.split('\t')
        if len(p) > 1 and re.fullmatch(r'[0-9a-fA-F]+', p[0]):
            cn.setdefault(int(p[0], 16), p[1].strip())
os_ = {}
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})', l)
    if m:
        os_[int(m.group(2), 16)] = m.group(1)
for tag, path, names, tgt in (('retail', '../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe', cn, int(sys.argv[1], 16)),
                              ('ours', 'build/CoD2MP_s.exe', os_, int(sys.argv[2], 16))):
    r = callers(path, sorted(names), names, tgt)
    print(tag)
    for s, (n, al) in sorted(r.items()):
        print(f'  {s:x} {"ALIGN" if al else "     "} {n[:70]}')
