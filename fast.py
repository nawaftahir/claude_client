#!/usr/bin/env python3
"""Fast per-TU check: compile one TU WITHOUT /GL (make's exact flags, ~4 s) and diff the named fns against retail.
Local-shape iteration only: LTCG register conventions and cross-TU inlining are absent, so compare BEFORE vs AFTER
an edit, then confirm with a real build (mym.sh). usage (target/): fast.py <src/rel/path.cpp> <mangled> <addr> [...]
FAST_V=1 prints the diffs."""
import difflib, os, re, struct, subprocess, sys
D = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, D)
os.chdir(os.path.join(D, 'target'))
import capstone, pefile
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
_pe = pefile.PE(os.path.join(D, '../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'), fast_load=True)
_t = [s for s in _pe.sections if s.Name.startswith(b'.text')][0]
_cbase, _cd = _pe.OPTIONAL_HEADER.ImageBase + _t.VirtualAddress, _t.get_data()

def client(ca, n): return _cd[ca - _cbase:ca - _cbase + n]

def norm(code, start, lim):  # same as alldiff_lib.norm
    out = []
    for ins in md.disasm(code, start):
        op = ins.op_str
        if ins.mnemonic == 'call' or (ins.mnemonic.startswith('j') and ins.size == 5): op = 'X'
        op = re.sub(r'0x[0-9a-f]{5,}', 'A', op)
        out.append('%s %s' % (ins.mnemonic, op))
        if ins.address - start >= lim - 1: break
    return out

def cmd_for(tu):
    import json
    rel = tu[4:] if tu.startswith('src/') else tu
    want = '.' + ''.join('\\\\' + p for p in rel.split('/'))  # make -n prints .\\oc\\x.cpp
    cache = 'build/fastcmd.json'
    db = json.load(open(cache)) if os.path.exists(cache) else {}
    if rel not in db:
        out = subprocess.run(['make', '-n', '-B', 'IN_IMAGE=1', 'build/CoD2MP_s.exe'], capture_output=True, text=True).stdout
        for l in out.split('\n'):
            c = l.split(' > ')[0]
            if l.startswith('cd src && ') and '/Fo' in c and c.rstrip().endswith(want):
                c = c.replace(' /GL', '').replace(' /showIncludes', '')
                db[rel] = re.sub(r'/Fo\S+', lambda m: r'/Fo..\\build\\fast.obj', c)
        json.dump(db, open(cache, 'w'))
    if rel not in db: sys.exit('no compile command for ' + tu)
    return db[rel]

def coff_funcs(path):
    b = open(path, 'rb').read()
    _, nsec, _, symptr, nsym, opt, _ = struct.unpack_from('<HHIIIHH', b, 0)
    secs = [struct.unpack_from('<8sIIIIIIHHI', b, 20 + opt + 40 * i) for i in range(nsec)]
    strtab = symptr + 18 * nsym
    syms, i = [], 0
    while i < nsym:
        o = symptr + 18 * i
        name, val, sec, typ, cls, naux = struct.unpack_from('<8sIhHBB', b, o)
        if name[:4] == b'\0\0\0\0':
            off = struct.unpack_from('<I', name, 4)[0]
            name = b[strtab + off:b.index(b'\0', strtab + off)]
        else:
            name = name.rstrip(b'\0')
        syms.append((name.decode(errors='replace'), val, sec, typ, cls))
        i += 1 + naux
    out = {}
    for si, s in enumerate(secs):
        if not s[0].startswith(b'.text'): continue
        raw = bytearray(b[s[4]:s[4] + s[3]])
        for r in range(s[7]):  # relocs: give every fixup a dummy absolute so norm() masks it
            va, _, typ = struct.unpack_from('<IIH', b, s[5] + 10 * r)
            if typ == 6: raw[va:va + 4] = struct.pack('<I', 0x01000000)
        fs = sorted((v, n) for n, v, sec, t, c in syms if sec == si + 1 and t == 0x20)
        for k, (v, n) in enumerate(fs):
            end = fs[k + 1][0] if k + 1 < len(fs) else len(raw)
            out[n] = bytes(raw[v:end]).rstrip(b'\xcc')
    return out

def main():
    tu, pairs = sys.argv[1], sys.argv[2:]
    c = cmd_for(tu)
    r = subprocess.run(['docker', 'run', '--rm', '-v', os.getcwd() + ':/w', '-w', '/w', 'cod2-msvc71-vs2003', 'sh', '-c', c],
                       capture_output=True, text=True)
    if not os.path.exists('build/fast.obj') or r.returncode:
        print((r.stdout + r.stderr)[-1500:]); sys.exit(1)
    fs = coff_funcs('build/fast.obj')
    for m, a in zip(pairs[::2], pairs[1::2]):
        hit = [n for n in fs if n.startswith(m)]
        if not hit: print('-- %s not emitted (inlined/static?)' % m); continue
        o = fs[hit[0]]; a = int(a, 16)
        x, y = norm(o, 0x1000, len(o)), norm(client(a, len(o) + 64), a, len(o))
        d = [l for l in difflib.unified_diff(x, y, lineterm='', n=int(os.environ.get('FDN', 0)))]
        n = sum(1 for l in d if l[:1] in '+-' and l[:3] not in ('+++', '---'))
        print('%5d %s' % (n, hit[0][:70]))
        if os.environ.get('FAST_V'): print('\n'.join(d[2:]))
    os.remove('build/fast.obj')

if __name__ == '__main__':
    main()
