#!/usr/bin/env python3
"""Fuzzy-locate our functions in the client: unmasked 12-byte windows vote for (client offset - window offset)."""
import collections, json, re, sys
import pefile, capstone

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
W = 12

def text(path):
    pe = pefile.PE(path)
    t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    return pe, t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress

def functions(img, mapf):
    pe, d, base = text(img)
    ib = pe.OPTIONAL_HEADER.ImageBase
    relocs = {ib + e.rva for b in getattr(pe, 'DIRECTORY_ENTRY_BASERELOC', []) for e in b.entries if e.type == 3}
    syms = sorted((int(m.group(2), 16), m.group(1), l.split()[-1]) for m, l in
                  ((re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l), l) for l in open(mapf, errors='replace')) if m)
    for i, (va, name, obj) in enumerate(syms):
        end = syms[i + 1][0] if i + 1 < len(syms) else base + len(d)
        code = d[va - base:end - base].rstrip(b'\xcc')
        mask = set()
        for r in relocs:
            if va <= r < va + len(code):
                mask.update(range(r - va, r - va + 4))
        for ins in md.disasm(code, va):
            if ins.size == 5 and ins.bytes[0] in (0xe8, 0xe9):
                mask.update(range(ins.address - va + 1, ins.address - va + 5))
        yield name, obj, va, code, mask

class Index:
    def __init__(self, data):
        self.data = data
        self.ix = collections.defaultdict(list)
        for k in range(0, len(data) - W):
            self.ix[data[k:k + W]].append(k)

    def vote(self, code, mask):
        v = collections.Counter()
        for k in range(0, len(code) - W, 2):
            if any(j in mask for j in range(k, k + W)):
                continue
            hits = self.ix.get(code[k:k + W], ())
            if 0 < len(hits) <= 4:
                for h in hits:
                    v[h - k] += 1
        return v.most_common(1)[0] if v else (None, 0)

if __name__ == '__main__':
    img, mapf, target, out = sys.argv[1:5]
    pe, cd, cbase = text(target)
    idx = Index(cd)
    res = {}
    for name, obj, va, code, mask in functions(img, mapf):
        if len(code) < 32:
            continue
        off, n = idx.vote(code, mask)
        if off is not None and n >= 3:
            res[name] = {'addr': hex(cbase + off), 'votes': n, 'size': len(code), 'obj': obj}
    json.dump(res, open(out, 'w'), indent=0)
    print(len(res), 'located')
