#!/usr/bin/env python3
# patch.py c2.dll cave.elf out.dll: hook F/X (compile order) + D/T/E/C (call-graph DFS) into c2.dll
# cave.elf: as --32 cave.s; ld -m elf_i386 -Ttext=0x10865d50 (cave written at file off 0x165d50)
import struct, subprocess, sys
b = bytearray(open(sys.argv[1], 'rb').read())
elf = sys.argv[2]
sym = {l.split()[2]: int(l.split()[0], 16) for l in subprocess.check_output(['nm', elf], text=True).splitlines()}
cave = subprocess.run(['objcopy', '-O', 'binary', '-j', '.text', elf, '/dev/stdout'], capture_output=True).stdout
assert len(cave) < 688
b[0x165d50:0x165d50 + len(cave)] = cave
def jmp(off, dst, n):
    b[off] = 0xe9
    struct.pack_into('<i', b, off + 1, dst - (0x10700000 + off + 5))
    b[off + 5:off + n] = b'\x90' * (n - 5)
jmp(0x724f6, sym['cave1'], 6)  # F: compile start
jmp(0x8d026, sym['cave6'], 7)  # I/V: inline decision per call site
jmp(0x8da56, sym['cave7'], 6)  # G: perform-inline entry flags
jmp(0x8d4f4, sym['cave8'], 10)  # R: perform-inline result
open(sys.argv[3], 'wb').write(b)
