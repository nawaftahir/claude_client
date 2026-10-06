#!/usr/bin/env python3
"""xref.py ADDR... : retail call sites and immediate refs to each address."""
import sys, struct, pefile
import os
pe = pefile.PE(os.environ.get("EXE") or __file__.rsplit("/", 1)[0] + "/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe", fast_load=True)
ib = pe.OPTIONAL_HEADER.ImageBase
secs = [(s.Name.rstrip(b'\0').decode(), ib + s.VirtualAddress, s.get_data()) for s in pe.sections]
for a in sys.argv[1:]:
    t = int(a, 16)
    calls, refs = [], []
    imm = struct.pack('<I', t)
    for name, base, d in secs:
        if name == '.text':
            for i in range(len(d) - 5):
                if d[i] in (0xe8, 0xe9) and base + i + 5 + struct.unpack('<i', d[i+1:i+5])[0] == t:
                    calls.append(hex(base + i))
        i = d.find(imm)
        while i != -1:
            refs.append('%s:%s' % (name, hex(base + i)))
            i = d.find(imm, i + 1)
    print(a, 'calls', calls, 'refs', refs)
