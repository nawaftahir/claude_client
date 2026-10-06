#!/usr/bin/env python3
# usage (in target/): calldiff.py all_scored.txt -- per unmatched fn, named direct-call multiset delta ours vs retail
import collections, re, sys, capstone
sys.path.insert(0, '..')
from alldiff_lib import syms, nxt, d, base, cd, cbase, names
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def plain(s): return (s[1:].split('@@')[0].split('@')[0] if s.startswith('?') else s).lstrip('_')
on = {a: plain(s) for a, s in syms}; cn = {a: plain(n.split('::')[-1]) for n, a in names.items()}
starts = sorted(cn)
import bisect
def calls(code, start, nm):
    c = collections.Counter()
    for i in md.disasm(code, start):
        t = re.match(r'0x([0-9a-f]+)$', i.op_str)
        if i.mnemonic in ('call', 'jmp') and t and i.size == 5:
            n = nm.get(int(t.group(1), 16))
            if n: c[n] += 1
    return c
for row in open(sys.argv[1]):
    r = row.split()
    if len(r) < 4 or r[0] == '0': continue
    ca, m = int(r[2], 16), r[3]
    if m not in nxt: continue
    va, end = nxt[m]; code = d[va - base:end - base].rstrip(b'\xcc')
    k = bisect.bisect_right(starts, ca); cend = starts[k] if k < len(starts) else ca + len(code)
    cend = min(cend, ca + 2 * len(code) + 64)
    a = calls(code, va, on); b = calls(cd[ca - cbase:cend - cbase].rstrip(b'\xcc'), ca, cn)
    known = set(cn.values())
    da = {n: a[n] - b[n] for n in set(a) | set(b) if a[n] != b[n] and n in known and n in set(on.values())}
    if da: print(r[1], r[0], r[2], plain(m), ' '.join('%s%+d' % (n, -v) for n, v in sorted(da.items())))
