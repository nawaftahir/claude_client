#!/usr/bin/env python3
"""Unmatched fns whose return-value idiom differs from retail: int (mov eax,1 / xor eax,eax) vs byte
(mov al,1 / xor al,al) vs bool-with-zeroing (xor eax,eax; setcc al). run in target/"""
import collections, difflib, json, re
exec(open('../alldiff_lib.py').read().split('def norm')[0])
from alldiff_lib import norm
def idioms(seq):
    c = collections.Counter()
    for i, l in enumerate(seq):
        if l in ('mov eax, 1', 'xor eax, eax') and i + 1 < len(seq) and seq[i+1].startswith(('ret', 'pop')): c['int'] += 1
        if l in ('mov al, 1', 'xor al, al'): c['byte'] += 1
        if l.startswith('set') and i >= 2 and 'xor eax, eax' in seq[i-3:i]: c['boolz'] += 1
        if l.startswith('set') and not (i >= 2 and 'xor eax, eax' in seq[i-3:i]): c['set'] += 1
    return c
for x in json.load(open('build/cmp.json')):
    if x['hits'] or x['name'] not in nxt: continue
    ca = names.get(plain(x['name']))
    if ca is None: continue
    va, end = nxt[x['name']]
    ours = d[va-base:end-base].rstrip(b'\xcc')
    a = norm(ours, va, len(ours)); b = norm(cd[ca-cbase:ca-cbase+len(ours)+64], ca, len(ours))
    ia, ib = idioms(a), idioms(b)
    if ia != ib and (ia['byte'] != ib['byte'] or ia['boolz'] != ib['boolz']):
        print(len(ours), x['name'], 'ours', dict(ia), 'retail', dict(ib))
