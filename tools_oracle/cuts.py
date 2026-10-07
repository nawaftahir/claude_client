#!/usr/bin/env python3
"""cuts.py ord.txt [name-substr]: cut edges (callee referenced before its own compile). With a name: its callers/callees with order."""
import sys
F={}; cur=None; refs={}
order=[]
for l in open(sys.argv[1]):
    k,_,n=l.rstrip('\n').partition(' ')
    if k=='F':
        cur=n; F.setdefault(n,len(order)); order.append(n); refs.setdefault(n,[])
    elif k=='X' and cur:
        refs[cur].append(n)
if len(sys.argv)>2:
    q=sys.argv[2]
    for n in F:
        if q in n:
            print('==',F[n],n)
            for c in refs[n]:
                print('  callee', F.get(c,'-'), 'CUT' if F.get(c,1e9)>F[n] and c in F else '', c)
            for m in F:
                if n in refs[m]:
                    print('  caller', F[m], 'CUT' if F[n]>F[m] else '', m)
else:
    for n in order:
        for c in refs[n]:
            if c in F and F[c]>F[n]: print(F[n],n,'->',F[c],c)
