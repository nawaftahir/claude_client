#!/usr/bin/env python3
"""Pair unmatched fns whose anchor is too short for cmp.py's search with uncovered retail slots.
usage: shortpair.py exe map retail exp.json [twins|nb]  -> prints names_fix rows (unique, callee-checked)"""
import bisect, json, re, sys
import pefile, capstone

img, mapf, target, expf = sys.argv[1:5]
TW = sys.argv[5:] == ['twins']  # list uncovered extra copies of matched fns too
NB = sys.argv[5:] == ['nb']  # tiny fns: only try the slot right after our previous fn's retail hit
pe = pefile.PE(img)
base = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections if s.Name.startswith(b'.text'))
tdata, tva = text.get_data(), base + text.VirtualAddress
relocs = sorted({base + e.rva for b in getattr(pe, 'DIRECTORY_ENTRY_BASERELOC', []) for e in b.entries if e.type == 3})
cpe = pefile.PE(target)
ctext = next(s for s in cpe.sections if s.Name.startswith(b'.text'))
cdata, cva = ctext.get_data(), cpe.OPTIONAL_HEADER.ImageBase + ctext.VirtualAddress

syms = []
for line in open(mapf, errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if m:
        syms.append((int(m.group(2), 16), m.group(1)))
syms.sort()
exp = json.load(open(expf))
hit = {x['name']: x['hits'] for x in exp}
cov = bytearray(len(cdata))
for x in exp:
    if len(x['hits']) == 1:
        a = int(x['hits'][0], 16) - cva
        cov[a:a + x['size']] = b'\1' * x['size']
ouraddr = {n: a for a, n in syms}
symva = [a for a, _ in syms]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

for i, (va, name) in enumerate(syms):
    if hit.get(name) and not TW:
        continue
    end = syms[i + 1][0] if i + 1 < len(syms) else tva + len(tdata)
    code = tdata[va - tva:end - tva].rstrip(b'\xcc')
    if len(code) < 6:
        continue
    mask = set()
    for r in relocs[bisect.bisect_left(relocs, va):bisect.bisect_left(relocs, va + len(code))]:
        mask.update(range(r - va, r - va + 4))
    calls = []
    for ins in md.disasm(code, va):
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5 and ins.bytes[0] in (0xe8, 0xe9):
            tgt = int.from_bytes(ins.bytes[1:], 'little', signed=True) + ins.address + 5
            if not va <= tgt < va + len(code):
                mask.update(range(ins.address - va + 1, ins.address - va + 5))
                calls.append((ins.address - va, tgt))
    if len(code) - len(mask) < (8 if TW else 2 if NB else 4):
        continue
    found = []
    slots = range(0, len(cdata) - len(code), 16)
    if NB:
        ph = hit.get(syms[i - 1][1]) if i else None
        if not ph or len(ph) != 1:
            continue
        pe_ = int(ph[0], 16) - cva + (va - syms[i - 1][0])
        slots = [pe_] if pe_ % 16 == 0 else []
    for off in slots:
        if any(cov[off:off + len(code)]):
            continue
        w = cdata[off:off + len(code)]
        if not all(w[k] == code[k] for k in range(len(code)) if k not in mask):
            continue
        nb = cdata[off + len(code):off + len(code) + 1]
        if nb != b'\xcc' and (off + len(code)) % 16:
            continue
        ok = True
        for co, tgt in calls:
            j = bisect.bisect_right(symva, tgt) - 1
            cn = syms[j][1] if j >= 0 and syms[j][0] == tgt else None
            h = hit.get(cn) if cn else None
            if h and len(h) == 1:
                rt = int.from_bytes(cdata[off + co + 1:off + co + 5], 'little', signed=True) + cva + off + co + 5
                if rt != int(h[0], 16):
                    ok = False
        if ok:
            found.append(cva + off)
        if len(found) > 3:
            break
    if len(found) == 1:
        print('%x\t%s\tfix\t# shortpair %d B' % (found[0], name, len(code)))
    elif found:
        print('# ambiguous %s %d B: %s' % (name, len(code), ' '.join(hex(f) for f in found)))
