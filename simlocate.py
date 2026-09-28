#!/usr/bin/env python3
"""Find retail addresses for unmatched fns that have none in the names DB, by code similarity
against unmatched retail functions. usage: simlocate.py [minratio] (run in target/, needs build/cmp.json)
Prints: ratio size ourname retail_addr runnerup_ratio. Treat as HYPOTHESIS until fdiff confirms."""
import bisect, difflib, json, re, sys
exec(open('../alldiff_lib.py').read().split('def norm')[0])
from alldiff_lib import norm
minr = float(sys.argv[1]) if len(sys.argv) > 1 else 0.75

cmp = json.load(open('build/cmp.json'))
hit = {int(h, 16) for x in cmp for h in x['hits']}
starts = set(names.values()) | hit
for i in range(len(cd) - 5):  # direct call targets
    if cd[i] == 0xe8:
        t = int.from_bytes(cd[i+1:i+5], 'little', signed=True) + cbase + i + 5
        if cbase <= t < cbase + len(cd): starts.add(t)
starts = sorted(starts)
claimed = hit | {names[plain(x['name'])] for x in cmp if plain(x['name']) in names}

def seq(code, va):
    return [l.split(' ', 1)[0] + (' ' + l.split(' ', 1)[1] if 'A' not in l else '') for l in norm(code, va, len(code))]

cands = []
for k, a in enumerate(starts[:-1]):
    if a in claimed: continue
    code = cd[a-cbase:starts[k+1]-cbase].rstrip(b'\xcc')
    if len(code) >= 16: cands.append((len(code), a, seq(code, a)))
cands.sort()
sizes = [c[0] for c in cands]

for x in cmp:
    if x['hits'] or plain(x['name']) in names or x['name'] not in nxt or x['name'].startswith(('$L', '__ehh')): continue
    va, end = nxt[x['name']]
    code = d[va-base:end-base].rstrip(b'\xcc')
    if len(code) < 16: continue
    s = seq(code, va)
    lo, hi = bisect.bisect_left(sizes, len(code) * 0.8), bisect.bisect_right(sizes, len(code) * 1.25)
    best = []
    for n, a, t in cands[lo:hi]:
        m = difflib.SequenceMatcher(None, s, t, autojunk=False)
        if m.real_quick_ratio() < minr or m.quick_ratio() < minr: continue
        best.append((m.ratio(), a))
    best.sort(reverse=True)
    if best and best[0][0] >= minr:
        print('%.3f %d %s %s %.3f' % (best[0][0], len(code), x['name'], hex(best[0][1]), best[1][0] if len(best) > 1 else 0), flush=True)
