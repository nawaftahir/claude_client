#!/usr/bin/env python3
"""shortpair.py: unmatched short-anchor fns (cmp.py name-pair fallback failed) -> masked scan of all 16-aligned client starts."""
import bisect, json, re, sys
import pefile, capstone
D = __file__.rsplit('/', 1)[0] + '/'
img, mapf, target = D + 'target/build/CoD2MP_s.exe', D + 'target/build/CoD2MP_s.map', D + '../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
pe = pefile.PE(img); base = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections if s.Name.startswith(b'.text'))
tdata, tva = text.get_data(), base + text.VirtualAddress
relocs = sorted({base + e.rva for b in getattr(pe, 'DIRECTORY_ENTRY_BASERELOC', []) for e in b.entries if e.type == 3})
cpe = pefile.PE(target)
ctext = next(s for s in cpe.sections if s.Name.startswith(b'.text'))
cdata, cva = ctext.get_data(), cpe.OPTIONAL_HEADER.ImageBase + ctext.VirtualAddress
unm = {e['name'] for e in json.load(open(D + 'target/build/cmp.json')) if not e['hits']}
covered = set(); o2r = {}
for e in json.load(open(D + 'target/build/cmp.json')):
    for h in e['hits']: covered.add(int(h, 16))
syma = {}
for line in open(mapf, errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if m: syma[m.group(1)] = int(m.group(2), 16)
for e in json.load(open(D + 'target/build/cmp.json')):
    if len(e['hits']) == 1 and e['name'] in syma: o2r[syma[e['name']]] = int(e['hits'][0], 16)
syms = []
for line in open(mapf, errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if m: syms.append((int(m.group(2), 16), m.group(1)))
syms.sort()
# data/call target map learned from matched fns' relocation slots
dmap = {}
for i, (va, name) in enumerate(syms):
    if va not in o2r: continue
    end = syms[i + 1][0] if i + 1 < len(syms) else tva + len(tdata)
    n = len(tdata[va - tva:end - tva].rstrip(b'\xcc'))
    for r in relocs[bisect.bisect_left(relocs, va):bisect.bisect_left(relocs, va + n)]:
        ov = int.from_bytes(tdata[r - tva:r - tva + 4], 'little')
        rv = int.from_bytes(cdata[o2r[va] + (r - va) - cva:o2r[va] + (r - va) - cva + 4], 'little')
        dmap.setdefault(ov, set()).add(rv)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i, (va, name) in enumerate(syms):
    if name not in unm: continue
    end = syms[i + 1][0] if i + 1 < len(syms) else tva + len(tdata)
    code = tdata[va - tva:end - tva].rstrip(b'\xcc')
    if len(code) < 6: continue
    mask = set()
    for r in relocs[bisect.bisect_left(relocs, va):bisect.bisect_left(relocs, va + len(code))]:
        mask.update(range(r - va, r - va + 4))
    for ins in md.disasm(code, va):
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5 and ins.bytes[0] in (0xe8, 0xe9):
            tgt = int.from_bytes(ins.bytes[1:], 'little', signed=True) + ins.address + 5
            if not va <= tgt < va + len(code): mask.update(range(ins.address - va + 1, ins.address - va + 5))
    calls = []
    for ins in md.disasm(code, va):
        if ins.mnemonic == 'call' and ins.size == 5 and ins.bytes[0] == 0xe8:
            calls.append((ins.address - va, int.from_bytes(ins.bytes[1:], 'little', signed=True) + ins.address + 5))
    keep = [k for k in range(len(code)) if k not in mask]
    if len(keep) < 6: continue
    hits = []
    for st in range(0, len(cdata) - len(code), 16):
        if cdata[st + keep[0]] != code[keep[0]]: continue
        w = cdata[st:st + len(code)]
        if all(w[k] == code[k] for k in keep):
            nb = cdata[st + len(code):st + len(code) + 1]
            ok = True; known = unk = 0
            for r in relocs[bisect.bisect_left(relocs, va):bisect.bisect_left(relocs, va + len(code))]:
                ov = int.from_bytes(tdata[r - tva:r - tva + 4], 'little')
                rv = int.from_bytes(w[r - va:r - va + 4], 'little')
                if ov in dmap and len(dmap[ov]) == 1:
                    if rv in dmap[ov]: known += 1
                    else: ok = False
                else: unk += 1
            for off, tgt in calls:
                if tgt in o2r:
                    rt = int.from_bytes(w[off + 1:off + 5], 'little', signed=True) + cva + st + off + 5
                    if rt != o2r[tgt]: ok = False
            if ok and (nb == b'\xcc' or (cva + st + len(code)) % 16 == 0):
                hits.append((cva + st, known, unk))
        if len(hits) > 4: break
    if hits and len(hits) <= 4:
        print(len(code), len(keep), name, ' '.join('%x%s/k%d/u%d' % (h, '*' if h in covered else '', k, u) for h, k, u in hits))
