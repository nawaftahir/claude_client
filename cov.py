#!/usr/bin/env python3
"""Union of client .text bytes covered by uniquely-placed matches in a cmp.py result, split by origin."""
import collections, json, sys
r = json.load(open(sys.argv[1]))
speex = {'bits', 'cb_search', 'filters', 'lpc', 'lsp', 'ltp', 'misc', 'modes', 'nb_celp', 'quant_lsp',
         'sb_celp', 'speex', 'speex_callbacks', 'vbr', 'vq'}

def kind(obj):
    if ':' in obj:
        return 'msvc libs'
    if obj.rsplit('.', 1)[0].lower() in speex:
        return 'speex'
    return 'game source'

cov = collections.defaultdict(set)
u = [x for x in r if len(x['hits']) == 1]
for x in u:
    a = int(x['hits'][0], 16)
    cov[kind(x['obj'])].update(range(a, a + x['size']))
allb = set().union(*cov.values())
print('fns %d matched %d unique %d | unique bytes %d = %.2f%% of 1,408,683' % (
    len(r), sum(1 for x in r if x['hits']), len(u), len(allb), len(allb) / 14086.83))
for k in sorted(cov):
    print('  %-12s %7d B  %.2f%%' % (k, len(cov[k]), len(cov[k]) / 14086.83))
