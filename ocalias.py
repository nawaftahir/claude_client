#!/usr/bin/env python3
"""Draft-layer link glue: alias unresolved opencod2 names to the engine definition of the same name.
Run from target/ after a link; appends to src/oc/alias.cpp."""
import collections, re
defs = collections.defaultdict(set)
for l in open('build/CoD2MP_s.map', errors='replace'):
    m = re.match(r'\s*\S+\s+(\?(\w+)@@\S*)\s', l)
    if m:
        defs[m.group(2)].add(m.group(1))
have = open('src/oc/alias.cpp').read()
new = []
for s in sorted(set(re.findall(r'unresolved external symbol "[^"]*" \((\?\S+)\)', open('build/link.log', errors='replace').read()))):
    m = re.match(r'\?(\w+)@', s)
    if m and len(defs[m.group(1)]) == 1 and s not in have:
        new.append('#pragma comment(linker, "/alternatename:%s=%s")\n' % (s, defs[m.group(1)].pop()))
open('src/oc/alias.cpp', 'a').write(''.join(new))
print(len(new), 'aliases added')
