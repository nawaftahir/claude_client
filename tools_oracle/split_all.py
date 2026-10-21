#!/usr/bin/env python3
# split_all.py all.txt outdir names...: write each named fn's F/P/C block (RA_FN='*' log) to outdir/<n>.txt
import sys, os
want = set(sys.argv[3:]); out = None; k = 0
os.makedirs(sys.argv[2], exist_ok=True)
for line in open(sys.argv[1], errors='replace'):
    if line.startswith('F '):
        name = line[2:].strip()
        if out: out.close(); out = None
        if name in want:
            k += 1; out = open(os.path.join(sys.argv[2], 'f%03d.txt' % sorted(want).index(name)), 'w')
    if out: out.write(line)
if out: out.close()
print(k, 'fns written')
