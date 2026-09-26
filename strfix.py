#!/usr/bin/env python3
"""For each literal in our exe the client lacks, show the closest client literal (text-fix candidates)."""
import difflib, re, os, collections
D = os.path.dirname(os.path.abspath(__file__))
def strs(p):
    return set(m.group().decode('latin1') for m in re.finditer(rb'[\x20-\x7e\t\n]{8,}', open(p, 'rb').read()))
ours = strs(D + '/target/build/CoD2MP_s.exe')
cl = strs(D + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
idx = collections.defaultdict(list)
for s in cl:
    idx[s[:6]].append(s)
for s in sorted(ours - cl):
    if not re.search(r'[a-z]{3,}', s) or s.startswith(('?', '.?')): continue
    cands = idx.get(s[:6]) or cl
    if len(cands) > 3000: continue
    m = difflib.get_close_matches(s, cands, 1, 0.75)
    if m: print('%r\n  -> %r' % (s, m[0]))
