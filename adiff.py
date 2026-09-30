#!/usr/bin/env python3
"""Annotated instruction diff (strings, callee names) ours vs client.
usage (in target/): adiff.py <mangled> <client-addr> [ctx]"""
import difflib, re, sys, os
import pefile, capstone
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
os.chdir(os.path.dirname(os.path.abspath(__file__)) + '/target')
import alldiff_lib as L

pat, caddr = sys.argv[1], int(sys.argv[2], 16)
ctx = int(sys.argv[3]) if len(sys.argv) > 3 else 2
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
syms = sorted((int(m.group(2), 16), m.group(1)) for m in
              (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open('build/CoD2MP_s.map', errors='replace')) if m)
oname = {a: L.plain(s) for a, s in syms}
cname = {a: k for k, a in L.names.items()}
i = next((k for k, s in enumerate(syms) if s[1] == pat), None)
if i is None:
    i = next(k for k, s in enumerate(syms) if pat in s[1])
va = syms[i][0]
ope, cpe = pefile.PE('build/CoD2MP_s.exe'), pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
code = L.ours(va, syms[i + 1][0])
ccode = L.client(caddr, len(code) + 64)

def string_at(pe, v):
    try:
        s = pe.get_data(v - 0x400000, 48).split(b'\0')[0]
        if len(s) >= 2 and all(32 <= c < 127 or c in (9, 10) for c in s):
            return repr(s.decode())
    except Exception:
        pass
    return ''

def dis(code, start, pe, names):
    out = []
    for ins in md.disasm(code, start):
        op = ins.op_str
        note = ''
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.size == 5):
            try:
                note = names.get(int(op, 16), op)
            except ValueError:
                pass
            op = 'X'
        for t in re.findall(r'0x[0-9a-f]{5,}', op):
            note = note or string_at(pe, int(t, 16))
        op = re.sub(r'0x[0-9a-f]{5,}', 'A', op)
        out.append(('%s %s' % (ins.mnemonic, op), note))
        if ins.address - start >= len(code) - 1:
            break
    return out

a, b = dis(code, va, ope, oname), dis(ccode, caddr, cpe, cname)
sm = difflib.SequenceMatcher(None, [x[0] for x in a], [x[0] for x in b], autojunk=False)
print('%s %d B, %d vs %d insns, ratio %.3f' % (syms[i][1], len(code), len(a), len(b), sm.ratio()))
for tag, i1, i2, j1, j2 in sm.get_opcodes():
    if tag == 'equal':
        continue
    print('@@ ours %d client %d' % (i1, j1))
    for k in range(max(0, i1 - ctx), i1):
        print('   %-34s %s' % a[k])
    for k in range(i1, i2):
        print(' - %-34s %s' % a[k])
    for k in range(j1, j2):
        print(' + %-34s %s' % b[k])
