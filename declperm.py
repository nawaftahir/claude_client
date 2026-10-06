#!/usr/bin/env python3
"""Hill-climb local declaration order of one fn (stack slot layout).
usage: declperm.py <mangled-prefix> <client-addr> <src-file> <first-decl-line> <last-decl-line>
Each line in the range must be one declaration. Tries move-to-front/back, keeps strict improvements."""
import os, sys
from vary import score, T

name, addr, path, a, b = sys.argv[1], sys.argv[2], os.path.join(T, sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
src = open(path).read().split('\n')
decl = src[a - 1:b]

def put(d):
    open(path, 'w').write('\n'.join(src[:a - 1] + d + src[b:]))

put(decl)
best, _ = score(name, addr)
print('base', best, flush=True)
improved = True
while improved and best:
    improved = False
    for i in range(len(decl)):
        for j in (0, len(decl) - 1):
            if i == j: continue
            d = decl[:]
            d.insert(j, d.pop(i))
            put(d)
            s, _ = score(name, addr)
            print('try', s, decl[i].strip(), '->', j, flush=True)
            if s < best:
                best, decl, improved = s, d, True
                print('keep', s, decl[j].strip(), '->', j, flush=True)
                break
        if improved: break
put(decl)
print('final', best)
