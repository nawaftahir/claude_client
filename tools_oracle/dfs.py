#!/usr/bin/env python3
"""Parse oracle D/E/C lines (c2 call-graph DFS): roots, cut edges, a fn's path.
usage: dfs.py ord.txt [roots|cuts|path NAME|kids NAME]"""
import sys
lines = [l.rstrip('\n') for l in open(sys.argv[1], errors='replace') if l[:2] in ('D ', 'E ', 'C ')]
mode = sys.argv[2] if len(sys.argv) > 2 else 'cuts'
stack, roots, cuts, parent, kids, pre = [], [], [], {}, {}, {}
for l in lines:
    t, n = l[0], l[2:]
    if t == 'D':
        if stack: parent[n] = stack[-1]; kids.setdefault(stack[-1], []).append(n)
        else: roots.append(n)
        pre[n] = len(pre); stack.append(n)
    elif t == 'E': stack.pop()
    else: cuts.append((stack[-1], n))
def path(n):
    p = [n]
    while p[-1] in parent: p.append(parent[p[-1]])
    return ' < '.join(p)
if mode == 'roots':
    for r in roots: print(r)
elif mode == 'cuts':
    for a, b in cuts: print(a, '->', b)
elif mode == 'path':
    print(path(sys.argv[3]))
elif mode == 'kids':
    print('\n'.join(kids.get(sys.argv[3], [])))
