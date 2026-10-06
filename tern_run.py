#!/usr/bin/env python3
# usage (in target/): tern_run.py DIR  (reads DIR/tern.json; do not edit src while it runs)
# apply each ternary candidate, build+measure, keep if net bytes improve
import json, re, subprocess, sys
SP = sys.argv[1]
cands = json.load(open(SP + '/tern.json'))
def measure():
    subprocess.run('make -s > build/b.txt 2>&1', shell=True)
    if 'error' in open('build/b.txt', errors='replace').read().lower().replace('0 error', ''):
        return None
    o = subprocess.run('../m.sh nobuild | head -1', shell=True, capture_output=True, text=True).stdout
    m = re.search(r'unique bytes (\d+)', o)
    return int(m.group(1)) if m else None
base = measure(); print('base', base, flush=True)
subprocess.run('cp build/cmp.json build/cmp_base.json', shell=True)
for c in cands:
    s = open(c['file']).read()
    t = c['text']
    if t not in s: print('skip-nf', c['name'], flush=True); continue
    head = t[:t.index('{')]
    new = head + '{\n\treturn %s ? %s : %s;\n}' % (c['cond'], c['a'], c['b'])
    open(c['file'], 'w').write(s.replace(t, new, 1))
    v = measure()
    d = None if v is None else v - base
    gl = ''
    if v is not None:
        a = {x['name']: bool(x['hits']) for x in json.load(open('build/cmp_base.json'))}
        b = {x['name']: bool(x['hits']) for x in json.load(open('build/cmp.json'))}
        g = [n for n in b if b[n] and not a.get(n) and not n.startswith('$L')]
        l = [n for n in a if a[n] and not b.get(n) and not n.startswith('$L')]
        gl = ' +%s -%s' % (g[:6], l[:6])
    if v is not None and d > 0:
        base = v; subprocess.run('cp build/cmp.json build/cmp_base.json', shell=True)
        print('KEEP %+d %s %s%s' % (d, c['name'], c['file'], gl), flush=True)
    else:
        open(c['file'], 'w').write(s)
        print('rev %s %s%s' % (d, c['name'], gl), flush=True)
measure(); print('final', base, flush=True)
