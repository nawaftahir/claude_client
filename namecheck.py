#!/usr/bin/env python3
"""Audit names-DB pairings: for each unmatched fn with a named retail address, score our normalized
instruction stream vs the retail fn there; for low scorers search unmatched retail fns for a better home.
usage: namecheck.py [thresh=0.5] (run in target/, needs build/cmp.json). Writes ../ghidra/names_fix.tsv
(confident relocations ratio>=0.75 & margin>=0.1, plus `0<TAB>name<TAB>drop` rows for wrong pairings with no home)."""
import bisect, difflib, json, os, re, sys
os.environ['NAMES_NOFIX'] = '1'  # audit the raw DB, so reruns are idempotent
from alldiff_lib import names, d, base, cd, cbase, nxt, norm, plain
thr = float(sys.argv[1]) if len(sys.argv) > 1 else 0.5
cmp = json.load(open('build/cmp.json'))
hit = {int(h, 16) for x in cmp for h in x['hits']}
starts = set(names.values()) | hit
for i in range(len(cd) - 5):
    if cd[i] == 0xe8:
        t = int.from_bytes(cd[i+1:i+5], 'little', signed=True) + cbase + i + 5
        if cbase <= t < cbase + len(cd): starts.add(t)
starts = sorted(starts)
claimed = hit | {names[plain(x['name'])] for x in cmp if plain(x['name']) in names}
def seq(code, va):
    return [l.split(' ', 1)[0] + (' ' + l.split(' ', 1)[1] if 'A' not in l else '') for l in norm(code, va, len(code))]
def rcode(a):
    k = bisect.bisect_right(starts, a)
    return cd[a-cbase:(starts[k] if k < len(starts) else a+4096)-cbase].rstrip(b'\xcc')
cands = []
for k, a in enumerate(starts[:-1]):
    if a not in claimed:
        code = rcode(a)
        if len(code) >= 16: cands.append((len(code), a, seq(code, a)))
cands.sort(); sizes = [c[0] for c in cands]
def ratio(s, t):
    m = difflib.SequenceMatcher(None, s, t, autojunk=False)
    return m.ratio()
from collections import Counter
dup = Counter(plain(n) for n in {x['name'] for x in cmp if not x['hits']})  # overloads share one DB key: skip
checked = 0; wrong = []; fix = []; drop = []
for x in cmp:
    if x['hits'] or x['name'] not in nxt or x['name'].startswith(('$L', '__ehh')): continue
    ca = names.get(plain(x['name']))
    if ca is None or dup[plain(x['name'])] > 1: continue
    va, end = nxt[x['name']]
    code = d[va-base:end-base].rstrip(b'\xcc')
    if len(code) < 16: continue
    checked += 1
    s = seq(code, va)
    cur = ratio(s, seq(rcode(ca), ca))
    if cur >= thr: continue
    lo, hi = bisect.bisect_left(sizes, len(code) * 0.8), bisect.bisect_right(sizes, len(code) * 1.25)
    best = []
    for n, a, t in cands[lo:hi]:
        m = difflib.SequenceMatcher(None, s, t, autojunk=False)
        if m.real_quick_ratio() < thr or m.quick_ratio() < thr: continue
        best.append((m.ratio(), a))
    best.sort(reverse=True)
    b2 = best[1][0] if len(best) > 1 else 0
    r1 = best[0][0] if best else 0
    wrong.append((x['name'], len(code), hex(ca), cur, r1, hex(best[0][1]) if best else '-', b2))
    if r1 >= 0.75 and r1 - b2 >= 0.1 and r1 > cur: fix.append((best[0][1], plain(x['name'])))
    elif cur < 0.3: drop.append(plain(x['name']))  # 0.3-thresh: reported only (may be a real structural near-miss)
for w in sorted(wrong, key=lambda w: -w[4]):
    print('cur=%.2f size=%d %s @%s -> best=%.2f @%s runnerup=%.2f' % (w[3], w[1], w[0][:60], w[2], w[4], w[5], w[6]))
with open('../ghidra/names_fix.tsv', 'w') as f:
    for a, n in fix: f.write('%x\t%s\tfix\n' % (a, n))
    for n in drop: f.write('0\t%s\tdrop\n' % n)
print('checked=%d wrong=%d relocated=%d dropped=%d' % (checked, len(wrong), len(fix), len(drop)), file=sys.stderr)
