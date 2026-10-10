#!/usr/bin/env python3
"""tabdump.py OURVA RETAILVA [n] [stride]: dump dword tables side by side (strings shown by content). Run in target/."""
import struct, sys
import pefile
B = 0x400000
imgs = [pefile.PE(p).get_memory_mapped_image() for p in ('build/CoD2MP_s.exe', '../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')]
a = [int(sys.argv[1], 16), int(sys.argv[2], 16)]
n = int(sys.argv[3]) if len(sys.argv) > 3 else 24
st = int(sys.argv[4]) if len(sys.argv) > 4 else 4


def show(img, v):
    if 0x559000 <= v < 0x1900000 and 0 <= v - B < len(img):
        e = img.find(b'\0', v - B, v - B + 40)
        s = img[v - B:e]
        if s and all(32 <= c < 127 or c >= 0xa0 for c in s):
            return '"%s"' % s.decode('latin1')
    return hex(v)


for k in range(n):
    row = []
    for i in (0, 1):
        x = a[i] + k * st
        row.append(' '.join(show(imgs[i], struct.unpack_from('<I', imgs[i], x - B + 4 * j)[0]) for j in range(st // 4)))
    print('%3d %-40s | %s' % (k, row[0][:40], row[1][:40]))
