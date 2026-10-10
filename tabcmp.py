#!/usr/bin/env python3
"""tabcmp.py exp.json [maxent]: walk data tables reached from matched fns (statics included, the map
omits them) and compare entry by entry with retail: string pointers by content, code pointers via
matched fns, other dwords by value. Prints tables whose first entries differ. Run in target/."""
import bisect, collections, json, re, struct, sys
import pefile

R = '../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
B = 0x400000
rp, op = pefile.PE(R), pefile.PE('build/CoD2MP_s.exe')
rimg, oimg = rp.get_memory_mapped_image(), op.get_memory_mapped_image()
MAXE = int(sys.argv[2]) if len(sys.argv) > 2 else 64


def sec(pe, name):
    s = next(s for s in pe.sections if s.Name.rstrip(b'\0') == name)
    return B + s.VirtualAddress, B + s.VirtualAddress + s.Misc_VirtualSize


otext, rtext = sec(op, b'.text'), sec(rp, b'.text')
odata = (sec(op, b'.rdata')[0], sec(op, b'.data')[1])
rdata = (sec(rp, b'.rdata')[0], sec(rp, b'.data')[1])
orel = sorted(B + e.rva for b in op.DIRECTORY_ENTRY_BASERELOC for e in b.entries if e.type == 3)
orelset = set(orel)

fa = {}
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l)
    if m:
        fa[m.group(1)] = int(m.group(2), 16)
o2r, pairs = {}, collections.Counter()
for x in json.load(open(sys.argv[1])):
    if len(x['hits']) != 1 or x['name'] not in fa:
        continue
    a, ra = fa[x['name']], int(x['hits'][0], 16)
    o2r[a] = ra
    for r in orel[bisect.bisect_left(orel, a):bisect.bisect_left(orel, a + x['size'])]:
        ov = struct.unpack_from('<I', oimg, r - B)[0]
        rv = struct.unpack_from('<I', rimg, ra + (r - a) - B)[0]
        if odata[0] <= ov < odata[1] and rdata[0] <= rv < rdata[1]:
            pairs[(ov, rv)] += 1


def cstr(img, a):
    o = a - B
    e = img.find(b'\0', o, o + 512)
    return img[o:e] if e >= 0 else None


def dw(img, a):
    return struct.unpack_from('<I', img, a - B)[0]


seen = set()
for (ov, rv), n in sorted(pairs.items()):
    if ov in seen:
        continue
    seen.add(ov)
    bad, nptr = [], 0
    for k in range(MAXE):
        oa, ra = ov + 4 * k, rv + 4 * k
        o, r = dw(oimg, oa), dw(rimg, ra)
        if k and (oa in seen):
            break
        if oa in orelset:  # ours holds a pointer here
            nptr += 1
            if otext[0] <= o < otext[1]:
                if o2r.get(o) is not None and o2r[o] != r:
                    bad.append((k, 'fn', hex(o), hex(r)))
            elif odata[0] <= o < odata[1]:
                so, sr = cstr(oimg, o), cstr(rimg, r) if rdata[0] <= r < rdata[1] else None
                printable = lambda x: x and all(32 <= c < 127 or c >= 0xa0 for c in x)
                if so is not None and sr is not None and so != sr and (printable(so) or printable(sr)):
                    bad.append((k, 'str', so[:24], sr[:24]))
        elif o != r:
            break  # plain value or pointer-count drift: table end or layout shift
        if o == 0 and r == 0 and k and nptr:
            break
    if bad and nptr >= 2:
        print('%x -> %x (%d refs, %d ptrs): %s' % (ov, rv, n, nptr, bad[:4]))
