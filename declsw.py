#!/usr/bin/env python3
"""Decl-order sweep: move one local declaration of the fn's leading decl block to the front or back.
Decl order steers MSVC 7.1 register/reload order (SV_Netchan_PrintProfileStats matched by moving one int to the end).
Initializers must be side-effect free. Line count unchanged. usage: declsw.py <scored.txt> [max_diff] [rounds]"""
import re
import cachesw

DECL = re.compile(r'^\s*(?:(?:const|unsigned|signed|static|struct|volatile)\s+)*[A-Za-z_]\w*[\s\*]+\**[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*'
                  r'(?:,\s*\**[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*)*(?:=\s*[^;(]+)?;\s*(?://.*)?$')
KW = re.compile(r'^\s*(return|else|goto|case|delete|throw)\b')

def block(L, a, b):
    st = j = a - 1  # a = first body line (sweep.locate)
    while j < b and DECL.match(L[j]) and not KW.match(L[j]): j += 1
    return st, j

def cands(L, a, b):
    st, en = block(L, a, b)
    d = L[st:en]
    out = []
    for i in range(len(d)):
        for j in (0, len(d) - 1):
            if i == j or (j == 0 and i == 0): continue
            n = d[:]; n.insert(j, n.pop(i))
            out.append(('L%d' % (st + i + 1), d[i].strip() + (' ->front' if j == 0 else ' ->back'),
                        {st + k: n[k] for k in range(len(d)) if n[k] != d[k]}))
    return out

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == '--test':
        L = ['void f( int x )', '{', '\tint i;', '\tint t = 0;', '\tchar buf[32], *p;', '\tconst float *v;', '', '\tfoo();', '}']
        assert block(L, 3, 9) == (2, 6), block(L, 3, 9)
        c = cands(L, 3, 9)
        assert len(c) == 6, len(c)
        assert c[0][2] == {2: '\tint t = 0;', 3: '\tchar buf[32], *p;', 4: '\tconst float *v;', 5: '\tint i;'}, c[0][2]
        assert not DECL.match('\tx = foo(1);') and not DECL.match('\tint t = foo();') and KW.match('\treturn a;')
        print('ok')
    else:
        cachesw.main(cands, 'declsw')
