#!/usr/bin/env python3
# perm.py <mangled> <addr> <file> <firstline> <lastline> : try all permutations of a decl block, keep best
import itertools, subprocess, sys, os
D='/home/nawaf/cod2/test/dev/claude_client'
sys.path.insert(0, D)
from vary import score
name, addr, f, a, b = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5])
L = open(f).read().split('\n')
orig = L[a-1:b]
best, bs = orig, score(name, addr)[0]
print('base', bs, flush=True)
perms = list(itertools.permutations(orig))[1:]
lim = int(os.environ.get('LIM', '40'))
for p in perms[:lim]:
    L[a-1:b] = list(p)
    open(f, 'w').write('\n'.join(L))
    s = score(name, addr)[0]
    print(s, [x.strip() for x in p], flush=True)
    if s < bs:
        best, bs = list(p), s
    if bs == 0:
        break
L[a-1:b] = best
open(f, 'w').write('\n'.join(L))
print('best', bs, [x.strip() for x in best])
