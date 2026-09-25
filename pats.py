#!/usr/bin/env python3
"""Most common (ours -> client) instruction replacements over near-miss functions.
usage (from target/): pats.py build/cls.json [min-ratio]"""
import collections, difflib, json, re, sys
import capstone, vote
from classify import norm
cls = {x['name']: x for x in json.load(open(sys.argv[1])) if x.get('addr') and x['ratio'] >= float(sys.argv[2] if len(sys.argv) > 2 else 0.7)}
pe, cd, cbase = vote.text('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
pat = collections.Counter(); ex = {}
for name, obj, va, code, mask in vote.functions('build/CoD2MP_s.exe', 'build/CoD2MP_s.map'):
    x = cls.get(name)
    if not x:
        continue
    off = int(x['addr'], 16) - cbase
    a = norm(code, va, len(code)); b = norm(cd[off:off + len(code) + 256], cbase + off, len(code) + 256)[:len(a) + 40]
    for t, i1, i2, j1, j2 in difflib.SequenceMatcher(None, a, b, autojunk=False).get_opcodes():
        if t == 'equal' or i2 - i1 > 3 or j2 - j1 > 3:
            continue
        k = (' ; '.join(re.sub(r'(e[a-d]x|e[sd]i|ebp|[a-d]l)', 'R', s) for s in a[i1:i2]), ' ; '.join(re.sub(r'(e[a-d]x|e[sd]i|ebp|[a-d]l)', 'R', s) for s in b[j1:j2]))
        pat[k] += 1; ex.setdefault(k, name.split('@')[0])
for k, n in pat.most_common(int(sys.argv[3]) if len(sys.argv) > 3 else 40):
    print('%4d  %-50s -> %-50s %s' % (n, k[0][:50], k[1][:50], ex[k][:30]))
