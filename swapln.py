#!/usr/bin/env python3
"""Swap two whole source lines and score one fn; each pair is tried alone, the best strict improvement is kept.
usage: swapln.py <mangled-prefix> <client-addr> <src-file> A:B [A:B ...]   (run from the tree root, path under target/)"""
import os, sys
from vary import score, T

name, addr, path = sys.argv[1], sys.argv[2], os.path.join(T, sys.argv[3])
orig = open(path).read().split('\n')
open(path, 'w').write('\n'.join(orig))
best, _ = score(name, addr)
print('base', best, flush=True)
keep = None
for pr in sys.argv[4:]:
    a, b = (int(x) - 1 for x in pr.split(':'))
    L = orig[:]; L[a], L[b] = L[b], L[a]
    open(path, 'w').write('\n'.join(L))
    s, _ = score(name, addr)
    print('%5d  %s' % (s, pr), flush=True)
    if s < best: best, keep = s, L
open(path, 'w').write('\n'.join(keep or orig))
if keep: score(name, addr)
print('kept', best)
