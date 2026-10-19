#!/usr/bin/env python3
"""Whole-file distance to retail: shameter.py ours.exe retail.exe
Per section: raw size, virtual size, bytes identical at the same offset. Plus headers and overall."""
import hashlib, sys
import pefile

a, b = sys.argv[1], sys.argv[2]
da, db = open(a, 'rb').read(), open(b, 'rb').read()
pa, pb = pefile.PE(data=da), pefile.PE(data=db)


def same(x, y):
    n = min(len(x), len(y))
    return sum(1 for i in range(n) if x[i] == y[i])


print('file   ours %9d  retail %9d  sha %s' % (len(da), len(db), 'IDENTICAL' if hashlib.sha256(da).digest() == hashlib.sha256(db).digest() else 'differs'))
h = pb.OPTIONAL_HEADER.SizeOfHeaders
print('header %-28s same %d/%d' % ('', same(da[:h], db[:h]), h))
sb = {s.Name.rstrip(b'\0').decode(): s for s in pb.sections}
tot = tsame = 0
for s in pa.sections:
    n = s.Name.rstrip(b'\0').decode(); r = sb.get(n)
    if not r:
        print('%-6s only in ours' % n); continue
    x, y = s.get_data(), r.get_data()
    k = same(x, y); tot += len(y); tsame += k
    print('%-6s raw %8d/%8d  vsize %8d/%8d  va %08x/%08x  same %8d (%.2f%%)' % (
        n, len(x), len(y), s.Misc_VirtualSize, r.Misc_VirtualSize,
        s.VirtualAddress, r.VirtualAddress, k, 100.0 * k / max(1, len(y))))
for n in sb:
    if n not in {s.Name.rstrip(b'\0').decode() for s in pa.sections}:
        print('%-6s only in retail' % n)
print('ALL sections same-offset %d/%d = %.3f%%' % (tsame, tot, 100.0 * tsame / tot))
