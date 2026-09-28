#!/usr/bin/env python3
"""Name retail callees from UNMATCHED named pairs: align the ordered direct-call lists of ours vs retail,
anchor on callees whose names agree, pair equal-length gaps between anchors elementwise.
usage: callalign.py [minvotes] (run in target/) -> 'addr<TAB>name<TAB>callalign<TAB># votes' for unnamed retail targets"""
import collections, difflib, json, struct, sys
from alldiff_lib import names, plain, d, base, cd, cbase, syms, cstarts
minv = int(sys.argv[1]) if len(sys.argv) > 1 else 2
va2name = dict(syms)
nxt = {a: b for (a, _), (b, _) in zip(syms, syms[1:])}
rstarts = sorted(set(cstarts()) | set(names.values()))
rnext = {a: b for a, b in zip(rstarts, rstarts[1:])}
rname = {}
for n, a in names.items(): rname.setdefault(a, n)
cmp = json.load(open('build/cmp.json'))

def calls(buf, b0, a, end, ok):
    out = []
    for k in range(a - b0, end - b0 - 4):
        if buf[k] != 0xe8: continue
        t = struct.unpack_from('<i', buf, k + 1)[0] + b0 + k + 5
        if ok(t): out.append(t)
    return out

votes = collections.defaultdict(collections.Counter)
for oa, on in syms:
    p = plain(on)
    ra = names.get(p)
    if ra is None or oa not in nxt or ra not in rnext: continue
    oc = [plain(va2name[t]) for t in calls(d, base, oa, nxt[oa], lambda t: t in va2name)]
    rc = calls(cd, cbase, ra, rnext[ra], lambda t: t in rnext)
    rk = [rname.get(t, '?%x' % t) for t in rc]
    sm = difflib.SequenceMatcher(None, oc, rk, autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != 'replace' or i2 - i1 != j2 - j1: continue
        for i, j in zip(range(i1, i2), range(j1, j2)):
            if rk[j].startswith('?') and oc[i] not in names: votes[oc[i]][rc[j]] += 1
taken = set(names.values())
for n, c in sorted(votes.items()):
    rt, v = c.most_common(1)[0]
    if v < minv or rt in taken: continue
    print('%x\t%s\tcallalign\t# votes=%d/%d' % (rt, n, v, sum(c.values())))
