#!/usr/bin/env python3
"""Count displacement pairs (ours -> client) in near-miss functions: struct layout differences show up as repeated shifts.
usage (from target/): layout.py build/cls.json"""
import collections, difflib, json, re, sys
import capstone, vote
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def ins(code, va):
    return [(i.mnemonic, i.op_str) for i in md.disasm(code, va)]
cls = {x['name']: x for x in json.load(open(sys.argv[1])) if x.get('addr') and x['ratio'] >= 0.6}
pe, cd, cbase = vote.text('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
pairs = collections.Counter(); where = collections.defaultdict(set)
num = re.compile(r'([+-] 0x[0-9a-f]+|[+-] \d+)\]')
for name, obj, va, code, mask in vote.functions('build/CoD2MP_s.exe', 'build/CoD2MP_s.map'):
    x = cls.get(name)
    if not x:
        continue
    off = int(x['addr'], 16) - cbase
    a = ins(code, va); b = ins(cd[off:off + len(code) + 64], cbase + off)[:len(a) + 10]
    sm = difflib.SequenceMatcher(None, [m for m, _ in a], [m for m, _ in b], autojunk=False)
    for t, i1, i2, j1, j2 in sm.get_opcodes():
        if t != 'equal':
            continue
        for (m1, o1), (m2, o2) in zip(a[i1:i2], b[j1:j2]):
            if o1 == o2 or 'esp' in o1 or 'ebp' in o1:
                continue
            d1, d2 = num.findall(o1), num.findall(o2)
            if len(d1) == 1 and len(d2) == 1 and num.sub('', o1) == num.sub('', o2):
                k = (d1[0].replace(' ', ''), d2[0].replace(' ', ''))
                pairs[k] += 1; where[k].add(name.split('@')[0])
for k, n in pairs.most_common(int(sys.argv[2]) if len(sys.argv) > 2 else 40):
    print(n, k, len(where[k]), ' '.join(sorted(where[k])[:6]))
