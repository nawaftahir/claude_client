#!/usr/bin/env python3
"""If/else arm swap sweep: `if (c) A else B` -> `if (!(c)) B else A`, one per near-miss fn per build.
Allman style only; line count is unchanged. usage: ifsw.py <scored.txt> [max_diff=80] [max_rounds=60]"""
import os, re, sys
from sweep import T, locate, build, score_many, objmap, log

IF = re.compile(r'^(\s*)((?:else\s+)?if\s*\()(.*)\)\s*$')
KR = re.compile(r'^(\s*)((?:\} else )?if \()(.*)\) \{\s*$')
NEG = {'<': '>=', '>': '<=', '<=': '>', '>=': '<', '==': '!=', '!=': '=='}

def arm(L, i, ind):
    """-> end index (exclusive) of the arm starting at L[i], or None."""
    if L[i] == ind + '{':
        for j in range(i + 1, len(L)):
            if L[j] == ind + '}': return j + 1
            if L[j].strip() and not L[j].startswith(ind + '\t') and not L[j].startswith(ind + ' '): return None
        return None
    s = L[i]
    if s.startswith(ind) and len(s) > len(ind) and s[len(ind)] in ' \t' and s.rstrip().endswith(';') \
            and not re.match(r'\s*(if|for|while|else|do)\b', s):
        return i + 1
    return None

def neg(c):
    c = c.strip()
    if '&&' in c or '||' in c or '?' in c: return '!( %s )' % c
    m = re.fullmatch(r'(.+?)\s*(<=|>=|==|!=|<|>)\s*(.+)', c)
    if m and not re.search(r'<=|>=|==|!=|<|>', m.group(3)) and '(' not in m.group(1) + m.group(3):
        return '%s %s %s' % (m.group(1), NEG[m.group(2)], m.group(3))
    if re.fullmatch(r'!\s*[\w.\->\[\]]+', c): return c[1:].strip()
    if re.fullmatch(r'[\w.\->\[\]]+', c): return '!' + c
    return '!( %s )' % c

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        m = None if os.environ.get('IFSW_KR_ONLY') else IF.match(L[i])
        if not m or m.group(3).count('(') != m.group(3).count(')'): continue
        ind = m.group(1)
        e1 = arm(L, i + 1, ind)
        if not e1 or e1 >= len(L) or L[e1] != ind + 'else': continue
        e2 = arm(L, e1 + 1, ind)
        if not e2 or e2 > b: continue
        A, B = L[i + 1:e1], L[e1 + 1:e2]
        new = [ind + m.group(2) + ' ' + neg(m.group(3)) + ' )'] + B + [ind + 'else'] + A
        out.append((i, e2, new))
    for i in range(a - 1, b):
        m = KR.match(L[i])
        if not m or m.group(3).count('(') != m.group(3).count(')'): continue
        ind = m.group(1)
        j = next((x for x in range(i + 1, b) if L[x].startswith(ind + '}')), None)
        if j is None or L[j] != ind + '} else {': continue
        k = next((x for x in range(j + 1, b) if L[x].startswith(ind + '}')), None)
        if k is None or L[k] != ind + '}': continue
        new = [ind + m.group(2) + neg(m.group(3)) + ') {'] + L[j + 1:k] + [ind + '} else {'] + L[i + 1:j]
        out.append((i, k, new))
    for i in range(a - 1, b):
        # mixed style: `if (c)` / `{` ... `} else {` ... `}`
        m = IF.match(L[i])
        if not m or m.group(3).count('(') != m.group(3).count(')') or i + 1 >= b or L[i + 1] != m.group(1) + '{': continue
        ind = m.group(1)
        j = next((x for x in range(i + 2, b) if L[x].startswith(ind + '}')), None)
        if j is None or L[j] != ind + '} else {': continue
        k = next((x for x in range(j + 1, b) if L[x].startswith(ind + '}')), None)
        if k is None or L[k] != ind + '}': continue
        new = [ind + m.group(2) + ' ' + neg(m.group(3)) + ' )', ind + '{'] + L[j + 1:k] + [ind + '} else {'] + L[i + 2:j]
        out.append((i, k, new))
    return out

def main():
    rows = [l.split() for l in open(sys.argv[1])]
    maxd = int(sys.argv[2]) if len(sys.argv) > 2 else 80
    rounds = int(sys.argv[3]) if len(sys.argv) > 3 else 60
    om = objmap()
    fns, seen = [], set()
    for r in rows:
        if len(r) < 4 or not (0 < int(r[0]) <= maxd): continue
        loc = locate(r[3], om.get(r[3], ''))
        if not loc or loc in seen: continue
        seen.add(loc)
        p, a, b = loc
        L = open(p).read().split('\n')
        n = len(cands(L, a, b))
        if n: fns.append({'m': r[3], 'a': r[2], 'p': p, 'r': (a, b), 'k': 0, 'n': n})
    log('ifsw: %d functions, %d swaps' % (len(fns), sum(f['n'] for f in fns)))
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
            i, e, new = cands(L, *f['r'])[f['k']]
            f['old'] = L[:]
            L[i:e] = new; open(f['p'], 'w').write('\n'.join(L)); f['k'] += 1
        ok, bad = build()
        if not ok:
            for f in live:
                if not bad or os.path.basename(f['p']).lower() in bad:
                    open(f['p'], 'w').write('\n'.join(f.pop('old')))
            live = [f for f in live if 'old' in f]
            if not live or not build()[0]:
                for f in live: open(f['p'], 'w').write('\n'.join(f.pop('old')))
                build(); continue
        won = 0
        for f, s in zip(live, score_many([(f['m'], f['a']) for f in live])):
            old = f.pop('old')
            if s < f['best']:
                f['best'] = s; won += 1
                log('  + %s %s cand %d -> %d' % (f['m'], os.path.relpath(f['p'], T), f['k'] - 1, s))
            else:
                open(f['p'], 'w').write('\n'.join(old))
        log('ifsw round %d: %d tried, %d improved, %d matched' % (rnd, len(live), won, sum(1 for f in fns if f['best'] == 0)))
    build()
    log('ifsw done: matched %s' % [f['m'] for f in fns if f['best'] == 0])

if __name__ == '__main__':
    main()
