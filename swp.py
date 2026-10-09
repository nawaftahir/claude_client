#!/usr/bin/env python3
"""Adjacent-statement swap sweep: one swap per near-miss fn per build, keep if diff drops.
Only swaps statements with no shared written identifier and at most one call.
usage: swp.py <all_scored.txt> [max_diff=24] [max_rounds=40]. Log: target/build/sweep.log"""
import os, re, sys
import sweep
from sweep import T, locate, getline, setline, build, score_many, objmap, log

ANY = bool(os.environ.get('SWP_ANY'))  # unsafe swaps kept only on a full match
GAP = bool(os.environ.get('SWP_GAP'))  # swap across blank lines
DMIN, DMAX = int(os.environ.get('SWP_DMIN', 1)), int(os.environ.get('SWP_DMAX', 1))
ID = re.compile(r'[A-Za-z_]\w*')
CTRL = re.compile(r'^\s*(return|break|continue|if|else|for|while|do|switch|case|default|goto|#|//|\{|\})')

def simple(l):
    s = l.strip()
    return s.endswith(';') and not CTRL.match(l) and s.count(';') == 1

def writes(l):
    s = l.strip()
    m = re.match(r'^(.*?)(?<![=!<>])(?:[-+*/&|^]|<<|>>)?=(?!=)', s)
    w = set(ID.findall(m.group(1))) if m else set()
    w |= set(re.findall(r'(\w+)\s*(?:\+\+|--)', s)) | set(re.findall(r'(?:\+\+|--)\s*(\w+)', s))
    return w

def indep(a, b):
    if '(' in a and '(' in b and re.search(r'\w\s*\(', a) and re.search(r'\w\s*\(', b):
        return False
    ia, ib = set(ID.findall(a)), set(ID.findall(b))
    return not (writes(a) & ib) and not (writes(b) & ia)

def main():
    rows = [l.split() for l in open(sys.argv[1])]
    maxd = int(sys.argv[2]) if len(sys.argv) > 2 else 24
    rounds = int(sys.argv[3]) if len(sys.argv) > 3 else 40
    om = objmap()
    fns, seen = [], set()
    for r in rows:
        if len(r) < 4 or not (int(os.environ.get('SWP_MIND', 1)) <= int(r[0]) <= maxd): continue
        loc = locate(r[3], om.get(r[3], ''))
        if not loc or loc in seen: continue
        seen.add(loc)
        p, a, b = loc
        todo = []
        L = open(p).read().split('\n')
        for n in range(a, b):
            if GAP:
                # swap two statements separated only by blank lines
                m = n + 1
                while m <= b and not L[m - 1].strip(): m += 1
                if m > n + 1 and m <= b and simple(L[n - 1]) and simple(L[m - 1]):
                    sg = indep(L[n - 1], L[m - 1])
                    if sg != ANY: todo.append((n, m - n, 2, sg))
                continue
            pv = L[n - 2].strip()
            if not (pv[-1:] in ';{}' or pv.startswith('//') or not pv): continue
            for d in range(DMIN, DMAX + 1):
                if n + d > b: break
                blk = L[n - 1:n + d]
                if not all(simple(l) for l in blk): break
                sa = all(indep(blk[0], l) for l in blk[1:])
                if sa != ANY: todo.append((n, d, 0, sa))
                sb = all(indep(blk[-1], l) for l in blk[:-1])
                if d > 1 and sb != ANY: todo.append((n, d, 1, sb))
        if todo: fns.append({'m': r[3], 'a': r[2], 'p': p, 'todo': todo})
    log('swp: %d functions, %d swaps' % (len(fns), sum(len(f['todo']) for f in fns)))
    build()
    for f, s in zip(fns, score_many([(f['m'], f['a']) for f in fns])): f['best'] = s
    def swap(f, op, undo=False):
        n, d, up = op[:3]
        L = open(f['p']).read().split('\n'); blk = L[n - 1:n + d]
        if up == 2: blk = blk[-1:] + blk[1:-1] + blk[:1]
        else: blk = blk[-1:] + blk[:-1] if up != undo else blk[1:] + blk[:1]
        L[n - 1:n + d] = blk; open(f['p'], 'w').write('\n'.join(L))
    for rnd in range(rounds):
        # one swap per file per round so two fns in a file never touch overlapping lines
        live, files = [], set()
        for f in fns:
            if f['todo'] and f['best'] > 0 and f['p'] not in files:
                files.add(f['p']); live.append(f)
        if not live: break
        for f in live:
            f['cur'] = f['todo'].pop(0); swap(f, f['cur'])
        ok, bad = build()
        if not ok:
            for f in live:
                if not bad or os.path.basename(f['p']).lower() in bad:
                    swap(f, f.pop('cur'), True)
            live = [f for f in live if 'cur' in f]
            if not live or not build()[0]:
                for f in live: swap(f, f.pop('cur'), True)
                build(); continue
        won = 0
        for f, s in zip(live, score_many([(f['m'], f['a']) for f in live])):
            op = f.pop('cur'); n, d = op[0], op[1]
            if s < f['best'] and (op[3] or s == 0):
                f['best'] = s; won += 1
                f['todo'] = [t for t in f['todo'] if t[0] + t[1] < n or t[0] > n + d]
                log('  + %s %s:%d %s %d' % (f['m'], os.path.relpath(f['p'], T), n, op, s))
            else:
                swap(f, op, True)
        log('swp round %d: %d tried, %d improved, %d matched' % (rnd, len(live), won, sum(1 for f in fns if f['best'] == 0)))
    build()
    log('swp done: matched %s' % [f['m'] for f in fns if f['best'] == 0])

if __name__ == '__main__':
    main()
