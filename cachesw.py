#!/usr/bin/env python3
"""Cached-local sweep: for `v = EXPR;` (EXPR = pure member/index/global read, v assigned once) substitute EXPR at
every later use of v in the fn and drop the assignment; one per fn per build, keep if the fn's diff drops.
Only when nothing EXPR reads is written between the assignment and the last use. Line count is unchanged.
usage: cachesw.py <scored.txt> [max_diff=200] [rounds=40]   CACHESW_LIST=1 prints candidates."""
import os, re, sys
from sweep import T, locate, build, score_many, objmap, log

ID = re.compile(r'[A-Za-z_]\w*')
ASSIGN = re.compile(r'^(\s*)([A-Za-z_]\w*)\s*=\s*([^;=?"\']+?)\s*;\s*$')
WIDE = bool(os.environ.get('CACHESW_WIDE'))
KW = {'return', 'if', 'else', 'for', 'while', 'do', 'switch', 'case', 'sizeof'}

def writes(l):
    m = re.match(r'^\s*(.*?)(?<![=!<>])(?:[-+*/&|^]|<<|>>)?=(?!=)', l)
    w = set(ID.findall(m.group(1))) if m else set()
    w |= set(re.findall(r'(\w+)\s*(?:\+\+|--)', l)) | set(re.findall(r'(?:\+\+|--)\s*(\w+)', l))
    return w

def cands(L, a, b):
    out = []
    body = range(a - 1, b)
    for i in body:
        m = ASSIGN.match(L[i])
        if not m: continue
        v, e = m.group(2), m.group(3)
        if v in KW or '(' in e or v in ID.findall(e): continue
        if re.search(r'\w\s*\(', e) or '++' in e or '--' in e or not ID.findall(e): continue
        if not WIDE and not re.fullmatch(r'[A-Za-z_][\w.\->\[\]\s+*/-]*', e): continue
        if not ('->' in e or '.' in e or '[' in e or WIDE): continue
        pat = re.compile(r'\b' + re.escape(v) + r'\b')
        # v written exactly once, not address-taken, used later
        decl = re.compile(r'^\s*[\w\s\*]+\b' + re.escape(v) + r'\s*;')
        uses = [j for j in body if j != i and pat.search(L[j]) and not decl.match(L[j])]
        if not uses or any(j < i for j in uses): continue
        if any(v in writes(L[j]) or re.search(r'&\s*' + re.escape(v) + r'\b', L[j]) for j in uses): continue
        deps = set(ID.findall(e))
        last = max(uses)
        if any(deps & writes(L[j]) for j in range(i + 1, last + 1)): continue
        new = {i: m.group(1) + ';'}
        for j in uses: new[j] = pat.sub('(' + e + ')' if re.search(r'[\->\[.]', e) and not re.fullmatch(r'[\w.\->\[\]]+', e) else e, L[j])
        out.append((v, e, new))
    return out

def main(gen=None, tag='cachesw'):
    gen = gen or cands
    rows = [l.split() for l in open(sys.argv[1])]
    maxd = int(sys.argv[2]) if len(sys.argv) > 2 else 200
    rounds = int(sys.argv[3]) if len(sys.argv) > 3 else 40
    om = objmap()
    fns, seen = [], set()
    for r in rows:
        if len(r) < 4 or not (0 < int(r[0]) <= maxd): continue
        loc = locate(r[3], om.get(r[3], ''))
        if not loc or loc in seen: continue
        seen.add(loc)
        L = open(loc[0]).read().split('\n')
        c = gen(L, loc[1], loc[2])
        if c: fns.append({'m': r[3], 'a': r[2], 'p': loc[0], 'r': (loc[1], loc[2]), 'k': 0, 'n': len(c), 'sz': r[1]})
    log(tag + ': %d functions, %d changes' % (len(fns), sum(f['n'] for f in fns)))
    if os.environ.get('CACHESW_LIST'):
        for f in fns:
            L = open(f['p']).read().split('\n')
            for v, e, _ in gen(L, *f['r']): print(f['sz'], f['m'][:50], v, '=', e)
        return
    build()
    for f, s in zip(fns, score_many([(f['m'], f['a']) for f in fns])): f['best'] = s
    for rnd in range(rounds):
        live, files = [], set()
        for f in fns:
            if f['k'] < f['n'] and f['best'] > 0 and f['p'] not in files:
                files.add(f['p']); live.append(f)
        if not live: break
        for f in live:
            L = open(f['p']).read().split('\n')
            c = gen(L, *f['r'])
            if f['k'] >= len(c): f['k'] = f['n']; continue
            v, e, new = c[f['k']]; f['k'] += 1
            f['old'] = L[:]; f['why'] = '%s=%s' % (v, e)
            for j, t in new.items(): L[j] = t
            open(f['p'], 'w').write('\n'.join(L))
        live = [f for f in live if 'old' in f]
        ok, bad = build()
        for _ in range(4):  # drop failing variants until the rest builds
            if ok or not live: break
            bad = {b.replace('\\', '/').split('/')[-1] for b in bad}  # cl reports dir\\file.cpp
            drop = [f for f in live if os.path.basename(f['p']).lower() in bad] or live
            log(tag + ' round %d: build failed, dropping %d of %d' % (rnd, len(drop), len(live)))
            for f in drop: open(f['p'], 'w').write('\n'.join(f.pop('old')))
            live = [f for f in live if 'old' in f]
            ok, bad = build() if live else (True, set())
        if not ok:
            for f in live: open(f['p'], 'w').write('\n'.join(f.pop('old')))
            build(); continue
        if not live: continue
        won = 0
        for f, s in zip(live, score_many([(f['m'], f['a']) for f in live])):
            old = f.pop('old')
            if s < f['best']:
                f['best'] = s; won += 1; f['k'] -= 1; f['n'] -= 1
                log('  + %s %s %s -> %d' % (f['m'], os.path.relpath(f['p'], T), f['why'], s))
            else:
                open(f['p'], 'w').write('\n'.join(old))
        log(tag + ' round %d: %d tried, %d improved, %d matched' % (rnd, len(live), won, sum(1 for f in fns if f['best'] == 0)))
    build()
    log(tag + ' done: matched %s' % [f['m'] for f in fns if f['best'] == 0])

if __name__ == '__main__':
    main()
