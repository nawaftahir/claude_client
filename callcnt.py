#!/usr/bin/env python3
"""Unmatched named fns where retail makes more calls than ours (missing callee / stubbed body).
usage: callcnt.py (run in target/) -> retail-ours calls, ours size, retail size, addr, name"""
import json, os, re
from alldiff_lib import names, nxt, ours, client, plain, md
starts = sorted(int(m.group(1), 16) for f in os.listdir('../ghidra/out') for m in [re.match(r'([0-9a-f]{8})_', f)] if m)
size = {a: b - a for a, b in zip(starts, starts[1:])}
def calls(code, va):
    n = 0
    for ins in md.disasm(code, va):
        if ins.mnemonic == 'call': n += 1
        if ins.mnemonic == 'ret' and ins.address + ins.size >= va + len(code) - 16: break
    return n
res = []
for x in json.load(open('build/cmp.json')):
    if x['hits'] or x['name'] not in nxt: continue
    ca = names.get(plain(x['name']))
    if ca is None or ca not in size: continue
    va, end = nxt[x['name']]
    o = ours(va, end); r = client(ca, size[ca]).rstrip(b'\xcc')
    d = calls(r, ca) - calls(o, va)
    if d > 0: res.append((d, len(o), len(r), ca, x['name']))
for r in sorted(res, key=lambda r: -r[2]): print(r[0], r[1], r[2], hex(r[3]), r[4])
