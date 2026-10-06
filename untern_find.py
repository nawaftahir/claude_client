#!/usr/bin/env python3
# usage (in target/): untern_find.py OUT.json -- small fns whose body is one `return c ? a : b;` (inverse of tern_find)
import re, os, sys, json
pat = re.compile(r'\n((?:static |inline |__forceinline |TU_STATIC )*[\w\*\s]+?\b(\w+)\s*\(([^;{)]*)\)\s*\n\{\s*\n\s*return\s+([^;]+);\s*\n\})')
out = []
for root, _, fs in os.walk('src'):
    for f in fs:
        if not f.endswith(('.cpp', '.c', '.h')): continue
        p = os.path.join(root, f); s = open(p, errors='replace').read()
        for m in pat.finditer(s):
            e = m.group(4)
            if e.count('?') != 1 or e.count(':') - e.count('::') != 1: continue
            c, rest = e.split('?'); a, b = rest.rsplit(':', 1)
            if c.count('(') != c.count(')') or a.count('(') != a.count(')'): continue
            out.append({'file': p, 'name': m.group(2), 'cond': c.strip(), 'a': a.strip(), 'b': b.strip(), 'text': m.group(1)})
json.dump(out, open(sys.argv[1], 'w'), indent=0)
print(len(out))
