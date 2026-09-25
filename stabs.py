#!/usr/bin/env python3
"""Dump STABS from the i386 slice of the Mac 1.3 client: functions per source file, and type stabs."""
import json, struct, sys

MAC = '/home/nawaf/cod2/test/Refrences/CoD2x/bin/mac/Call of Duty 2 Multiplayer 1.3'
N_GSYM, N_FUN, N_STSYM, N_LCSYM, N_SO, N_SOL, N_LSYM = 0x20, 0x24, 0x26, 0x28, 0x64, 0x84, 0x80

d = open(MAC, 'rb').read()
nfat = struct.unpack('>I', d[4:8])[0]
for i in range(nfat):
    cpu, _, off, size, _ = struct.unpack('>5I', d[8 + i * 20:28 + i * 20])
    if cpu == 7:  # CPU_TYPE_X86
        m = d[off:off + size]
ncmds = struct.unpack('<I', m[16:20])[0]
p = 28
for _ in range(ncmds):
    cmd, sz = struct.unpack('<II', m[p:p + 8])
    if cmd == 2:  # LC_SYMTAB
        symoff, nsyms, stroff, _ = struct.unpack('<4I', m[p + 8:p + 24])
    p += sz

def name(x):
    return m[stroff + x:m.index(b'\0', stroff + x)].decode('latin1')

files, types, cur = {}, {}, None
for k in range(nsyms):
    strx, typ, sect, desc, val = struct.unpack('<IBBhI', m[symoff + k * 12:symoff + k * 12 + 12])
    if not typ & 0xe0:
        continue
    s = name(strx)
    if typ == N_SO and s and not s.endswith('/'):
        cur = s
        files.setdefault(cur, [])
    elif typ == N_FUN and s and cur:
        files[cur].append([s.split(':')[0], hex(val)])
    elif typ == N_LSYM and (':T' in s or ':t' in s):
        types.setdefault(s.split(':')[0], s)

out = sys.argv[1] if len(sys.argv) > 1 else 'mac_stabs.json'
json.dump({'files': files, 'types': types}, open(out, 'w'), indent=0)
print(len(files), 'files', sum(len(v) for v in files.values()), 'functions', len(types), 'types')
