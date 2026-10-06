#!/usr/bin/env python3
"""Rebuild the client's icon file from a local retail CoD2MP_s.exe (never committed).
usage: rsrc.py <retail exe> <out.ico>   -> ICO with the RT_GROUP_ICON 1 entries in retail order"""
import struct, sys

d = open(sys.argv[1], 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]
ns, osz = struct.unpack_from('<H', d, pe + 6)[0], struct.unpack_from('<H', d, pe + 20)[0]
for i in range(ns):
    o = pe + 24 + osz + 40 * i
    if d[o:o + 8].rstrip(b'\0') == b'.rsrc':
        va, ra = struct.unpack_from('<I', d, o + 12)[0], struct.unpack_from('<I', d, o + 20)[0]


def leaves(off, path):
    nn, ni = struct.unpack_from('<HH', d, ra + off + 12)
    for k in range(nn + ni):
        idn, ent = struct.unpack_from('<II', d, ra + off + 16 + 8 * k)
        if ent & 0x80000000:
            yield from leaves(ent & 0x7fffffff, path + (idn,))
        else:
            dva, dsz = struct.unpack_from('<II', d, ra + ent)
            yield path + (idn,), d[ra + dva - va:ra + dva - va + dsz]


res = {p[:2]: b for p, b in leaves(0, ())}
grp = res[(14, 1)]
n = struct.unpack_from('<H', grp, 4)[0]
ents, imgs, off = [], [], 6 + 16 * n
for k in range(n):
    w, h, c, r, planes, bits, size, iid = struct.unpack_from('<BBBBHHIH', grp, 6 + 14 * k)
    img = res[(3, iid)]
    ents.append(struct.pack('<BBBBHHII', w, h, c, r, planes, bits, len(img), off))
    imgs.append(img)
    off += len(img)
open(sys.argv[2], 'wb').write(grp[:6] + b''.join(ents) + b''.join(imgs))
