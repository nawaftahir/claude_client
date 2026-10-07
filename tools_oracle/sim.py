#!/usr/bin/env python3
"""Offline replay of c2.dll's LTCG call-graph DFS (0x1070ce78) from an oracle log (D/T lines).
graph(ord) -> {node: [callees in edge order]}; order(rsp, map) -> node list (reverse obj, reverse addr);
dfs(graph, nodes) -> (cuts, preorder). usage: sim.py ord.txt map rsp  -> prints cut count + cuts"""
import re, sys
sys.setrecursionlimit(100000)
def graph(path):
    g, cur = {}, None
    for l in open(path, errors='replace'):
        if l[:2] == 'D ': cur = l[2:].rstrip('\n'); g[cur] = []
        elif l[:2] == 'T ': g[cur].append(l[2:].rstrip('\n'))
    return g
def fnobj(mp):
    r = {}
    for l in open(mp):
        m = re.match(r' 0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8}) f\s+(?:\S+:)?(\S+\.obj)', l)
        if m: r[m.group(1)] = (m.group(3), int(m.group(2), 16))
    return r
def order(rsp, fo, roots):
    """node list: reverse obj order, reverse addr; nodes missing from the map keep their observed root slot"""
    objs = [l.strip().split('\\')[-1] for l in open(rsp) if l.strip()]
    rank = {o: i for i, o in enumerate(objs)}
    by = {}
    for n, (o, a) in fo.items(): by.setdefault(o, []).append((a, n))
    out = []
    for o in reversed(objs):
        out += [n for a, n in sorted(by.get(o, []), reverse=True)]
    return out
def dfs(g, nodes):
    st, cuts, pre = {}, [], []
    def visit(n):
        st[n] = 2; pre.append(n)
        for c in g.get(n, []):
            s = st.get(c, 0)
            if s == 2: cuts.append((n, c))
            elif s == 0 and c in g: visit(c)
        st[n] = 1
    for n in nodes:
        if n in g and not st.get(n): visit(n)
    for n in g:  # nodes the map does not list
        if not st.get(n): visit(n)
    return cuts, pre
if __name__ == '__main__':
    g = graph(sys.argv[1]); fo = fnobj(sys.argv[2])
    cuts, pre = dfs(g, order(sys.argv[3], fo, None))
    print(len(cuts), 'cuts;', sum(1 for n in g if n not in fo), 'nodes not in map')
    for a, b in cuts: print(a, '->', b)
def order_retail(cmpjson, rsp, fo):
    """node list = retail address descending (exact retail root order); fns without a retail address
    are placed right after (in list order) the previous fn of their obj that has one"""
    import json
    ra = {f['name']: int(f['hits'][0], 16) for f in json.load(open(cmpjson)) if len(f['hits']) == 1}
    base = order(rsp, fo, None)
    key, last = {}, 0x7fffffff
    for i, n in enumerate(base):
        if n in ra: last = ra[n]; key[n] = (last, 0)
        else: key[n] = (last, i)
    return sorted(base, key=lambda n: (-key[n][0], key[n][1]))
