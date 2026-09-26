#!/usr/bin/env python3
"""Name unnamed client FUN_ functions via KisakCOD / our source: shared string literals + named callees."""
import json, os, re, sys, collections
d = os.path.dirname(os.path.abspath(__file__))
STR = re.compile(r'"((?:[^"\\]|\\.){3,})"')
CALL = re.compile(r'\b([A-Za-z_]\w{3,})\s*\(')
KW = {'if', 'while', 'for', 'switch', 'return', 'sizeof', 'assert', 'iassert', 'vassert', 'alwaysfails'}

def bodies(root, exts=('.cpp', '.c')):
    """name -> (file, strings, callees) for every column-0 function definition."""
    out = {}
    for dp, _, fs in os.walk(root):
        for f in fs:
            if not f.endswith(exts): continue
            p = os.path.join(dp, f)
            src = open(p, errors='replace').read()
            for m in re.finditer(r'^[A-Za-z_][^\n;{}()]*?\b([A-Za-z_]\w*)\s*\([^;{}]*?\)\s*\n\{(.*?)^\}', src, re.S | re.M):
                n, b = m.group(1), m.group(2)
                if n in KW: continue
                out.setdefault(n, (os.path.relpath(p, root), set(STR.findall(b)), set(CALL.findall(b)) - KW))
    return out

def client():
    out = {}
    for f in os.listdir(d + '/ghidra/out'):
        m = re.match(r'([0-9a-f]{8})_(FUN_\w+)\.c$', f)
        if not m: continue
        b = open(d + '/ghidra/out/' + f, errors='replace').read()
        calls = {c for c in CALL.findall(b) if not re.match(r'(FUN|DAT|LAB|thunk|caseD|switchD)_', c)} - KW
        out[int(m.group(1), 16)] = (set(STR.findall(b)), calls)
    return out

if __name__ == '__main__':
    root = sys.argv[1]
    ref = bodies(root)
    hit = set()
    for e in json.load(open(d + '/target/build/cmp.json')): hit.update(int(h, 16) for h in e['hits'])
    starts = sorted(int(f[:8], 16) for f in os.listdir(d + '/ghidra/out') if re.match(r'[0-9a-f]{8}_', f))
    size = {a: b - a for a, b in zip(starts, starts[1:])}
    sidx = collections.defaultdict(set)
    for n, (_, s, _) in ref.items():
        for x in s: sidx[x].add(n)
    for a, (s, c) in sorted(client().items()):
        if a in hit or not s: continue
        cand = collections.Counter()
        for x in s:
            if len(sidx[x]) <= 5:
                for n in sidx[x]: cand[n] += 1
        best = []
        for n, k in cand.items():
            f, rs, rc = ref[n]
            sc = (len(s & rs) + 0.5 * len(c & rc)) / max(len(s | rs) + 0.5 * len(c | rc), 1)
            best.append((sc, n, f))
        if best:
            best.sort(reverse=True)
            sc, n, f = best[0]
            amb = len(best) > 1 and best[1][0] >= sc - 0.05
            print('%.2f %6d %x %s %s%s' % (sc, size.get(a, 0), a, n, f, ' AMB' if amb else ''))
