#!/usr/bin/env python3
"""Pairwise LTCG child-order probe: main calls P_i, Q_i; P_i <-> Q_i is a 2-cycle. The cdecl member is
the one main's DFS visits first. Prints per pair: decl order, call order, COFF order, entry."""
import os, random, re, subprocess, struct, sys
import capstone
random.seed(int(sys.argv[1]) if len(sys.argv) > 1 else 1)
d = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'pairs'); os.makedirs(d, exist_ok=True)
words = ['Alpha','Bravo','Cg','Dvar','Echo','Fx','Gfx','Hunk','Item','Key','Menu','Net','Sys','Ui','Va','Win','Zm','Scr','Com','Cl']
pairs = []
for i in range(16):
    a, b = random.sample(words, 2)
    pairs.append(('%s_%d' % (a, i), '%s_%d' % (b, i)))
body = '''int {n}(int x)
{{
    int i, s = 0;
    if (x <= 0) return g[1];
    for (i = 0; i < x; i++) {{ s += g[i & 63] * {k}; g[(i * 7) & 63] ^= s; }}
    g[x & 63] += {c}(x - 1);
    if (s & 1) g[3] = s; else g[4] = s >> 2;
    return s + g[x & 31];
}}
'''
decl = []
for p, q in pairs:
    decl += [p, q] if random.random() < .5 else [q, p]
random.shuffle(decl)
callo = [x for pq in pairs for x in pq]
random.shuffle(callo)
hdr = 'extern int g[64];\n' + ''.join('int %s(int x);\n' % f for f in decl)
a = hdr + ''.join(body.format(n=p, k=i + 3, c=q) + body.format(n=q, k=i + 5, c=p) for i, (p, q) in enumerate(pairs))
m = hdr + 'int g[64];\nint main(int argc, char **argv)\n{\n' + ''.join('    g[%d] = %s(argc);\n' % (j & 63, f) for j, f in enumerate(callo)) + '    return g[0];\n}\n'
open(d + '/a.cpp', 'w').write(a); open(d + '/m.cpp', 'w').write(m)
cmd = ('cd /w && wine /opt/msvc710/Vc7/bin/cl.exe /nologo /c /O2 /GL a.cpp m.cpp >/dev/null && '
       'wine /opt/msvc710/Vc7/bin/link.exe /nologo /LTCG /OPT:NOREF /MAP:m.map /OUT:t.exe a.obj m.obj libcmt.lib >/dev/null && '
       'mkdir -p n && cd n && wine /opt/msvc710/Vc7/bin/cl.exe /nologo /c /O2 /Ob0 ../m.cpp >/dev/null && wine /opt/msvc710/Vc7/bin/dumpbin.exe /nologo /symbols m.obj')
r = subprocess.run(['docker', 'run', '--rm', '-v', d + ':/w', 'cod2-msvc71-build', 'sh', '-c', cmd], capture_output=True, text=True)
coff = [re.search(r'\?(\w+)@@', l).group(1) for l in r.stdout.split('\n') if 'UNDEF' in l and '?' in l and 'YAHH' in l]
pe = open(d + '/t.exe', 'rb').read()
syms = {}
for l in open(d + '/m.map', errors='replace'):
    mm = re.match(r'\s*0001:[0-9a-f]+\s+\?(\w+)@@\S*\s+([0-9a-f]{8})', l)
    if mm: syms[mm.group(1)] = int(mm.group(2), 16)
po = struct.unpack_from('<I', pe, 0x3c)[0]; opt = struct.unpack_from('<H', pe, po + 20)[0]
nm, vs, va, rs, ro = struct.unpack_from('<8sIIII', pe, po + 24 + opt)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def cdecl(f):
    a = syms[f]; ins = list(md.disasm(pe[a - 0x400000 - va + ro:][:40], a))[:6]
    return any(i.mnemonic == 'mov' and 'esp +' in i.op_str.split(',')[-1] for i in ins)
for p, q in pairs:
    if p not in syms or q not in syms:
        print(p, q, 'inlined?'); continue
    e = [x for x in (p, q) if cdecl(x)]
    first = lambda L: p if L.index(p) < L.index(q) else q
    print('%-10s %-10s entry=%-10s decl=%-10s call=%-10s coff=%-10s alpha=%s' % (p, q, ','.join(e), first(decl), first(callo), first(coff) if p in coff and q in coff else '?', min(p, q)))
