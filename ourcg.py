#!/usr/bin/env python3
"""Name retail callees exactly: a matched fn's call at offset k targets the same function in both binaries.
usage: ourcg.py (run in target/) -> new pairs 'addr<TAB>name<TAB>ourcg' for unmatched callees not yet in the names DB"""
import bisect, collections, json, struct
from alldiff_lib import names, plain, d, base, cd, cbase, syms
va2name = dict(syms)
cmp = json.load(open('build/cmp.json'))
hit = {x['name'] for x in cmp if x['hits']}
votes = collections.defaultdict(collections.Counter)
for x in cmp:
    if len(x['hits']) != 1 or x['name'] not in {n for _, n in syms}: continue
    ra = int(x['hits'][0], 16)
    oa = next(a for a, n in syms if n == x['name']) if False else None
votes_src = {n: a for a, n in syms}
for x in cmp:
    if len(x['hits']) != 1 or x['name'] not in votes_src: continue
    oa, ra, n = votes_src[x['name']], int(x['hits'][0], 16), x['size']
    for k in range(n - 4):
        if d[oa - base + k] != 0xe8 or cd[ra - cbase + k] != 0xe8: continue
        ot = struct.unpack_from('<i', d, oa - base + k + 1)[0] + oa + k + 5
        rt = struct.unpack_from('<i', cd, ra - cbase + k + 1)[0] + ra + k + 5
        if ot in va2name: votes[va2name[ot]][rt] += 1
known = set(names.values())
for name, c in sorted(votes.items()):
    if name in hit: continue
    rt, v = c.most_common(1)[0]
    p = plain(name)
    if names.get(p) == rt: continue
    tag = 'CONFLICT %s' % hex(names[p]) if p in names else ('taken' if rt in known else 'new')
    print('%x\t%s\tourcg\t# %s votes=%d/%d' % (rt, p, tag, v, sum(c.values())))
