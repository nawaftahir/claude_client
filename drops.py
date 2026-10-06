#!/usr/bin/env python3
"""drops.py: for each names_fix 'drop' row, fdiff our fn against every candidate addr in the other name DBs."""
import subprocess, re, os, glob
D = os.path.dirname(os.path.abspath(__file__))
T = D + '/target'
R = D + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
drops = [l.split('\t')[1] for l in open(D + '/ghidra/names_fix.tsv') if '\tdrop' in l]
cand = {}
for f in glob.glob(D + '/ghidra/names_*.tsv'):
    if f.endswith('names_fix.tsv'):
        continue
    for l in open(f):
        p = l.rstrip('\n').split('\t')
        if len(p) >= 2 and p[1] in drops and p[0] not in ('0', ''):
            cand.setdefault(p[1], set()).add(p[0])
mp = open(T + '/build/CoD2MP_s.map').read()
for n in drops:
    m = re.search(r'\s(\?%s@@\S*)' % re.escape(n), mp)
    if not m:
        continue
    for a in sorted(cand.get(n, ())):
        out = subprocess.run(['python3', D + '/fdiff.py', 'build/CoD2MP_s.exe', 'build/CoD2MP_s.map', R, '?%s@@' % n, '0x' + a, '6000'],
                             cwd=T, capture_output=True, text=True).stdout.splitlines()
        if not out:
            continue
        nd = sum(1 for l in out if l[:1] in '+-' and not l.startswith(('+++', '---')))
        print(nd, a, n, out[0].split(' ', 1)[1] if ' ' in out[0] else '', flush=True)
