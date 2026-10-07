#!/usr/bin/env python3
"""Compare the call targets of a matched fn: ours (exe+map) vs retail, names via cmp.json hits.
usage: calls.py exe map cmp.json '<mangled>' retailVA size"""
import sys, re, json, struct
exe, mp, cj, name, rva, size = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], int(sys.argv[5], 16), int(sys.argv[6])
RET = '../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
def pe(p):
    b = open(p, 'rb').read(); o = struct.unpack_from('<I', b, 0x3c)[0]; n = struct.unpack_from('<H', b, o + 6)[0]
    oh = struct.unpack_from('<H', b, o + 20)[0]; base = struct.unpack_from('<I', b, o + 24 + 28)[0]; s = []
    for i in range(n):
        va, rs, rp = struct.unpack_from('<I4xII', b, o + 24 + oh + 40 * i + 12)[0], 0, 0
        vs, va, rs, rp = struct.unpack_from('<IIII', b, o + 24 + oh + 40 * i + 8)
        s.append((base + va, rs, rp))
    def rd(a, k):
        for v, rs, rp in s:
            if v <= a < v + rs: return b[rp + a - v: rp + a - v + k]
    return rd
def calls(rd, a, n):
    c = rd(a, n); out = []
    for i in range(len(c) - 4):
        if c[i] == 0xe8:
            t = a + i + 5 + struct.unpack_from('<i', c, i + 1)[0]
            if 0x401000 <= t < 0x560000: out.append(t)
    return out
names = {}
for l in open(mp):
    m = re.match(r' 0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8}) f', l)
    if m: names[int(m.group(2), 16)] = m.group(1)
ours = {v: k for k, v in names.items()}[name]
rn = {}
for f in json.load(open(cj)):
    for h in f['hits']: rn[int(h, 16)] = f['name']
o = [names.get(t, hex(t)) for t in calls(pe(exe), ours, size)]
r = [rn.get(t, hex(t)) for t in calls(pe(RET), rva, size)]
print('ours  ', len(o)); print('retail', len(r))
for i in range(max(len(o), len(r))):
    a = o[i] if i < len(o) else '-'; c = r[i] if i < len(r) else '-'
    print('  ' if a == c else '!!', a[:50].ljust(52), c[:50])
