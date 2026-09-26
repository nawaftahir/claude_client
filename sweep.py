#!/usr/bin/env python3
"""Batch source-form search: one variant line per near-miss function per build, greedy keep.
usage: sweep.py <all_scored.txt> [max_diff=24] [max_rounds=12]
all_scored rows: '<diff> <size> <client-addr> <mangled>'. Log: target/build/sweep.log"""
import difflib, glob, json, os, re, subprocess, sys
import vary
D = os.path.dirname(os.path.abspath(__file__))
T = D + '/target'
LOG = open(T + '/build/sweep.log', 'a')

def log(*a):
    s = ' '.join(str(x) for x in a)
    print(s); LOG.write(s + '\n'); LOG.flush()

def score_many(items):
    """items: [(mangled, caddr)] -> [diff lines]; fresh process so binaries are re-read."""
    r = subprocess.run([sys.executable, __file__, '--score'], input=json.dumps(items), cwd=T, capture_output=True, text=True)
    return json.loads(r.stdout)

def _score():
    sys.path.insert(0, D)
    from alldiff_lib import nxt, norm, ours, client
    out = []
    for m, a in json.load(sys.stdin):
        if m not in nxt:
            out.append(10**6); continue
        va, end = nxt[m]
        o = ours(va, end)
        x, y = norm(o, va, len(o)), norm(client(int(a, 16), len(o) + 64), int(a, 16), len(o))
        out.append(sum(1 for l in difflib.unified_diff(x, y, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('+++', '---')))
    print(json.dumps(out))

def objmap():
    om = {}
    for l in open(T + '/build/CoD2MP_s.map', errors='replace'):
        m = re.match(r'\s*0001:\S+\s+(\S+)\s+[0-9a-f]{8} f\s+(?:\S+:)?(\S+)\.obj', l)
        if m: om[m.group(1)] = m.group(2)
    return om

SRC = {}
for p in glob.glob(T + '/src/**/*.c*', recursive=True):
    SRC.setdefault(os.path.splitext(os.path.basename(p))[0].lower(), []).append(p)

def locate(mangled, obj):
    """-> (path, first body line, last body line) 1-based, or None."""
    name = mangled[1:].split('@@')[0] if mangled.startswith('?') else mangled.lstrip('_')
    parts = name.split('@')
    q = parts[0] if len(parts) == 1 else parts[1] + '::' + parts[0]
    pat = re.compile(r'^[A-Za-z_].*\b' + re.escape(q) + r'\s*\(')
    for p in SRC.get(obj.lower(), []):
        L = open(p, errors='replace').read().split('\n')
        for i, l in enumerate(L):
            if pat.match(l) and not l.rstrip().endswith(';'):
                j = i
                while j < len(L) and L[j] != '{': j += 1
                k = j
                while k < len(L) and L[k] != '}': k += 1
                if k < len(L): return p, j + 2, k
    return None

def setline(p, n, text):
    L = open(p).read().split('\n'); L[n - 1] = text; open(p, 'w').write('\n'.join(L))

def getline(p, n):
    return open(p).read().split('\n')[n - 1]

def build():
    r = subprocess.run(['make'], cwd=T, capture_output=True, text=True)
    bad = set()
    for m in re.finditer(r'^(\S+?)\((\d+)\) : (?:fatal )?error', r.stdout + r.stderr, re.M):
        bad.add(os.path.basename(m.group(1)).lower())
    return r.returncode == 0, bad

def main():
    if sys.argv[1] == '--score':
        return _score()
    rows = [l.split() for l in open(sys.argv[1])]
    maxd = int(sys.argv[2]) if len(sys.argv) > 2 else 24
    rounds = int(sys.argv[3]) if len(sys.argv) > 3 else 12
    om = objmap()
    fns, seen = [], set()
    for r in rows:
        if len(r) < 4 or not (0 < int(r[0]) <= maxd): continue
        loc = locate(r[3], om.get(r[3], ''))
        if not loc or loc in seen: continue
        seen.add(loc)
        p, a, b = loc
        todo = [(n, v) for n in range(a, b + 1) for v in vary.auto(getline(p, n))]
        if todo: fns.append({'m': r[3], 'a': r[2], 'p': p, 'todo': todo, 'kept': []})
    log('sweep: %d functions, %d variants' % (len(fns), sum(len(f['todo']) for f in fns)))
    ok, _ = build()
    base = score_many([(f['m'], f['a']) for f in fns])
    for f, s in zip(fns, base): f['best'] = s
    for rnd in range(rounds):
        live = [f for f in fns if f['todo'] and f['best'] > 0]
        if not live: break
        for f in live:
            n, v = f['todo'].pop(0)
            f['cur'] = (n, getline(f['p'], n), v)
            setline(f['p'], n, v)
        ok, bad = build()
        if not ok:
            for f in live:
                if os.path.basename(f['p']).lower() in bad or not bad:
                    n, o, v = f.pop('cur'); setline(f['p'], n, o)
            ok, _ = build()
            live = [f for f in live if 'cur' in f]
            if not ok:
                for f in live:
                    n, o, v = f.pop('cur'); setline(f['p'], n, o)
                build(); continue
        sc = score_many([(f['m'], f['a']) for f in live])
        won = 0
        for f, s in zip(live, sc):
            n, o, v = f.pop('cur')
            if s < f['best']:
                f['best'] = s; f['kept'].append((n, v.strip())); won += 1
                f['todo'] = [t for t in f['todo'] if t[0] != n]
                log('  + %s %s:%d %d -> %s' % (f['m'], os.path.relpath(f['p'], T), n, s, v.strip()))
            else:
                setline(f['p'], n, o)
        log('round %d: %d tried, %d improved, %d matched' % (rnd, len(live), won, sum(1 for f in fns if f['best'] == 0)))
    build()
    log('done: matched %s' % [f['m'] for f in fns if f['best'] == 0])

if __name__ == '__main__':
    main()
