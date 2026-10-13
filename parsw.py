#!/usr/bin/env python3
"""Paren sweep: wrap one sub-expression in parentheses (MSVC 7.1 keeps a parenthesised FP expression as a unit;
VectorRotate matched only with `SinCos((v[i] * M_PI / 180.0), ...)`). Candidates per line: each call argument,
assignment/return right side, and the left prefix of a chain. Same line, line count unchanged.
usage: parsw.py <scored.txt> [max_diff] [rounds]   PARSW_MAX=n caps candidates per fn (default 30), PARSW_SKIP=n skips the first n"""
import os, re
import cachesw
from splitsw import top_ops

MAX = int(os.environ.get('PARSW_MAX', 30))
SKIPN = int(os.environ.get('PARSW_SKIP', 0))  # start at the n-th candidate (later windows of big fns)
SKIP = re.compile(r'^\s*(#|//|/\*|\*|case\b|for\b)')

def args(s, o):
    """-> [(start, end)] of the top-level args of the call whose '(' is at o."""
    out, d, st = [], 0, o + 1
    for i in range(o, len(s)):
        ch = s[i]
        if ch in '([': d += 1
        elif ch in ')]':
            d -= 1
            if d == 0: out.append((st, i)); return out
        elif ch == ',' and d == 1: out.append((st, i)); st = i + 1
    return []

def spans(line):
    out = []
    for m in re.finditer(r'\b[A-Za-z_]\w*\s*\(', line):  # call arguments
        if m.group(0).split('(')[0].strip() in ('if', 'while', 'switch', 'return', 'sizeof'): continue
        out += args(line, m.end() - 1)
    m = re.match(r'^(\s*(?:return\s+|[\w.\->\[\]\*]+\s*[-+*/]?=\s*))(.+?);\s*$', line)  # rhs / return value
    if m and not m.group(2).startswith('{'): out.append((m.start(2), m.end(2)))
    res = []
    for a, b in out:
        e = line[a:b]
        lead = len(e) - len(e.lstrip()); e = e.strip(); a += lead; b = a + len(e)
        if not e or '"' in e or "'" in e or '?' in e or '=' in e.replace('==', '').replace('<=', '').replace('>=', '').replace('!=', ''): continue
        ops = top_ops(e)
        if not ops: continue  # none at depth 0, or already wrapped
        res.append((a, b))
        if len(ops) >= 2:  # operands of the last lowest-precedence op
            p = ([o for o in ops if o[1] in '+-'] or ops)[-1][0]
            pre, post = e[:p].rstrip(), e[p + 1:]
            if top_ops(pre): res.append((a, a + len(pre)))
            if top_ops(post.strip()): res.append((a + p + 1 + len(post) - len(post.lstrip()), b))
    return res

CMP = re.compile(r'(==|!=|<=|>=|<|>|&&|\|\|)')

def deep(line):
    """PARSW_DEEP: operands inside every ( ) group, split at comparisons and && ||."""
    res, stack = [], []
    for i, ch in enumerate(line):
        if ch == '(': stack.append(i)
        elif ch == ')' and stack:
            o = stack.pop()
            seg, base = line[o + 1:i], o + 1
            parts, st, d = [], 0, 0
            for k, c in enumerate(seg):
                if c in '([': d += 1
                elif c in ')]': d -= 1
                elif d == 0 and c == ',': parts.append((st, k)); st = k + 1
            parts.append((st, len(seg)))
            for x, y in parts:
                piece = seg[x:y]
                st = 0
                for m in list(CMP.finditer(piece)) + [None]:
                    en = m.start() if m else len(piece)
                    if m and (piece[m.start():m.start() + 2] == '->' or piece[m.start() - 1:m.start() + 1] == '->'): continue
                    e = piece[st:en]
                    lead = len(e) - len(e.lstrip()); e = e.strip()
                    a0 = base + x + st + lead
                    if e and '"' not in e and top_ops(e) and not re.search(r'[-+*/]\s*$', e) and (a0, a0 + len(e)) != (base, i):
                        res.append((a0, a0 + len(e)))
                    if m: st = m.end()
    old = set(spans(line))
    return [r for r in res if r not in old]

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        s = L[i]
        if SKIP.match(s): continue
        for x, y in (deep(s) if os.environ.get('PARSW_DEEP') else spans(s)):
            new = s[:x] + '(' + s[x:y] + ')' + s[y:]
            out.append(('L%d' % (i + 1), s[x:y], {i: new}))
            if len(out) >= SKIPN + MAX: return out[SKIPN:]
    return out[SKIPN:]

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == '--test':
        t = '\t\t\tSinCos(vRotation[i] * M_PI / 180.0, &s, &c);'
        r = [t[x:y] for x, y in spans(t)]
        assert r == ['vRotation[i] * M_PI / 180.0', 'vRotation[i] * M_PI'], r
        t = '\treturn (float)(ps->bobCycle & 255) / 255.0f * (M_PI * 2) + M_PI * 2;'
        r = [t[x:y] for x, y in spans(t)]
        assert '(float)(ps->bobCycle & 255) / 255.0f * (M_PI * 2)' in r, r
        t = '\tif ( a * b + c > d - e * f )'
        r = [t[x:y] for x, y in deep(t)]
        assert r == ['a * b + c', 'd - e * f'], r
        print('ok')
    else:
        cachesw.main(cands, 'parsw')
