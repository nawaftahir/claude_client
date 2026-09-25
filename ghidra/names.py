#!/usr/bin/env python3
"""Client 1.0 address -> name: exact matches (cmp.json), vote-located (cls.json), codex known names."""
import json, re, sys
here = __file__.rsplit('/', 1)[0]
out = {}
def plain(m):
    g = re.match(r'\?(\w+)@(\w+)?@', m)
    if not g:
        return m.lstrip('_')
    return g.group(2) + '::' + g.group(1) if g.group(2) else g.group(1)
for x in json.load(open(here + '/../../codex/client/salvage_gemini/data/names_1.0.json'))['functions']:
    if not x['name'].startswith(('FUN_', 'sub_')):
        out[int(x['addr'], 16)] = (x['name'], 'codex')
for x in json.load(open(sys.argv[2])):
    if x.get('addr') and x.get('votes', 0) >= 4:
        out[int(x['addr'], 16)] = (plain(x['name']), 'located')
for x in json.load(open(sys.argv[1])):
    if len(x['hits']) == 1:
        out[int(x['hits'][0], 16)] = (plain(x['name']), 'exact')
with open(sys.argv[3], 'w') as f:
    for a in sorted(out):
        f.write('%x\t%s\t%s\n' % (a, out[a][0], out[a][1]))
import collections
print(len(out), collections.Counter(v[1] for v in out.values()))
