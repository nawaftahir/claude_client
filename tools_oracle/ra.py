#!/usr/bin/env python3
# ra.py ra.txt [ra1.bin LOCAL]: group cave_ra N lines by object; with a cave_rb snapshot, objects
# that are a local's leaf (sym+0x10) get the local's name. Field meanings: see REGALLOC.md.
import struct, sys
from collections import OrderedDict
names = {}
if len(sys.argv) > 3:
    sys.path.insert(0, '/home/nawaf/cod2/test/dev/lab/s6/target/build/vc/ra')
    from snap import Snap
    a = Snap(sys.argv[2])
    for base, sz, d in a.regs:
        for i in range(0, sz - 0x60, 4):
            np = struct.unpack_from('<I', d, i + 4)[0]
            if base + i - 0x40 < np < base + i and d[np - base:np - base + 1] == b'_':
                leaf = struct.unpack_from('<I', d, i + 0x10)[0]
                if leaf: names.setdefault(leaf, a.cstr(np))
s = lambda v: v - (1 << 32) if v & 0x80000000 else v
groups = OrderedDict()
for line in open(sys.argv[1]):
    w = line.split()
    if w[0] == 'F':
        print(line.strip()); continue
    n = [int(x, 16) for x in w[1:19]]
    o = [int(x, 16) for x in w[20:32]]
    groups.setdefault(n[0], (o, []))[1].append(n)
for obj, (o, ns) in groups.items():
    kind = o[1] & 0xff
    nm = names.get(obj) or ('temp' if kind == 0x0d else 'kind%02x val=%d' % (kind, s(o[7])))
    print('%-22s n17=%4d  nodes: %s' % (nm, ns[0][17], ' '.join('[%02x %d/%d]' % (n[1] >> 8 & 0xff, s(n[3]), s(n[16])) for n in ns)))
