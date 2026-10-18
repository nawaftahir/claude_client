#!/usr/bin/env python3
# patch_q.py c2.dll cave_q.elf out.dll: log F + inline-revert test (Q callee, r result, q 1/0 count thr)
import struct, subprocess, sys
b = bytearray(open(sys.argv[1], 'rb').read())
elf = sys.argv[2]
sym = {l.split()[2]: int(l.split()[0], 16) for l in subprocess.check_output(['nm', elf], text=True).splitlines()}
cave = subprocess.run(['objcopy', '-O', 'binary', '-j', '.text', elf, '/dev/stdout'], capture_output=True).stdout
assert len(cave) < 688, len(cave)
b[0x165d50:0x165d50 + len(cave)] = cave
def jmp(off, dst, n):
    b[off] = 0xe9
    struct.pack_into('<i', b, off + 1, dst - (0x10700000 + off + 5))
    b[off + 5:off + n] = b'\x90' * (n - 5)
jmp(0x724f6, sym['cave1'], 6)
jmp(0x285bb, sym['caveQ'], 13)
jmp(0x28582, sym['caveA'], 10)
jmp(0x28596, sym['caveB'], 6)
open(sys.argv[3], 'wb').write(b)
