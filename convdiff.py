#!/usr/bin/env python3
"""Rank callees whose call-site argument passing differs from retail (LTCG convention drift).
usage: convdiff.py <scored.txt> <retail.dis> <ours.dis>  (run in target/; dis = objdump -d -M intel --no-show-raw-insn)"""
import sys, re, bisect, collections, os
scored, rdis, odis = sys.argv[1:4]
def load(p):
    L = []
    for l in open(p, errors='replace'):
        t = l.rstrip('\n').split('\t')
        if len(t) >= 2 and t[0].strip().endswith(':'):
            try: L.append((int(t[0].strip()[:-1], 16), t[-1].strip()))
            except ValueError: pass
    return L
R, O = load(rdis), load(odis)
Ra, Oa = [a for a, _ in R], [a for a, _ in O]
fns = []
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l)
    if m: fns.append((int(m.group(2), 16), m.group(1)))
fns.sort(); ostart = {n: a for a, n in fns}; oname = {a: n for a, n in fns}; ostarts = [a for a, _ in fns]
d = os.path.dirname(os.path.abspath(__file__))
rst = sorted(int(m.group(1), 16) for f in os.listdir(d + '/ghidra/out') for m in [re.match(r'([0-9a-f]{8})_', f)] if m)
def body(L, A, s, e): return L[bisect.bisect_left(A, s):bisect.bisect_left(A, e)]
def calls(ins):
    out, pushes, regs = [], 0, set()
    for _, i in ins:
        op = i.split()[0] if i else ''
        if op == 'push': pushes += 1
        elif op == 'mov' and re.match(r'mov\s+(eax|ecx|edx|ebx|esi|edi),', i): regs.add(i.split()[1].rstrip(','))
        elif op == 'call':
            m = re.match(r'call\s+0x([0-9a-f]+)$', i)
            out.append((int(m.group(1), 16) if m else None, pushes)); pushes = 0; regs = set()
        elif op in ('add', 'sub') and 'esp' in i: pass
    return out
bad = collections.defaultdict(list); good = collections.Counter()
for l in open(scored):
    p = l.split()
    if len(p) < 4 or p[3] not in ostart: continue
    ra = int(p[2], 16); oa = ostart[p[3]]
    re_ = rst[bisect.bisect_right(rst, ra)] if bisect.bisect_right(rst, ra) < len(rst) else ra + 4096
    oe = ostarts[bisect.bisect_right(ostarts, oa)] if bisect.bisect_right(ostarts, oa) < len(ostarts) else oa + 4096
    rc, oc = calls(body(R, Ra, ra, re_)), calls(body(O, Oa, oa, oe))
    if len(rc) != len(oc) or not oc: continue
    for (rt, rp), (ot, op) in zip(rc, oc):
        if ot is None or ot not in oname: continue
        callee = oname[ot]
        if rp != op: bad[callee].append((p[3].split('@@')[0], op, rp))
        else: good[callee] += 1
rows = sorted(bad.items(), key=lambda kv: -len(kv[1]))
for c, v in rows[:60]:
    print(f'{len(v):3d} bad {good[c]:3d} ok  {c}  e.g. {v[0][0]} ours {v[0][1]} pushes, retail {v[0][2]}')
