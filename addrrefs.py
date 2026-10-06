#!/usr/bin/env python3
# usage: addrrefs.py HEXADDR... -- absolute 4-byte references to each address in the retail client (all sections)
import sys, pefile
pe = pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
for a in sys.argv[1:]:
    v = int(a, 16); b = v.to_bytes(4, 'little'); hits = []
    for s in pe.sections:
        d = s.get_data(); i = d.find(b)
        while i >= 0:
            hits.append('%s:%x' % (s.Name.rstrip(b'\0').decode(), pe.OPTIONAL_HEADER.ImageBase + s.VirtualAddress + i)); i = d.find(b, i + 1)
    print(a, hits)
