#!/usr/bin/env python3
# pops.py ra.txt: allocator pop order (P lines of cave_ra.s) with object kind/value and sym name
import sys
s = lambda v: v - (1 << 32) if v & 0x80000000 else v
k = 0
for line in open(sys.argv[1]):
    w = line.split()
    if w[0] != 'P': continue
    n = [int(x, 16) for x in w[1:19]]; k += 1
    o = [int(x, 16) for x in w[20:34]] if len(w) > 33 else [0] * 14
    nm = line.split(' = ')[1].strip() if ' = ' in line else ''
    kind = o[1] & 0xff
    desc = nm or ('temp def=%08x' % o[10] if kind == 0x0d else 'kind%02x(%08x) val=%d' % (kind, o[1], s(o[7])))
    print('%3d id=%3d fl=%02x n3=%5d n16=%4d n17=%4d +24=%2d  %s' % (k, n[7], n[1] >> 8 & 0xff, s(n[3]), s(n[16]), n[17], n[9], desc))
