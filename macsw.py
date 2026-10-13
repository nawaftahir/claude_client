#!/usr/bin/env python3
"""Macro sweep: expand one Vector* helper call into its Q3 macro text (q_shared.h), e.g.
`VectorMA(v, s, b, o);` -> `((o)[0]=(v)[0]+(b)[0]*(s),...);`. Parenthesised FP operands change MSVC 7.1 codegen,
so retail may have used the macros. Same line, line count unchanged.
usage: macsw.py <scored.txt> [max_diff] [rounds]"""
import re
import cachesw
from parsw import args

Q3 = {
    'VectorCopy': ('a,b', '((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])'),
    'VectorAdd': ('a,b,c', '((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])'),
    'VectorSubtract': ('a,b,c', '((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])'),
    'VectorScale': ('v,s,o', '((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))'),
    'VectorMA': ('v,s,b,o', '((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))'),
    'VectorSet': ('v,x,y,z', '((v)[0]=(x),(v)[1]=(y),(v)[2]=(z))'),
    'DotProduct': ('x,y', '((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])'),
    'Vector2Copy': ('a,b', '((b)[0]=(a)[0],(b)[1]=(a)[1])'),
}
CALL = re.compile(r'\b(' + '|'.join(Q3) + r')\s*\(')

def expand(name, actual):
    params, body = Q3[name]
    params = params.split(',')
    if len(params) != len(actual): return None
    sub = dict(zip(params, actual))
    return re.sub(r'\b(' + '|'.join(params) + r')\b', lambda m: sub[m.group(1)], body)

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        s = L[i]
        if re.match(r'\s*(#|//|/\*|\*)', s): continue
        for m in CALL.finditer(s):
            sp = args(s, m.end() - 1)
            if not sp: continue
            act = [s[x:y].strip() for x, y in sp]
            if any(not t or '"' in t for t in act): continue
            e = expand(m.group(1), act)
            if not e: continue
            end = sp[-1][1] + 1
            out.append(('L%d' % (i + 1), m.group(1), {i: s[:m.start()] + e + s[end:]}))
    return out

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == '--test':
        L = ['\tVectorMA(origin, deltaHorz, vRight, origin);', '\tx = DotProduct(a, b[1]) * 2;']
        r = [list(c[2].values())[0] for c in cands(L, 1, 2)]
        assert r[0] == '\t((origin)[0]=(origin)[0]+(vRight)[0]*(deltaHorz),(origin)[1]=(origin)[1]+(vRight)[1]*(deltaHorz),(origin)[2]=(origin)[2]+(vRight)[2]*(deltaHorz));', r[0]
        assert r[1] == '\tx = ((a)[0]*(b[1])[0]+(a)[1]*(b[1])[1]+(a)[2]*(b[1])[2]) * 2;', r[1]
        print('ok')
    else:
        cachesw.main(cands, 'macsw')
