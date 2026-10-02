#!/usr/bin/env python3
"""LTCG SCC-entry lab. gen.py <name> <spec.json>: writes TUs, builds in cod2-msvc71-build, prints convention per fn."""
import json, os, re, subprocess, sys, struct
import capstone
name, spec = sys.argv[1], json.loads(sys.argv[2])
d = os.path.join(os.path.dirname(os.path.abspath(__file__)), name)
os.makedirs(d, exist_ok=True)
for f in os.listdir(d): os.remove(os.path.join(d, f))
body = '''int {n}(int x)
{{
    int i, s = 0;
    if (x <= 0) return g[1];
    for (i = 0; i < x; i++) {{ s += g[i & 63] * {k}; g[(i * 7) & 63] ^= s; }}
    g[x & 63] += {calls};
    if (s & 1) g[3] = s; else g[4] = s >> 2;
    return s + g[x & 31];
}}
'''
decl = 'extern int g[64];\n' + ''.join('int %s(int x);\n' % f for f in spec.get('decl', spec['fns']))
tus = spec['tus']  # {"a.cpp": ["A","B"], ...} definition order per TU; main in "m.cpp"
edges = spec['edges']  # {"A": ["B"], ...}
for tu, fns in tus.items():
    src = 'extern int g[64];\n' + ''.join('int %s(int x);\n' % f for f in spec.get('decl_' + tu, spec.get('decl', spec['fns'])))
    for k, f in enumerate(fns):
        if f == 'main':
            src += 'int g[64];\nint main(int argc, char **argv)\n{\n' + ''.join('    g[%d] = %s(argc);\n' % (j, c) for j, c in enumerate(spec['main'])) + '    return g[0];\n}\n'
        else:
            calls = ' + '.join('%s(x - %d)' % (c, j + 1) for j, c in enumerate(edges.get(f, []))) or '1'
            src += body.format(n=f, k=k + 3 + len(f), calls=calls)
    open(os.path.join(d, tu), 'w').write(src)
order = spec.get('link', list(tus))
cmd = 'cd /w && for f in %s; do wine /opt/msvc710/Vc7/bin/cl.exe /nologo /c /O2 /GL /MT $f >/dev/null || exit 1; done && wine /opt/msvc710/Vc7/bin/link.exe /nologo /LTCG /OPT:NOREF /MAP:m.map /OUT:t.exe %s libcmt.lib kernel32.lib >/dev/null' % (' '.join(tus), ' '.join(t.replace('.cpp', '.obj') for t in order))
r = subprocess.run(['docker', 'run', '--rm', '-v', d + ':/w', 'cod2-msvc71-build', 'sh', '-c', cmd], capture_output=True, text=True)
if r.returncode:
    print(r.stdout[-2000:], r.stderr[-2000:]); sys.exit(1)
pe = open(os.path.join(d, 't.exe'), 'rb').read()
syms = {}
for l in open(os.path.join(d, 'm.map'), errors='replace'):
    m = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8})', l)
    if m: syms[m.group(1)] = int(m.group(2), 16)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
base = 0x400000; text_off = 0x400; text_rva = 0x1000  # default layout for /OPT link
pe_off = struct.unpack_from('<I', pe, 0x3c)[0]
nsec = struct.unpack_from('<H', pe, pe_off + 6)[0]; opt = struct.unpack_from('<H', pe, pe_off + 20)[0]
sh = pe_off + 24 + opt
for i in range(nsec):
    nm, vs, va, rs, ro = struct.unpack_from('<8sIIII', pe, sh + 40 * i)
    if nm.startswith(b'.text'): text_rva, text_off = va, ro
inl = {}
out = []
for f in spec['fns']:
    s = [k for k in syms if re.match(r'\?%s@@' % f, k)]
    if not s:
        out.append('%s:inlined' % f); continue
    a = syms[s[0]]
    code = pe[a - base - text_rva + text_off:][:40]
    ins = list(md.disasm(code, a))[:6]
    stk = any('[esp + ' in i.op_str and i.mnemonic == 'mov' and i.op_str.split(',')[0].strip().startswith(('e', 'e')) and 'esp' in i.op_str.split(',')[1] for i in ins)
    out.append('%s:%s@%x' % (f, 'cdecl' if stk else 'REG', a))
ma = syms.get('_main')
mc = []
if ma:
    for i in md.disasm(pe[ma - base - text_rva + text_off:][:300], ma):
        if i.mnemonic == 'call': mc.append(i.op_str)
        if i.mnemonic == 'ret': break
print(name, spec['main'], ' '.join(out), 'main->', ' '.join(mc))
