#!/usr/bin/env python3
"""Compound-assignment sweep: `x = x op e;` <-> `x op= e;` (also `x = e op x` for + and *), one per fn per build.
Same line, line count unchanged. usage: compsw.py <scored.txt> [max_diff] [rounds]"""
import re
import cachesw
from splitsw import top_ops

ASG = re.compile(r'^(\s*)([A-Za-z_][\w.\->\[\]]*)\s*=\s*([^;=?"\']+?)\s*;(.*)$')
CMP = re.compile(r'^(\s*)([A-Za-z_][\w.\->\[\]]*)\s*([-+*/])=\s*([^;"\']+?)\s*;(.*)$')

def cands(L, a, b):
    out = []
    for i in range(a - 1, b):
        s = L[i]
        if re.match(r'\s*(return|if|for|while|case|#|//)\b', s): continue
        m = CMP.match(s)
        if m:
            ind, x, op, e, rest = m.groups()
            eo = [o for _, o in top_ops(e)]
            if eo and (op in '*/' or op == '-' or any(o in '+-' for o in eo)): e = '(' + e + ')'
            out.append((x, op + '=', {i: '%s%s = %s %s %s;%s' % (ind, x, x, op, e, rest)}))
            continue
        m = ASG.match(s)
        if not m: continue
        ind, x, e, rest = m.groups()
        ops = top_ops(e)
        if not ops: continue
        p, op = ops[0]
        if e[:p].strip() == x:
            tail = [o for _, o in ops[1:]]
            if (op in '+-' and not any(o in '+-' for o in tail)) or (op in '*/' and not tail):
                out.append((x, op + '=', {i: '%s%s %s= %s;%s' % (ind, x, op, e[p + 1:].strip(), rest)}))
                continue
        p, op = ops[-1]
        if op in '+*' and e[p + 1:].strip() == x and ((op == '+' and all(o in '*/' for _, o in ops[:-1])) or len(ops) == 1):
            out.append((x, op + '= (rev)', {i: '%s%s %s= %s;%s' % (ind, x, op, e[:p].strip(), rest)}))
    return out

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == '--test':
        L = ['\tx = x + a * b;', '\ty += c - d;', '\tz *= c + d;', '\tw = a * 2 + w;', '\tv = v - a - b;']
        r = [list(c[2].values())[0] for c in cands(L, 1, 5)]
        assert r == ['\tx += a * b;', '\ty = y + (c - d);', '\tz = z * (c + d);', '\tw += a * 2;'], r
        print('ok')
    else:
        cachesw.main(cands, 'compsw')
