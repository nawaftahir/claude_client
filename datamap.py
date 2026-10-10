#!/usr/bin/env python3
"""datamap.py exp.json out.tsv: map our data symbols to retail addresses via reloc pairs of matched fns.
Row: retail_start our_start our_size retail_gap votes conflict name obj"""
import bisect, collections, json, re, sys
import pefile

R = '../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
B = 0x400000
rp, op = pefile.PE(R), pefile.PE('build/CoD2MP_s.exe')
rimg, oimg = rp.get_memory_mapped_image(), op.get_memory_mapped_image()
orel = sorted(B + e.rva for b in op.DIRECTORY_ENTRY_BASERELOC for e in b.entries if e.type == 3)
otext = next(s for s in op.sections if s.Name.startswith(b'.text'))
tlo, thi = B + otext.VirtualAddress, B + otext.VirtualAddress + otext.Misc_VirtualSize

fa, syms = {}, []
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*000(\d):\S+\s+(\S+)\s+([0-9a-f]{8})( f)?\s+(?:i\s+)?(\S+)?', l)
    if not m:
        continue
    a = int(m.group(3), 16)
    if m.group(1) == '1':
        if m.group(4):
            fa[m.group(2)] = a
    else:
        syms.append((a, m.group(2), m.group(5) or ''))
syms.sort()
sa = [s[0] for s in syms]

votes = collections.defaultdict(collections.Counter)
for x in json.load(open(sys.argv[1])):
    if len(x['hits']) != 1 or x['name'] not in fa:
        continue
    a, ra = fa[x['name']], int(x['hits'][0], 16)
    for r in orel[bisect.bisect_left(orel, a):bisect.bisect_left(orel, a + x['size'])]:
        ov = int.from_bytes(oimg[r - B:r - B + 4], 'little')
        rv = int.from_bytes(rimg[ra + (r - a) - B:ra + (r - a) - B + 4], 'little')
        if tlo <= ov < thi:
            continue
        k = bisect.bisect_right(sa, ov) - 1
        if k < 0:
            continue
        votes[k][rv - (ov - sa[k])] += 1

rows = []
for k, c in votes.items():
    (rs, n), = c.most_common(1)
    osz = (sa[k + 1] if k + 1 < len(sa) else sa[k]) - sa[k]
    rows.append([rs, sa[k], osz, 0, n, len(c) - 1, syms[k][1], syms[k][2]])
rows.sort()
for i, r in enumerate(rows):
    r[3] = rows[i + 1][0] - r[0] if i + 1 < len(rows) else 0
with open(sys.argv[2], 'w') as f:
    for r in rows:
        f.write('%x\t%x\t%d\t%d\t%d\t%d\t%s\t%s\n' % tuple(r))
same = sum(1 for r in rows if r[0] == r[1])
print('mapped %d of %d data syms; at retail addr %d; conflicting votes %d' % (len(rows), len(syms), same, sum(1 for r in rows if r[5])))
