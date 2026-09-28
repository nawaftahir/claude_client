#!/usr/bin/env python3
"""From lab.py results: pick edits whose own function (edit 'fn') gained in its batch, or whole experiments with
net > 0 and no 'fn' tags.
  promote.py cands <exps.json> <out.jsonl> <confirm.json>   write one experiment per candidate edit
  promote.py apply <confirm.json> <confirm.out.jsonl>       apply every net-positive candidate to target/ (master)"""
import json, os, sys
D = os.path.dirname(os.path.abspath(__file__))

def results(p):
    r = {}
    for l in open(p):
        x = json.loads(l); r[x['id']] = x  # last run of an id wins
    return r

def cands(exps, out, dst):
    res, c = results(out), []
    for e in json.load(open(exps)):
        x = res.get(e['id'])
        if not x or not x.get('ok'): continue
        gained = {n for _, n in x.get('gained', [])}
        tagged = [ed for ed in e['edits'] if ed.get('fn') in gained and ed.get('fn') not in x.get('dropped', [])]
        for ed in tagged:
            c.append({'id': '%s:%s' % (e['id'], ed['fn'][:60]), 'edits': [ed]})
        if not any('fn' in ed for ed in e['edits']) and x['net'] > 0:
            c.append({'id': e['id'], 'edits': e['edits']})
    json.dump(c, open(dst, 'w'), indent=0)
    print(len(c), 'candidates')

def apply(conf, out):
    res, n = results(out), 0
    for e in json.load(open(conf)):
        x = res.get(e['id'])
        if not x or not x.get('ok') or x['net'] <= 0: continue
        for ed in e['edits']:
            p = os.path.join(D, 'target', ed['file'])
            s = open(p, errors='surrogateescape').read()
            if 'line' in ed:
                L = s.split('\n')
                if L[ed['line'] - 1] != ed['old']:
                    print('skip (moved)', e['id']); break
                L[ed['line'] - 1] = ed['new']; s = '\n'.join(L)
            elif s.count(ed['old']) == 1:
                s = s.replace(ed['old'], ed['new'])
            else:
                print('skip (ambiguous)', e['id']); break
            open(p, 'w', errors='surrogateescape').write(s)
        else:
            n += 1; print('applied %+d %s' % (x['net'], e['id']))
    print(n, 'applied')

if __name__ == '__main__':
    {'cands': lambda: cands(*sys.argv[2:5]), 'apply': lambda: apply(*sys.argv[2:4])}[sys.argv[1]]()
