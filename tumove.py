#!/usr/bin/env python3
"""Move TUs to their retail link slot (median retail address of matched fns).
usage (in target/): tumove.py <base tus.txt> list | all | <key,key,...>   -> rewrites src/tus.txt
'list' prints the movers (TUs off the longest retail-increasing run); TUs without hits ride with their predecessor."""
import json, statistics, sys
L = open(sys.argv[1]).read().split('\n')
key = lambda t: t.split()[0].rsplit('/', 1)[-1].rsplit('.', 1)[0].lower()
hits = {}
for x in json.load(open('build/cmp_base.json')):
    if len(x['hits']) == 1 and not x['name'].startswith('$'):
        hits.setdefault(x['obj'].rsplit('.', 1)[0].lower(), []).append(int(x['hits'][0], 16))
head, groups = [], []
for l in L:
    if not l.strip() or l.startswith('#'):
        (groups[-1][2] if groups else head).append(l); continue
    k = key(l)
    if k in hits and k != 'imports':
        groups.append([statistics.median_low(hits[k]), k, [l]])
    elif groups:
        groups[-1][2].append(l)
    else:
        head.append(l)
n = len(groups)
dp, pr = [1] * n, [-1] * n
for j in range(n):
    for i in range(j):
        if groups[i][0] < groups[j][0] and dp[i] + 1 > dp[j]: dp[j], pr[j] = dp[i] + 1, i
j, keep = max(range(n), key=lambda k: dp[k]), set()
while j >= 0: keep.add(j); j = pr[j]
movers = [groups[i][1] for i in range(n) if i not in keep]
if sys.argv[2] == 'list':
    print(','.join(movers)); sys.exit()
mv = set(movers) if sys.argv[2] == 'all' else set(sys.argv[2].split(','))
stay = [g for g in groups if g[1] not in mv]
for g in [g for g in groups if g[1] in mv]:
    p = next((i for i, s in enumerate(stay) if s[0] > g[0]), len(stay))
    stay.insert(p, g)
open('src/tus.txt', 'w').write('\n'.join(head + [x for g in stay for x in g[2]]))
