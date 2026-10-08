#!/usr/bin/env python3
"""Adjacent-statement swap sweep: one swap per near-miss fn per build, keep if diff drops.
Only swaps statements with no shared written identifier and at most one call.
usage: swp.py <all_scored.txt> [max_diff=24] [max_rounds=40]. Log: target/build/sweep.log"""
import os, re, sys
import sweep
from sweep import T, locate, getline, setline, build, score_many, objmap, log

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
        if len(r) < 4 or not (0 < int(r[0]) <= maxd): continue
        loc = locate(r[3], om.get(r[3], ''))
        if not loc or loc in seen: continue
        seen.add(loc)
        p, a, b = loc
        todo = []
        for n in range(a, b):
            x, y = getline(p, n), getline(p, n + 1)
            pv = getline(p, n - 1).strip()
            if simple(x) and simple(y) and indep(x, y) and (pv[-1:] in ';{}' or pv.startswith('//') or not pv):
                todo.append(n)
        if todo: fns.append({'m': r[3], 'a': r[2], 'p': p, 'todo': todo})
    log('swp: %d functions, %d swaps' % (len(fns), sum(len(f['todo']) for f in fns)))
    build()
    for f, s in zip(fns, score_many([(f['m'], f['a']) for f in fns])): f['best'] = s
    def swap(f, n):
        x, y = getline(f['p'], n), getline(f['p'], n + 1)
        setline(f['p'], n, y); setline(f['p'], n + 1, x)
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
                    swap(f, f.pop('cur'))
            live = [f for f in live if 'cur' in f]
            if not live or not build()[0]:
                for f in live: swap(f, f.pop('cur'))
                build(); continue
        won = 0
        for f, s in zip(live, score_many([(f['m'], f['a']) for f in live])):
            n = f.pop('cur')
            if s < f['best']:
                f['best'] = s; won += 1
                f['todo'] = [t for t in f['todo'] if abs(t - n) > 1]
                log('  + %s %s:%d %d' % (f['m'], os.path.relpath(f['p'], T), n, s))
            else:
                swap(f, n)
        log('swp round %d: %d tried, %d improved, %d matched' % (rnd, len(live), won, sum(1 for f in fns if f['best'] == 0)))
    build()
    log('swp done: matched %s' % [f['m'] for f in fns if f['best'] == 0])

if __name__ == '__main__':
    main()
