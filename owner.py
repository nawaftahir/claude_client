#!/usr/bin/env python3
"""owner.py ADDR...: retail function (Ghidra export start/name) containing each address."""
import os, sys, bisect
D = os.path.dirname(os.path.abspath(__file__))
fs = sorted((int(f[:8], 16), f[9:-2]) for f in os.listdir(D + '/ghidra/out') if f.endswith('.c'))
st = [a for a, _ in fs]
for x in sys.argv[1:]:
    i = bisect.bisect_right(st, int(x, 16)) - 1
    print(x, hex(fs[i][0]), fs[i][1])
