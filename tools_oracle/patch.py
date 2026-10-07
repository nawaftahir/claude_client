#!/usr/bin/env python3
# patch.py c2.dll cave.bin out.dll: hook F/X (compile order) + D/E/C (call-graph DFS) into c2.dll
# cave.bin: as --32 cave.s; ld -m elf_i386 -Ttext=0x10865d50; objcopy -O binary -j .text
# label VAs below must match `nm` of the linked cave
import struct, sys
b = bytearray(open(sys.argv[1], 'rb').read())
cave = open(sys.argv[2], 'rb').read()
b[0x165d50:0x165d50 + len(cave)] = cave
def jmp(off, dst, n):
    b[off] = 0xe9
    struct.pack_into('<i', b, off + 1, dst - (0x10700000 + off + 5))
    b[off + 5:off + n] = b'\x90' * (n - 5)
jmp(0x724f6, 0x10865d50, 6)  # F: compile start
jmp(0x7079a, 0x10865d6f, 6)  # X: callee referenced
jmp(0xce78, 0x10865d8e, 6)   # D: DFS enter node
jmp(0xce9b, 0x10865dae, 7)   # C: recursive arc (cut edge)
jmp(0xcedd, 0x10865dd2, 6)   # E: DFS leave node
open(sys.argv[3], 'wb').write(b)
