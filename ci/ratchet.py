#!/usr/bin/env python3
"""ratchet.py good.json exp.json ties.txt: list matched fns lost/gained vs the last accepted build.
Exit 2 if a fn outside ties.txt (and not a $L label) was lost."""
import json, sys

def matched(p):
    return {x['name']: (x['size'], x['hits'][0]) for x in json.load(open(p)) if x['hits']}

g, e = matched(sys.argv[1]), matched(sys.argv[2])
# a renamed fn (signature change) keeps its retail address: not a loss
eaddr = {a for _, a in e.values()}
good = {n: s for n, (s, a) in g.items() if n in e or a not in eaddr}
gaddr = {a for _, a in g.values()}
exp = {n: s for n, (s, a) in e.items() if n in g or a not in gaddr}
try:
    ties = {l.strip() for l in open(sys.argv[3]) if l.strip() and not l.startswith('#')}
except FileNotFoundError:
    ties = set()
real = lambda n: not n.startswith('$L') and n not in ties
lost = sorted(((good[n], n) for n in good if n not in exp), reverse=True)
gain = sorted(((exp[n], n) for n in exp if n not in good), reverse=True)
for tag, rows in (('LOST', lost), ('GAIN', gain)):
    for s, n in rows:
        print('%s %6d %s%s' % (tag, s, n[:70], '' if real(n) else '  (tie/label)'))
bad = [n for _, n in lost if real(n)]
if bad:
    print('RATCHET REJECT: %d real fns lost (%d B). Revert or add to ties.txt with a reason.' % (len(bad), sum(good[n] for n in bad)))
    sys.exit(2)
print('RATCHET OK')
