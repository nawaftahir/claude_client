#!/usr/bin/env python3
"""Float/double literal kinds by constant multiset, not by aligned instructions (catches reordered code).
usage (from target/): litsall.py build/CoD2MP_s.exe build/CoD2MP_s.map <client> build/cls.json src"""
import collections, glob, json, os, re, sys
from lits import Img, CONST, edit

def consts(img, va, n):
    c = collections.defaultdict(set)
    for i in img.insns(va, n):
        m = CONST.match('%s %s' % (i.mnemonic, i.op_str))
        if m:
            try:
                c[img.value(int(m.group(3), 16), m.group(2))].add(m.group(2))
            except Exception:
                pass
    return c

img, mapf, target, cls, src = sys.argv[1:6]
ours, client = Img(img), Img(target)
syms = {m.group(1): int(m.group(2), 16) for m in
        (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open(mapf, errors='replace')) if m}
files = {os.path.splitext(os.path.basename(p))[0].lower(): p for p in glob.glob(src + '/**/*.c*', recursive=True)}
total = fns = 0
for x in json.load(open(cls)):
    if not x.get('addr') or x['name'] not in syms or x['ratio'] < 0.5:
        continue
    a = consts(ours, syms[x['name']], x['size'])
    b = consts(client, int(x['addr'], 16), x['size'])
    want = {}
    for v, k in a.items():
        if k == {'qword'} and b.get(v) == {'dword'}:
            want[('lit', v)] = 'f'
        elif k == {'dword'} and b.get(v) == {'qword'}:
            want[('lit', v)] = 'd'
    path = files.get(os.path.splitext(x['obj'])[0].lower())
    m = re.match(r'\?(\w+)@', x['name'])
    if not want or not path or not m:
        continue
    n = edit(path, m.group(1), want)
    if n:
        total += n
        fns += 1
        print('%-50s %2d  %s' % (x['name'][:50], n, want))
print(total, 'edits in', fns, 'functions')
