#!/usr/bin/env python3
"""bindcheck.py <img> <map> <retail> <cmp.json> [out.json]
Symbol-binding audit for masked matches: for each function hit, every external call/jmp rel32 and every
absolute pointer into .text must land, in retail, where the referenced function of ours uniquely matched.
Verdict per (fn, hit): ok (all checkable refs agree), bad (a ref lands elsewhere), unk (refs unmatched)."""
import bisect, json, re, sys
import pefile, capstone

img, mapf, target, cmpf = sys.argv[1:5]
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
starts = [s[0] for s in syms]
cmp = {e['name']: e for e in json.load(open(cmpf))}
# where each of our functions sits in retail, only when unambiguous
where = {n: int(e['hits'][0], 16) for n, e in cmp.items() if len(e['hits']) == 1}

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def u32(b, o): return int.from_bytes(b[o:o + 4], 'little')
out, tot = [], {'ok': 0, 'bad': 0, 'unk': 0}
for i, (va, name) in enumerate(syms):
    e = cmp.get(name)
    if not e or not e['hits']:
        continue
    end = syms[i + 1][0] if i + 1 < len(syms) else tva + len(tdata)
    code = tdata[va - tva:end - tva].rstrip(b'\xcc')
    refs = []  # (offset in fn, our target VA, kind)
    for ins in md.disasm(code, va):
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5 and ins.bytes[0] in (0xe8, 0xe9):
            tgt = int.from_bytes(ins.bytes[1:], 'little', signed=True) + ins.address + 5
            if not va <= tgt < va + len(code):
                refs.append((ins.address - va + 1, tgt, 'rel'))
    for r in relocs[bisect.bisect_left(relocs, va):bisect.bisect_left(relocs, va + len(code))]:
        t = u32(tdata, r - tva)
        if tva <= t < tva + len(tdata) and not va <= t < va + len(code):
            refs.append((r - va, t, 'abs'))
    for h in e['hits']:
        h = int(h, 16)
        verdict = 'ok' if refs else 'unk'
        checked = 0
        for off, tgt, kind in refs:
            k = bisect.bisect_right(starts, tgt) - 1
            sname = syms[k][1]
            if sname not in where:
                continue
            want = where[sname] + (tgt - syms[k][0])
            p = h + off - cva
            got = (int.from_bytes(cdata[p:p + 4], 'little', signed=True) + h + off + 4) if kind == 'rel' else u32(cdata, p)
            checked += 1
            if got != want:
                verdict = 'bad'
                break
        if verdict == 'ok' and not checked:
            verdict = 'unk'
        tot[verdict] += 1
        out.append({'name': name, 'hit': hex(h), 'size': len(code), 'refs': len(refs), 'checked': checked, 'v': verdict})
print(tot)
if len(sys.argv) > 5:
    json.dump(out, open(sys.argv[5], 'w'), indent=0)
