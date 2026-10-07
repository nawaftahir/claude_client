#!/usr/bin/env python3
"""DFS stack at the moment edge A->B is cut (sim replay). usage (target/): cutpath.py ord map rsp A-substr B-substr"""
import sys; sys.path.insert(0, __file__.rsplit('/', 1)[0])
from sim import graph, fnobj, order
g = graph(sys.argv[1]); nodes = order(sys.argv[3], fnobj(sys.argv[2]), None)
A, B = sys.argv[4], sys.argv[5]; st, stack = {}, []
def visit(n):
    st[n] = 2; stack.append(n)
    for c in g.get(n, []):
        s = st.get(c, 0)
        if s == 2 and A in n and B in c:
            print(' > '.join(x.split('@@')[0] for x in stack)); sys.exit()
        elif s == 0 and c in g: visit(c)
    st[n] = 1; stack.pop()
sys.setrecursionlimit(100000)
for n in nodes:
    if n in g and not st.get(n): visit(n)
