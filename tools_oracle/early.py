#!/usr/bin/env python3
"""early(F): F referenced (X) before its own compile (F) = keeps cdecl. Label retail status from builds
where F matched. usage: early.py ordA cmpA [ordB cmpB ...] -> label table + per-build disagreements"""
import json, sys
def early(p):
    seen, comp, e = set(), set(), set()
    for l in open(p, errors='replace'):
        t, n = l[:1], l[2:].rstrip('\n')
        if t == 'F': comp.add(n)
        elif t == 'X' and n not in comp: e.add(n)
    return e, comp
def matched(p): return {f['name']: f['size'] for f in json.load(open(p)) if f['hits']}
builds = [(early(sys.argv[i]), matched(sys.argv[i + 1])) for i in range(1, len(sys.argv), 2)]
lab, conflict = {}, set()
for (e, comp), m in builds:
    for n, sz in m.items():
        if n not in comp: continue
        v = n in e
        if lab.get(n, v) != v: conflict.add(n)
        lab[n] = v
for n in conflict: lab.pop(n)
print(len(lab), 'labelled', sum(lab.values()), 'early;', len(conflict), 'conflicting (both matched, status differs: conv-neutral)')
for k, ((e, comp), m) in enumerate(builds):
    bad = [(n, lab[n]) for n in lab if n in comp and (n in e) != lab[n]]
    print('build', k, 'disagrees on', len(bad))
json.dump(lab, open('build/early_labels.json', 'w'))
