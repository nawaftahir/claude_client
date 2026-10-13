#!/usr/bin/env python3
"""Split sweep: `x = A op B;` -> `x = A; x = x op B;` (retail stores the intermediate), one per fn per build,
keep if the fn's diff drops. Same line, so line count is unchanged. usage: splitsw.py <scored.txt> [max_diff] [rounds]"""
import os, re, sys
import cachesw

LINE = re.compile(r'^(\s*)([A-Za-z_][\w.\->\[\]]*)\s*=\s*([^;=?"\']+?)\s*;(.*)$')
ID = re.compile(r'[A-Za-z_]\w*')

def top_ops(e):
    """-> [(pos, op)] of binary + - * / at paren depth 0."""
    out, d, prev = [], 0, ''
    for i, ch in enumerate(e):
        if ch in '([': d += 1
        elif ch in ')]': d -= 1
        elif d == 0 and ch in '+-*/' and prev not in ('', '(', '+', '-', '*', '/', ',') and not re.search(r'\d[eE]$', e[:i]) \
                and e[i - 1:i + 1] != '->' and e[i:i + 2] != '->':
            out.append((i, ch))
        if not ch.isspace(): prev = ch
    return out

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        m = LINE.match(L[i])
        if not m or re.match(r'\s*(return|if|for|while|case)\b', L[i]): continue
        ind, lhs, e, rest = m.group(1), m.group(2), m.group(3), m.group(4)
        if os.environ.get('SPLIT_CASCADE') and ';' not in rest: continue
        ops = top_ops(e)
        if not ops: continue
        low = [o for o in ops if o[1] in '+-'] or ops
        p, op = low[-1]  # left-assoc: split before the last lowest-precedence op
        A, B = e[:p].strip(), e[p + 1:].strip()
        if not A or not B: continue
        base = ID.findall(lhs)[0]
        if base in ID.findall(B) and lhs in B: continue
        if re.search(r'\b' + re.escape(base) + r'\b', B) and not re.fullmatch(r'\w+', lhs): continue
        if re.search(r'\b' + re.escape(lhs) + r'\b', B): continue
        if op in '*/' and re.search(r'[+-]', B): B = '(' + B + ')'
        # Both statements must stay inside an unbraced if/else or loop body.
        out.append(('%s' % lhs, e, {i: '%s{ %s = %s; %s = %s %s %s; }%s' % (ind, lhs, A, lhs, lhs, op, B, rest)}))
    return out

if __name__ == '__main__':
    cachesw.main(cands, 'splitsw')
