#!/usr/bin/env python3
# patch_ra.py c2.dll cave.elf out.dll: hook F (always) + N (if the cave has caveN); used for cave_ra.s and cave_rb.s
# build: as --32 cave_ra.s -o cave_ra.o; ld -m elf_i386 -Ttext=0x10865d50 cave_ra.o -o cave_ra.elf
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
jmp(0x724f6, sym['caveF'], 6)
if 'caveN' in sym:
    jmp(0x84d3b, sym['caveN'], 6)
if 'caveP' in sym:
    jmp(0x82a5e, sym['caveP'], 5)
for k, site in enumerate([0x107ef8f7, 0x108166c1, 0x10816735, 0x1083d73d, 0x1083dc83]):
    if 'caveW%d' % k in sym:
        jmp(site - 0x10700000, sym['caveW%d' % k], 7)
open(sys.argv[3], 'wb').write(b)
