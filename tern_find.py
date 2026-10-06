#!/usr/bin/env python3
# usage (in target/): tern_find.py OUT.json
# find small fns whose whole body is if(c) return A; [else] return B;
import re, os, sys, json
pat = re.compile(r'\n((?:static |inline |__forceinline |TU_STATIC )*[\w\*\s]+?\b(\w+)\s*\(([^;{)]*)\)\s*\n\{\s*\n'
                 r'\s*if\s*\((.+?)\)\s*\n?\s*\{?\s*\n?\s*return\s+([^;]+);\s*\n?\s*\}?\s*\n?\s*(?:else\s*\n?\s*\{?\s*\n?\s*)?return\s+([^;]+);\s*\n?\s*\}?\s*\n\})')
out = []
for root, _, fs in os.walk('src'):
    for f in fs:
        if not f.endswith(('.cpp', '.c', '.h')): continue
        p = os.path.join(root, f); s = open(p, errors='replace').read()
        for m in pat.finditer(s):
            body = m.group(1)
            if body.count('\n') > 14 or '?' in m.group(4): continue
            out.append({'file': p, 'name': m.group(2), 'cond': m.group(4).strip(), 'a': m.group(5).strip(), 'b': m.group(6).strip(), 'text': body})
json.dump(out, open(sys.argv[1], 'w'), indent=0)
print(len(out))
for o in out[:400]: print(o['file'], o['name'], '|', o['cond'], '?', o['a'], ':', o['b'])
