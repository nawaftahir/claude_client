#!/usr/bin/env python3
"""Per-site LTCG inline decisions from the oracle log (F/I/V lines).
usage: inl.py ord.txt CALLER-substr  -> each decision: result instrs depth maxdepth budget [size81 callers flags20 callersize] callee"""
import sys
cur, show, name = None, False, None
for l in open(sys.argv[1], errors='replace'):
    t = l[:2]
    if t == 'F ': cur = l[2:].strip(); show = sys.argv[2] in cur; show and print('F', cur)
    elif t == 'I ': name = l[2:].strip()
    elif l[:2] == 'V ' and show:
        v = [int(x, 16) for x in l.split()[1:]]
        r, n, d, md, b = v[:5]; ext = v[5:]
        print(' ', 'IN ' if r else 'no ', n, d, md, b, *ext, name[:60])
