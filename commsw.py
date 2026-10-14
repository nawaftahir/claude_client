#!/usr/bin/env python3
"""Commute sweep: swap operands of a commutative + or * in an assignment RHS (x87 load order / scheduling),
one per fn per build, keep if the fn's diff drops. usage: commsw.py <scored.txt> [max_diff] [rounds]"""
import re
import cachesw
from splitsw import top_ops

LINE = re.compile(r'^(\s*)([A-Za-z_][\w.\->\[\]]*\s*[-+*/]?=\s*)([^;=?"\'<>!&|]+?)\s*;(.*)$')

def swaps(e):
    """-> alternative spellings of e with one commutative pair swapped."""
    out = []
    ops = top_ops(e)
    kinds = {o for _, o in ops}
    # swap around '+' (terms may hold * /), or inside a pure product; never across - or /
    adds = [p for p, o in ops if o == '+'] if '-' not in kinds else []
    if not adds and kinds == {'*'}: adds = [p for p, o in ops]
    if adds:
        for p in adds:
            # swap the two terms around this op
            lo = max([q for q in adds if q < p], default=-1)
            hi = min([q for q in adds if q > p], default=len(e))
            a, b = e[lo + 1:p].strip(), e[p + 1:hi].strip()
            if a and b and a != b:
                out.append(e[:lo + 1] + (' ' if lo >= 0 else '') + b + ' ' + e[p] + ' ' + a + (' ' if hi < len(e) else '') + e[hi:])
    # products inside each + term
    terms, last = [], 0
    for p, o in ops:
        if o in '+-': terms.append((last, p)); last = p + 1
    terms.append((last, len(e)))
    if len(terms) > 1:
        for s, t in terms:
            sub = e[s:t]
            for alt in swaps_mul(sub):
                out.append(e[:s] + alt + e[t:])
    return [re.sub(r'\s+', ' ', x).strip() for x in out]

def swaps_mul(sub):
    ops = top_ops(sub.strip())
    if len(ops) != 1 or ops[0][1] != '*': return []
    s = sub.strip(); p = ops[0][0]
    lead = sub[:len(sub) - len(sub.lstrip())]; trail = sub[len(sub.rstrip()):]
    return [lead + s[p + 1:].strip() + ' * ' + s[:p].strip() + trail]

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        m = LINE.match(L[i])
        if not m or re.match(r'\s*(return|if|for|while|case)\b', L[i]): continue
        ind, lhs, e, rest = m.groups()
        for alt in swaps(e):
            if alt.replace(' ', '') != e.replace(' ', ''):
                out.append((lhs.strip(), alt, {i: '%s%s%s;%s' % (ind, lhs, alt, rest)}))
    return out

if __name__ == '__main__':
    assert swaps('a + b') == ['b + a'], swaps('a + b')
    assert 'c * d + a * b' in swaps('a * b + c * d')
    cachesw.main(cands, 'commsw')
