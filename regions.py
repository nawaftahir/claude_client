#!/usr/bin/env python3
"""Partition client .text into matched / unmatched blocks and label unmatched blocks by referenced strings."""
import json, re, sys
import pefile, capstone

EXE = '../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
pe = pefile.PE(EXE)
IB = pe.OPTIONAL_HEADER.ImageBase
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
F = json.load(open('../codex/client/salvage_gemini/data/names_1.0.json'))['functions']
F = sorted((int(x['addr'], 16), x['approx_size']) for x in F)
cmp = json.load(open(sys.argv[1]))
matched = {}
for x in cmp:
    if len(x['hits']) == 1:
        matched[int(x['hits'][0], 16)] = (x['name'], x['obj'])

def cstr(va):
    try:
        b = pe.get_data(va - IB, 80)
    except Exception:
        return None
    s = b.split(b'\0')[0]
    if len(s) >= 5 and all(32 <= c < 127 or c in (9, 10) for c in s):
        return s.decode()
    return None

def strings(va, size):
    out = []
    code = pe.get_data(va - IB, size)
    for ins in md.disasm(code, va):
        for m in re.findall(r'0x[0-9a-f]{6,}', ins.op_str):
            s = cstr(int(m, 16))
            if s:
                out.append(s)
    return out

blocks, cur = [], None
for va, size in F:
    if va in matched:
        if cur:
            blocks.append(cur)
            cur = None
        blocks.append({'va': va, 'size': size, 'matched': matched[va][1]})
        continue
    if cur is None:
        cur = {'va': va, 'size': 0, 'n': 0, 'strings': []}
    cur['size'] = va + size - cur['va']
    cur['n'] += 1
    if len(cur['strings']) < 400:
        cur['strings'] += strings(va, size)
if cur:
    blocks.append(cur)
json.dump(blocks, open('regions.json', 'w'), indent=0)
for b in blocks:
    if 'matched' not in b and b['size'] >= int(sys.argv[2] if len(sys.argv) > 2 else 4096):
        s = [x for x in b['strings'] if re.search(r'[A-Za-z_]{4,}', x)]
        print('%08x %7d %4d fns | %s' % (b['va'], b['size'], b['n'], ' | '.join(x.strip()[:40] for x in s[:6])))
