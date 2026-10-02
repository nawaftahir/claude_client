#!/usr/bin/env python3
"""Replay MSVC 7.1 LTCG's bottom-up compile order on a non-/GL /Ob0 /Gy build (lab-pinned model):
roots = objs in REVERSE link order, fns in REVERSE definition order; children in the caller TU's
first-declaration order (from the /E .i next to each obj); post-order compile. A call to a fn still on the DFS stack is a cycle cut: that
callee keeps cdecl and can't be inlined there.
usage (from target/): ltcgsim.py <nogl obj dir> [tus.txt] [fn ...]  -> per fn: cut-callers, DFS path"""
import os, re, struct, sys
sys.setrecursionlimit(1000000)
objdir = sys.argv[1]
tus = sys.argv[2] if len(sys.argv) > 2 and sys.argv[2].endswith('.txt') else 'src/tus.txt'
want = [a for a in sys.argv[2:] if not a.endswith('.txt')]

def coff(path):
    b = open(path, 'rb').read()
    mach, nsec, _, symoff, nsym, opt, _ = struct.unpack_from('<HHIIIHH', b, 0)
    secs = []
    for i in range(nsec):
        nm, vs, va, rs, rp, rlp, lp, nr, nl, ch = struct.unpack_from('<8sIIIIIIHHI', b, 20 + opt + 40 * i)
        secs.append((nm.rstrip(b'\0').decode(errors='replace'), rp, rs, rlp, nr, ch))
    strtab = symoff + 18 * nsym
    syms, i = [], 0
    while i < nsym:
        raw = b[symoff + 18 * i: symoff + 18 * i + 18]
        if raw[:4] == b'\0\0\0\0':
            o = struct.unpack_from('<I', raw, 4)[0]
            name = b[strtab + o: b.index(b'\0', strtab + o)].decode(errors='replace')
        else:
            name = raw[:8].rstrip(b'\0').decode(errors='replace')
        val, sec, typ, cls, naux = struct.unpack_from('<IhHBB', raw, 8)
        syms.append((name, val, sec, typ, cls))
        syms.extend([None] * naux)
        i += 1 + naux
    rel = {}
    for k, (nm, rp, rs, rlp, nr, ch) in enumerate(secs):
        if ch & 0x20 and nr:  # code
            rel[k + 1] = [struct.unpack_from('<IIH', b, rlp + 10 * j) for j in range(nr)]
    return secs, syms, rel

def bare(k):
    k = k.split(':', 1)[-1]
    m = re.match(r'\?([A-Za-z_]\w*)@', k)
    if m: return m.group(1)
    return re.sub(r'^_', '', re.sub(r'@\d+$', '', k))
order = [l.split('#')[0].split()[0] for l in open(tus) if l.split('#')[0].split()]
objs = [os.path.join(objdir, re.sub(r'\.[^.]*$', '.obj', f)) for f in order]
defs, fns_of, symorder, calls = {}, {}, {}, {}
for oi, p in enumerate(objs):
    secs, syms, rel = coff(p)
    tag = os.path.basename(p)
    dpos = {}
    ip = p[:-4] + '.i'
    if os.path.exists(ip):  # children are visited in the caller TU's first-declaration order (lab-pinned)
        for m in re.finditer(r'([A-Za-z_]\w*)\s*\(', open(ip, errors='replace').read()):
            dpos.setdefault(m.group(1), m.start())
    for ov in os.environ.get('OVR', '').split():  # what-if: tu.obj:Name<Other = declare Name just before Other
        t, rule = ov.split(':', 1); a, b = rule.split('<')
        if t == tag and b in dpos: dpos[a] = dpos[b] - 1
    tag = os.path.basename(p)
    seckey = {}
    fl = []
    so = {}
    for idx, s in enumerate(syms):
        if not s: continue
        name, val, sec, typ, cls = s
        key = name if cls == 2 else tag + ':' + name
        if typ & 0x20 and sec > 0 and cls in (2, 3):  # function defined here
            if key not in defs:
                defs[key] = oi
                fl.append((sec, key))
            seckey.setdefault(sec, key)
        if typ & 0x20 and cls in (2, 3) and key not in so:
            so[key] = len(so)
    fl.sort()  # section number = definition order
    fns_of[oi] = [k for _, k in fl]
    for sec, key in fl:
        out = []
        for va, si, ty in rel.get(sec, ()):
            if ty != 0x14: continue  # REL32 = call/jmp
            s = syms[si]
            if not s or not (s[3] & 0x20): continue
            ck = s[0] if s[4] == 2 else tag + ':' + s[0]
            if ck != key and ck not in out: out.append(ck)
        out.sort(key=lambda k: (dpos.get(bare(k), 1 << 40), so.get(k, 1 << 30)))
        calls[key] = out
idx, pre = {}, {}
on, done, cuts, path = set(), set(), {}, {}
stack = []
def dfs(v):
    on.add(v); stack.append(v); pre[v] = len(pre)
    path.setdefault(v, list(stack))
    for w in calls.get(v, ()):
        if w not in defs: continue
        if w in on:
            cuts.setdefault(w, []).append(v)
        elif w not in done:
            dfs(w)
    on.discard(v); stack.pop(); done.add(v)
    idx[v] = len(idx)
for oi in reversed(range(len(objs))):
    for f in reversed(fns_of.get(oi, [])):
        if f not in done: dfs(f)
print('fns', len(defs), 'compiled', len(idx), 'cut targets', len(cuts))
import json
json.dump({'cuts': cuts, 'order': idx}, open('build/ltcgsim.json', 'w'))
for w in want:
    ks = [k for k in defs if w in k]
    for k in ks:
        print(k, 'pre', pre.get(k), 'post', idx.get(k), 'cut-from:', [c[:40] for c in cuts.get(k, [])][:8])
        print('   calls:', ' '.join(re.sub(r'^(\w+\.obj:)?\?([^@]+)@.*', r'\2', x)[:28] for x in calls.get(k, [])))
        print('   path:', ' > '.join(re.sub(r'^\?([^@]+)@.*', r'\1', x)[:28] for x in path.get(k, [])))
