#!/usr/bin/env python3
# rdata.py <va> [count]: dump dwords from the client image at va
import struct, sys
b = open(__file__.rsplit('/', 1)[0] + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe', 'rb').read()
pe = struct.unpack_from('<I', b, 0x3c)[0]
n, so = struct.unpack_from('<H', b, pe + 6)[0], struct.unpack_from('<H', b, pe + 20)[0]
va, cnt = int(sys.argv[1], 16), int(sys.argv[2]) if len(sys.argv) > 2 else 1
for k in range(cnt):
    a = va + 4 * k
    for i in range(n):
        o = pe + 24 + so + 40 * i
        vs, sva, rs, ro = struct.unpack_from('<IIII', b, o + 8)
        sva += 0x400000
        if sva <= a < sva + vs:
            off = a - sva
            print(hex(a), hex(struct.unpack_from('<I', b, ro + off)[0]) if off < rs else 'bss')
