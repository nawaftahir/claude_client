#!/usr/bin/env python3
"""Find functions of a linked test image (map + .reloc) byte-identical in the client, relocations masked."""
import bisect, json, re, sys
import pefile, capstone

img, mapf, target = sys.argv[1:4]
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
        syms.append((int(m.group(2), 16), m.group(1), line.split()[-1]))
syms.sort()

_pairs = None
def pairs():
    # client address per plain name, same table priority as alldiff_lib
    global _pairs
    if _pairs is None:
        _pairs, drop = {}, set()
        g = __file__.rsplit('/', 1)[0] + '/ghidra/'
        for f in ('names_fix.tsv', 'names_k.tsv', 'names_1.0.tsv', 'names_cg.tsv', 'names_loc.tsv'):
            for l in open(g + f):
                r = l.rstrip('\n').split('\t')
                if len(r) > 2 and r[2] == 'drop': drop.add(r[1]); continue
                if len(r) >= 2:
                    a = int(r[0], 16)
                    if r[1] not in _pairs or (_pairs[r[1]] % 16 and not a % 16): _pairs[r[1]] = a
        for k in drop: _pairs.pop(k, None)
    return _pairs

def plain(m):
    if m.startswith('?'):
        p = m[1:].split('@@')[0].split('@')
        if len(p) >= 2 and p[1] and (p[1] + '::' + p[0]) in pairs(): return p[1] + '::' + p[0]
        return p[0]
    return m.lstrip('_')

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
res, stats = [], {'fns': 0, 'bytes': 0, 'match_fns': 0, 'match_bytes': 0}
for i, (va, name, obj) in enumerate(syms):
    end = syms[i + 1][0] if i + 1 < len(syms) else tva + len(tdata)
    code = tdata[va - tva:end - tva].rstrip(b'\xcc')
    if len(code) < 6:
        continue
    mask = set()
    for r in relocs[bisect.bisect_left(relocs, va):bisect.bisect_left(relocs, va + len(code))]:
        mask.update(range(r - va, r - va + 4))
    for ins in md.disasm(code, va):
        if ins.mnemonic in ('call', 'jmp') and ins.size == 5 and ins.bytes[0] in (0xe8, 0xe9):
            tgt = int.from_bytes(ins.bytes[1:], 'little', signed=True) + ins.address + 5
            if not va <= tgt < va + len(code):
                mask.update(range(ins.address - va + 1, ins.address - va + 5))
    # longest unmasked run as the search anchor
    best, s = (0, 0), 0
    for k in range(len(code) + 1):
        if k == len(code) or k in mask:
            if k - s > best[1] - best[0]:
                best = (s, k)
            s = k + 1
    stats['fns'] += 1
    stats['bytes'] += len(code)
    lit = code[best[0]:best[1]]
    hits = []
    # short anchors (call-dense fns) still need the full masked compare; cov.py counts unique placements only
    if len(lit) >= 8 or (len(lit) >= 4 and len(code) - len(mask) >= 16):
        p = cdata.find(lit)
        while p >= 0 and len(hits) < 8:
            st = p - best[0]
            w = cdata[st:st + len(code)]
            if st >= 0 and len(w) == len(code) and all(w[k] == code[k] for k in range(len(code)) if k not in mask):
                hits.append(hex(cva + st))
            p = cdata.find(lit, p + 1)
    paired = False
    if not hits and not (len(lit) >= 8 or (len(lit) >= 4 and len(code) - len(mask) >= 16)):
        # anchor too short to search: compare at the name-paired client address instead
        ca = pairs().get(plain(name))
        if ca is not None and (ca % 16 == 0 or ':' in obj):
            w = cdata[ca - cva:ca - cva + len(code)]
            nb = cdata[ca - cva + len(code):ca - cva + len(code) + 1]
            if len(w) == len(code) and (nb == b'\xcc' or (ca + len(code)) % 16 == 0) and \
                    all(w[k] == code[k] for k in range(len(code)) if k not in mask):
                hits, paired = [hex(ca)], True
    if hits:
        stats['match_fns'] += 1
        stats['match_bytes'] += len(code)
    res.append({'name': name, 'obj': obj, 'size': len(code), 'hits': hits, **({'paired': 1} if paired else {})})

json.dump(res, open(sys.argv[4] if len(sys.argv) > 4 else 'cmp.json', 'w'), indent=0)
print(stats, 'client .text', len(cdata))
