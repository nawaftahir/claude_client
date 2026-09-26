#!/usr/bin/env python3
"""Disassemble the client at an address until int3 padding: cdump.py <addr> [max-insns]
Annotates string immediates and call targets (our matched name, else the ghidra name)."""
import json, os, re, struct, sys
import capstone, vote
d = os.path.dirname(os.path.abspath(__file__))
exe = d + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
pe, cd, base = vote.text(exe)
img = open(exe, 'rb').read()
p = struct.unpack_from('<I', img, 0x3c)[0]
secs = []
for i in range(struct.unpack_from('<H', img, p + 6)[0]):
    o = p + 24 + struct.unpack_from('<H', img, p + 20)[0] + 40 * i
    vs, va, rs, ro = struct.unpack_from('<IIII', img, o + 8)
    secs.append((va + 0x400000, rs, ro, img[o:o + 8].rstrip(b'\0')))


def cstr(va):
    for sva, rs, ro, name in secs:
        if name != b'.text' and sva <= va < sva + rs:
            off = ro + va - sva
            s = img[off:img.find(b'\0', off, off + 200)]
            if len(s) >= 2 and all(32 <= c < 127 or c in (9, 10, 13, 20, 21) for c in s):
                return repr(s.decode('latin-1'))
    return None


names = {}
try:
    for e in json.load(open(d + '/target/build/cmp.json')):
        for h in e['hits']:
            names.setdefault(int(h, 16), e['name'])
except OSError:
    pass
for f in os.listdir(d + '/ghidra/out'):
    m = re.match(r'([0-9a-f]{8})_(\w+)\.c$', f)
    if m:
        names.setdefault(int(m.group(1), 16), m.group(2))

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a = int(sys.argv[1], 16); n = int(sys.argv[2]) if len(sys.argv) > 2 else 60
for k, i in enumerate(md.disasm(cd[a - base:a - base + 8192], a)):
    if i.mnemonic == 'int3' or k >= n:
        break
    note = ''
    for v in re.findall(r'0x[0-9a-f]{6,}', i.op_str):
        v = int(v, 16)
        if i.mnemonic in ('call', 'jmp') and v in names:
            note = names[v]
        elif cstr(v):
            note = cstr(v)
    print(hex(i.address), i.mnemonic, i.op_str, ('  ; ' + note) if note else '')
