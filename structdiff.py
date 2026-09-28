#!/usr/bin/env python3
"""Split unmatched fns into register/schedule ties vs structural diffs (mnemonic multiset differs). run in target/"""
import collections, difflib
exec(open('../alldiff.py').read().split('res = []')[0])
res = []
for x in json.load(open('build/cmp.json')):
    if x.get('hits') or x['name'] not in nxt: continue
    ca = names.get(plain(x['name']))
    if ca is None: continue
    va, end = nxt[x['name']]
    ours = d[va-base:end-base].rstrip(b'\xcc')
    a = norm(ours, va, len(ours)); b = norm(cd[ca-cbase:ca-cbase+len(ours)+64], ca, len(ours))
    ma = collections.Counter(l.split()[0] for l in a); mb = collections.Counter(l.split()[0] for l in b)
    ops = collections.Counter(a); opb = collections.Counter(b)
    kind = 'struct' if ma != mb else ('regalloc' if ops != opb else 'sched')
    n = sum(1 for l in difflib.unified_diff(a, b, n=0, lineterm='') if l[:1] in '+-' and l[:3] not in ('+++', '---'))
    dm = (ma - mb) + (mb - ma)
    res.append((kind, n, len(ours), hex(ca), x['name'], ' '.join('%s%d' % (k, v) for k, v in dm.most_common(4))))
for r in sorted(res): print(*r)
