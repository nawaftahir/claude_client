#!/usr/bin/env python3
"""Open-code sweep: a Vector* helper call statement -> three plain per-component statements in braces, e.g.
`VectorScale(d, s, o);` -> `{ o[0] = d[0] * s; o[1] = d[1] * s; o[2] = d[2] * s; }` (PM_Accelerate matched this way).
Same line, line count unchanged. usage: unvsw.py <scored.txt> [max_diff] [rounds]"""
import re
import cachesw
from parsw import args
from splitsw import top_ops

FORM = {
    'VectorCopy': ('a,b', 'b[k] = a[k]'),
    'VectorAdd': ('a,b,c', 'c[k] = a[k] + b[k]'),
    'VectorSubtract': ('a,b,c', 'c[k] = a[k] - b[k]'),
    'VectorScale': ('v,s,o', 'o[k] = v[k] * s'),
    'VectorMA': ('v,s,b,o', 'o[k] = v[k] + b[k] * s'),
    'VectorNegate': ('a,b', 'b[k] = -a[k]'),
}
STMT = re.compile(r'^(\s*)(' + '|'.join(FORM) + r')\s*\(')

def wrap(t, scalar):
    # keep precedence: compound operands get parens (a scalar sum times v[k] must stay a sum)
    if top_ops(t) or (not scalar and not re.fullmatch(r'[\w.\->\[\]]+', t)): return '(' + t + ')'
    return t

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        s = L[i]
        m = STMT.match(s)
        if not m: continue
        sp = args(s, m.end() - 1)
        params, body = FORM[m.group(2)]
        params = params.split(',')
        if len(sp) != len(params): continue
        rest = s[sp[-1][1] + 1:].strip()
        if not rest.startswith(';'): continue
        act = {p: wrap(s[x:y].strip(), p == 's') for p, (x, y) in zip(params, sp)}
        parts = []
        for k in '012':
            t = re.sub(r'\b(' + '|'.join(params) + r')\b(\[k\])?',
                       lambda q: act[q.group(1)] + ('[%s]' % k if q.group(2) else ''), body)
            parts.append(t + ';')
        out.append(('L%d' % (i + 1), m.group(2), {i: '%s{ %s }%s' % (m.group(1), ' '.join(parts), rest[1:])}))
    return out

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == '--test':
        L = ['\t\tVectorScale(wishdir, wishspeed, wishVelocity);', '\tVectorMA(o, a + b, ent->r, o);']
        r = [list(c[2].values())[0] for c in cands(L, 1, 2)]
        assert r[0] == '\t\t{ wishVelocity[0] = wishdir[0] * wishspeed; wishVelocity[1] = wishdir[1] * wishspeed; wishVelocity[2] = wishdir[2] * wishspeed; }', r[0]
        assert r[1] == '\t{ o[0] = o[0] + ent->r[0] * (a + b); o[1] = o[1] + ent->r[1] * (a + b); o[2] = o[2] + ent->r[2] * (a + b); }', r[1]
        print('ok')
    else:
        cachesw.main(cands, 'unvsw')
