#!/usr/bin/env python3
"""Locate every unmatched function of a build in the client and summarize how it differs."""
import collections, difflib, json, os, re, sys
import capstone
import vote

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

def norm(code, start, limit):
    out = []
    for ins in md.disasm(code, start):
        if ins.address - start >= limit:
            break
        op = ins.op_str
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.size >= 5):
            op = 'X'
        elif ins.mnemonic.startswith('j'):
            op = 'x'
        op = re.sub(r'0x[0-9a-f]{5,}', 'A', op)
        out.append('%s %s' % (ins.mnemonic, op))
    return out

if __name__ == '__main__':
    img, mapf, target, cmpf, out = sys.argv[1:6]
    matched = {x['name'] for x in json.load(open(cmpf)) if x['hits']}
    pe, cd, cbase = vote.text(target)
    import bisect
    F = sorted(int(x['addr'], 16) for x in json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../codex/client/salvage_gemini/data/names_1.0.json')))['functions'])
    idx = vote.Index(cd)
    res = []
    for name, obj, va, code, mask in vote.functions(img, mapf):
        if name in matched or len(code) < 32 or ':' in obj:
            continue
        off, n = idx.vote(code, mask)
        if off is None or n < 3:
            res.append({'name': name, 'obj': obj, 'size': len(code), 'addr': None})
            continue
        c = cbase + off
        k = bisect.bisect_left(F, c - 64)
        near = [f for f in F[k:k + 8] if abs(f - c) <= 64]
        if near:
            off = min(near, key=lambda f: abs(f - c)) - cbase
        a = norm(code, va, len(code))
        b = norm(cd[off:off + len(code) + 256], cbase + off, len(code) + 256)[:len(a) + 40]
        sm = difflib.SequenceMatcher(None, a, b, autojunk=False)
        first = next(((t, a[i1:i2][:2], b[j1:j2][:2]) for t, i1, i2, j1, j2 in sm.get_opcodes() if t != 'equal'), None)
        res.append({'name': name, 'obj': obj, 'size': len(code), 'addr': hex(cbase + off), 'votes': n,
                    'ratio': round(sm.ratio(), 3), 'first': first})
    json.dump(res, open(out, 'w'), indent=0)
    loc = [x for x in res if x['addr']]
    print('unmatched', len(res), 'located', len(loc), 'bytes located', sum(x['size'] for x in loc))
    for lo, hi in ((0.98, 1.01), (0.95, 0.98), (0.9, 0.95), (0.8, 0.9), (0, 0.8)):
        s = [x for x in loc if lo <= x['ratio'] < hi]
        print('  ratio %.2f-%.2f: %4d fns %7d B' % (lo, hi, len(s), sum(x['size'] for x in s)))
