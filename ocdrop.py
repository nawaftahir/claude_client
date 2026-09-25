#!/usr/bin/env python3
"""Disable draft (oc) function definitions that the engine source already provides: ocdrop.py file name..."""
import re, sys
path, names = sys.argv[1], sys.argv[2:]
L = open(path, errors='replace').read().split('\n')
for n in names:
    k = next(i for i, l in enumerate(L) if re.match(r'^(?!extern)[A-Za-z_].*\b%s\s*\(' % n, l) and not l.rstrip().endswith(';'))
    e = next(i for i in range(k, len(L)) if L[i].rstrip() == '}')
    L[k:e + 1] = ['#if 0  // engine source'] + L[k:e + 1] + ['#endif']
open(path, 'w').write('\n'.join(L))
