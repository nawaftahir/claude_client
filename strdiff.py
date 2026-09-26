#!/usr/bin/env python3
"""String literals in our exe that the client lacks: leftover debug prints / wrong text."""
import re, sys, os
D = os.path.dirname(os.path.abspath(__file__))
def strs(p):
    return set(m.group().decode('latin1') for m in re.finditer(rb'[\x20-\x7e\t\n]{6,}', open(p, 'rb').read()))
ours = strs(D + '/target/build/CoD2MP_s.exe')
cl = strs(D + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
for s in sorted(ours - cl):
    print(repr(s))
