#!/usr/bin/env python3
"""Replace draft imp_X (address of X) with (void *)&X and declare X; usage (from target/src): imp2direct.py [names]"""
import re, glob, sys
imps = re.findall(r'(?m)^void \*imp_(\w+);', open('oc/globals.cpp').read())
src = {p: open(p, errors='replace').read() for p in glob.glob('**/*.c*', recursive=True)}
defs = {}
for x in imps:
    for p, s in src.items():
        m = re.search(r'(?m)^((?:const\s+)?(?:struct\s+)?[A-Za-z_]\w*(?:\s*\*)*\s+(?:const\s+)?\**\s*' + re.escape(x) + r'\b(?:\s*\[[^\]]*\])*)\s*(?:=[^;]*)?;', s)
        if m and not m.group(1).startswith('static'):
            defs[x] = (p, re.sub(r'\[[^\]]*\]', '[]', m.group(1).strip()))
            break
want = set(sys.argv[1:]) or set(defs)
n = 0
for p in [q for q in glob.glob("oc/**/*.cpp", recursive=True) if not q.endswith("globals.cpp")]:
    s = src[p]; o = s
    add = []
    for x in sorted(want & set(defs)):
        if not re.search(r'\bimp_' + x + r'\b', s):
            continue
        s = re.sub(r'(?m)^extern void \*imp_' + x + r';\n', '', s)
        s = re.sub(r'\bimp_' + x + r'\b', '((void *)&' + x + ')', s)
        if defs[x][0] != p and not re.search(r'(?m)^(extern\s+)?[^\n(]*\b' + x + r'\b(\s*\[[^\]]*\])*\s*;', s):
            add.append('extern %s;\n' % defs[x][1])
    if s != o:
        if add:
            k = [m.end() for m in re.finditer(r'(?m)^#include[^\n]*\n', s)]
            k = k[-1] if k else 0
            s = s[:k] + ''.join(add) + s[k:]
        open(p, 'w').write(s); n += 1
print(n, 'files')
