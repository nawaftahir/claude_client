#!/usr/bin/env python3
# tri.py all_scored.txt: classify each unmatched fn's residue (run in claude_client/target)
#   B = same instructions, different order (scheduling); A? = same after register renaming (register choice);
#   C = instruction multiset differs (structure/IL). Prints: class diff bytes retail name
import re, sys, collections, pefile, capstone
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def section(path):
    pe = pefile.PE(path)
    t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
syms = {m.group(1): int(m.group(2), 16) for m in
        (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m}
od, ob = section('build/CoD2MP_s.exe')
rd, rb = section('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
REG = re.compile(r'\b(e[abcd]x|e[sd]i|ebp|[abcd][lh]|[abcd]x|si|di)\b')
def dis(d, b, va, n):
    out = []
    for i in md.disasm(d[va - b:va - b + n], va):
        op = i.op_str
        if i.mnemonic == 'call' or i.mnemonic.startswith('j'): op = 'X'
        out.append((i.mnemonic, re.sub(r'0x[0-9a-f]{5,}', 'A', op)))
    return out
def canon(seq):
    # rename registers in first-seen order so a consistent register permutation compares equal
    m = {}
    def f(x):
        r = x.group(1)[-2:] if x.group(1).startswith('e') else x.group(1)
        m.setdefault(r, 'r%d' % len(m)); return m[r]
    return [(a, REG.sub(f, b)) for a, b in seq]
for line in open(sys.argv[1]):
    w = line.split()
    if len(w) < 4 or int(w[0]) == 0: continue
    diff, size, ra, name = int(w[0]), int(w[1]), int(w[2], 16), w[3]
    if name not in syms: continue
    o = dis(od, ob, syms[name], size); r = dis(rd, rb, ra, size)
    if o == r: cls = 'D'
    elif collections.Counter(o) == collections.Counter(r): cls = 'B'
    elif canon(o) == canon(r): cls = 'A'
    elif collections.Counter(m for m, _ in o) == collections.Counter(m for m, _ in r): cls = 'A+B'
    else: cls = 'C'
    print('%s\t%d\t%d\t0x%x\t%s' % (cls, diff, size, ra, name))
