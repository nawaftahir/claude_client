#!/usr/bin/env python3
"""TU link-order experiments: move groups of TUs to their retail position (median retail address of matched fns).
usage (in target/): tuorder.py <out.json> [g1+g2 ...]  -> lab experiments 'only_<g>' and 'allbut_<g>' (whole tus.txt edits)"""
import json, statistics, sys
text = open('src/tus.txt').read()
L = text.split('\n')
head = [l for l in L if l.startswith('#')]
tus = [l for l in L if l.strip() and not l.startswith('#')]
key = lambda t: t.split()[0].rsplit('/', 1)[-1].rsplit('.', 1)[0].lower()
hits = {}
for x in json.load(open('build/cmp.json')):
    if len(x['hits']) == 1 and not x['name'].startswith('$'):
        hits.setdefault(x['obj'].rsplit('.', 1)[0].lower(), []).append(int(x['hits'][0], 16))
addr = {i: statistics.median_low(hits[key(t)]) for i, t in enumerate(tus) if key(t) in hits and key(t) != 'imports'}
# non-movers = longest increasing run of retail addresses in the current order
pts = sorted(addr)
dp, pr = [1] * len(pts), [-1] * len(pts)
for j in range(len(pts)):
    for i in range(j):
        if addr[pts[i]] < addr[pts[j]] and dp[i] + 1 > dp[j]: dp[j], pr[j] = dp[i] + 1, i
j, keep = max(range(len(pts)), key=lambda k: dp[k]), set()
while j >= 0: keep.add(pts[j]); j = pr[j]
movers = [i for i in pts if i not in keep]
def group(i):
    p = tus[i].split()[0]
    for g, pre in (('voice', ('oc/groupvoice/', 'oc/win32/win_voice')), ('cg', ('oc/cgame_mp/', 'cgame/cg_effects')),
                   ('cl', ('oc/client_mp/',)), ('ui', ('oc/ui',)), ('snd', ('oc/win32/cinematics', 'oc/snd', 'oc/win32/snd_driver')),
                   ('win', ('win32/', 'oc/win32/win_syscon')), ('xanim', ('xanim/', 'universal/com_math')),
                   ('str', ('stringed/', 'universal/assertive'))):
        if p.startswith(pre): return g
    return 'misc'
G = {}
for i in movers:
    G.setdefault(group(i), set()).add(i)
    G[key(tus[i])] = {i}  # single TU by basename
def order(moved):
    k, prev = [], 0
    for i, t in enumerate(tus):
        if i in keep: prev = addr[i]
        k.append((addr[i] if i in moved else prev, i, t))  # everything else rides with its predecessor
    return '\n'.join(head + [t for _, _, t in sorted(k)]) + '\n'
exps = []
for spec in sys.argv[2:]:  # explicit group combinations
    exps.append({'id': 'set_' + spec, 'edits': [{'file': 'src/tus.txt', 'old': text, 'new': order(set().union(*(G[g] for g in spec.split('+'))))}]})
for g, s in ([] if sys.argv[2:] else sorted(G.items())):
    exps.append({'id': 'only_' + g, 'edits': [{'file': 'src/tus.txt', 'old': text, 'new': order(s)}]})
    exps.append({'id': 'allbut_' + g, 'edits': [{'file': 'src/tus.txt', 'old': text, 'new': order(set(movers) - s)}]})
json.dump(exps, open(sys.argv[1], 'w'))
print(len(exps), 'experiments')
