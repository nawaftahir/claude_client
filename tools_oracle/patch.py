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
jmp(0x7079a, sym['cave2'], 6)  # X: callee referenced
jmp(0xce78, sym['cave3'], 6)   # D: DFS enter node, T: its edges in order
jmp(0xce9b, sym['cave4'], 7)   # C: recursive arc (cut edge)
jmp(0xcedd, sym['cave5'], 6)   # E: DFS leave node
jmp(0x8d026, sym['cave6'], 7)  # I/V: inline decision per call site
open(sys.argv[3], 'wb').write(b)
