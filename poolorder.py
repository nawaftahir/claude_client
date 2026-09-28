#!/usr/bin/env python3
"""LTCG emits __real@ constants in codegen order: list the constant pool with the functions that reference each
entry, for the client and for our build, to compare function codegen order. run in target/ after a build.
usage: poolorder.py [client|ours]   -> one line per pool entry: addr value referencing-fns"""
import bisect, json, re, struct, sys
import capstone, pefile

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
ABS = re.compile(r'\[(0x[0-9a-f]+)\]')

def refs(pe, starts, names, lo, hi):
    """pool address -> set of function names referencing it"""
    base = pe.OPTIONAL_HEADER.ImageBase
    t = next(s for s in pe.sections if s.Name.startswith(b'.text'))
    data, tva = t.get_data(), base + t.VirtualAddress
    out = {}
    for k, a in enumerate(starts):
        end = starts[k + 1] if k + 1 < len(starts) else tva + len(data)
        for ins in md.disasm(data[a - tva:end - tva], a):
            for m in ABS.finditer(ins.op_str):
                v = int(m.group(1), 16)
                if lo <= v < hi:
                    out.setdefault(v, set()).add(names[k])
    return out

def client():
    pe = pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
    fn = {}
    for p in ('../ghidra/names_1.0.tsv', '../ghidra/names_loc.tsv'):
        for l in open(p):
            a, n = l.split('\t')[:2]
            fn[int(a, 16)] = n
    for x in json.load(open('build/cmp.json')):
        if len(x['hits']) == 1:
            fn[int(x['hits'][0], 16)] = re.sub(r'^\?(\w+)@.*', r'\1', x['name'])
    starts = sorted(fn)
    return pe, refs(pe, starts, [fn[a] for a in starts], 0x582e00, 0x583900)

def ours():
    pe = pefile.PE('build/CoD2MP_s.exe')
    fn = {}
    for l in open('build/CoD2MP_s.map', errors='replace'):
        m = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l)
        if m: fn[int(m.group(2), 16)] = re.sub(r'^\?(\w+)@.*', r'\1', m.group(1))
    pool = [int(m.group(1), 16) for m in (re.match(r'\s*0002:\S+\s+__real@\w+\s+([0-9a-f]{8})', l)
            for l in open('build/CoD2MP_s.map')) if m]
    pool = [a for a in pool if a > 0x570000]
    starts = sorted(fn)
    return pe, refs(pe, starts, [fn[a] for a in starts], min(pool), max(pool) + 8)

if __name__ == '__main__':
    pe, r = (client if sys.argv[1:2] == ['client'] else ours)()
    for a in sorted(r):
        o = pe.get_offset_from_rva(a - pe.OPTIONAL_HEADER.ImageBase)
        print('%x %08x %s' % (a, struct.unpack_from('<I', pe.__data__, o)[0], ' '.join(sorted(r[a]))))
