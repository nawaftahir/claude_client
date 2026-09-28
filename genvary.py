#!/usr/bin/env python3
"""Batched line-variant experiments for lab.py: each experiment applies one vary.auto variant in each of up to K
different near-miss functions. usage (in target/): genvary.py <structdiff.txt> <out.json> [maxdiff=40] [K=10] [gen=cmp|tern|ifelse]
Winners = variants whose own function gained in its batch; confirm them alone with lab.py before merging."""
import glob, json, os, re, sys
import cmpflip
T = os.getcwd()

om = {}
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:\S+\s+(\S+)\s+[0-9a-f]{8} f\s+(?:\S+:)?(\S+)\.obj', l)
    if m: om[m.group(1)] = m.group(2)
SRC = {}
for p in glob.glob('src/**/*.c*', recursive=True):
    SRC.setdefault(os.path.splitext(os.path.basename(p))[0].lower(), []).append(p)

def locate(mangled):
    """-> (path, first body line, last body line) 1-based, or None"""
    name = mangled[1:].split('@@')[0] if mangled.startswith('?') else mangled.lstrip('_')
    parts = name.split('@')
    q = parts[0] if len(parts) == 1 else parts[1] + '::' + parts[0]
    pat = re.compile(r'^[A-Za-z_].*\b' + re.escape(q) + r'\s*\(')
    for p in SRC.get(om.get(mangled, '').lower(), []):
        L = open(p, errors='replace').read().split('\n')
        for i, l in enumerate(L):
            if pat.match(l) and not l.rstrip().endswith(';'):
                j = i
                while j < len(L) and L[j] not in ('{',): j += 1
                k = j
                while k < len(L) and L[k] != '}': k += 1
                if k < len(L): return p, j + 2, k
    return None

def sane(o, v):
    if v.count('->') != o.count('->') or 'assert' in o or v.strip().startswith(('/*', '//')): return False
    if re.search(r'\bif\s+[^(\s]', v) or re.search(r'-\s*[;)]', v): return False
    dep = 0
    for ch in v:
        dep += ch == '('; dep -= ch == ')'
        if dep < 0: return False
    return dep == o.count('(') - o.count(')')

maxd = int(sys.argv[3]) if len(sys.argv) > 3 else 40
K = int(sys.argv[4]) if len(sys.argv) > 4 else 10
gen = sys.argv[5] if len(sys.argv) > 5 else 'cmp'
fns, seen = [], set()
for r in (l.split() for l in open(sys.argv[1])):
    if len(r) < 5 or r[0] == 'sched' or not (0 < int(r[1]) <= maxd): continue
    loc = locate(r[4])
    if not loc or loc in seen: continue
    seen.add(loc)
    p, a, b = loc
    L = open(p, errors='replace').read().split('\n')
    if gen == 'ifelse':  # exact-text block edits, unique in the file
        txt = '\n'.join(L)
        todo = [(None, o, v) for o, v in cmpflip.ifelse(L, a - 1, b) if txt.count(o) == 1]
    else:
        f = {'cmp': cmpflip.variants, 'tern': cmpflip.ternaries}[gen]
        todo = [(n, L[n - 1], v) for n in range(a, b + 1) for v in f(L[n - 1]) if sane(L[n - 1], v)]
    if todo: fns.append((r[4], p, todo))
exps, k = [], 0
while any(f[2] for f in fns):
    batch = [f for f in fns if f[2]][:K]
    edits, tags = [], []
    for m, p, todo in batch:
        n, o, v = todo.pop(0)
        edits.append(dict({'file': p, 'old': o, 'new': v, 'fn': m}, **({'line': n} if n else {})))
    exps.append({'id': 'v%04d' % k, 'edits': edits}); k += 1
    fns.sort(key=lambda f: -len(f[2]))  # spread long lists across batches
json.dump(exps, open(sys.argv[2], 'w'), indent=0)
print('%d functions, %d variants, %d experiments' % (len(seen), sum(len(e['edits']) for e in exps), len(exps)))
