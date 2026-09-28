import os, difflib, re, json, sys, pefile, capstone
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def section(path):
    pe = pefile.PE(path)
    t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
syms = sorted((int(m.group(2), 16), m.group(1)) for m in
              (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m)
nxt = {s[1]: (s[0], syms[k+1][0]) for k, s in enumerate(syms[:-1])}
d, base = section('build/CoD2MP_s.exe')
cd, cbase = section('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
names = {}
located = set()  # mid-function hits; CRT names are legitimately unaligned
dropped = set()
for f in (() if os.environ.get('NAMES_NOFIX') else ('../ghidra/names_fix.tsv',)) + ('../ghidra/names_k.tsv', '../ghidra/names_1.0.tsv', '../ghidra/names_cg.tsv', '../ghidra/names_loc.tsv'):
    for l in open(f):
        r = l.rstrip('\n').split('\t')
        if len(r) > 2 and r[2] == 'drop': dropped.add(r[1]); continue
        if len(r) >= 2:
            a = int(r[0], 16)  # prefer a 16-aligned (function start) candidate
            if r[1] not in names or (names[r[1]] % 16 and not a % 16): names[r[1]] = a
            if len(r) > 2 and r[2] == 'located': located.add(r[1])
for _k in dropped: names.pop(_k, None)
def plain(m):
    if m.startswith('?'): return m[1:].split('@')[0]
    return m.lstrip('_')
def norm(code, start, lim):
    out = []
    for ins in md.disasm(code, start):
        op = ins.op_str
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.size == 5): op = 'X'
        op = re.sub(r'0x[0-9a-f]{5,}', 'A', op)
        out.append('%s %s' % (ins.mnemonic, op))
        if ins.address - start >= lim - 1: break
    return out

def ours(va, end): return d[va-base:end-base].rstrip(b'\xcc')
def client(ca, n): return cd[ca-cbase:ca-cbase+n]
def cstarts():
    return [cbase+k for k in range(16, len(cd), 16) if cd[k-1] in (0xcc, 0xc3) or cd[k-2:k] == b'\xc2\x00']

# mid-function "located" names: snap back to the containing function start
import bisect
_cs = cstarts()
for _k, _a in names.items():
    if _a % 16 and _k in located:
        _i = bisect.bisect_right(_cs, _a) - 1
        if _i >= 0: names[_k] = _cs[_i]
