#!/usr/bin/env python3
"""Like vary.py, but each variant may set several lines: 'N=text' parts joined by ' |; '.
usage: varyn.py <mangled-prefix> <client-addr> <src-file> 'N=text |; M=text' ...   (keeps the best strict improvement)"""
import os, sys
from vary import score, T

SEP = ' |; '
name, addr, path = sys.argv[1], sys.argv[2], os.path.join(T, sys.argv[3])
orig = open(path).read().split('\n')
best, _ = score(name, addr)
print('base', best, flush=True)
keep = None
for v in sys.argv[4:]:
    L = orig[:]
    for part in v.split(SEP):
        n, t = part.split('=', 1)
        L[int(n) - 1] = t
    open(path, 'w').write('\n'.join(L))
    s, _ = score(name, addr)
    print('%5d  %s' % (s, v.replace('\t', '')), flush=True)
    if s < best: best, keep = s, L
open(path, 'w').write('\n'.join(keep or orig))
if keep: score(name, addr)
print('kept', best)
