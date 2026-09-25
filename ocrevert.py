#!/usr/bin/env python3
"""Undo ocdecl.py for names that break a draft TU: restore the draft's own declaration from opencod2.
usage (from target/): ocrevert.py [name...]   (reads build/obj/oc/**.log)"""
import glob, re, sys
extra = set(sys.argv[1:])
sys.argv = [sys.argv[0]]
REF = '../../refrences/opencod2/src/'
src = open('../ocdecl.py').read()
exec(src.split('want = ')[0].replace("open('oc/alias.cpp')", "open('src/oc/alias.cpp')").replace("glob.glob('**/", "glob.glob('src/**/").replace("p.startswith('oc/')", "p.startswith('src/oc/')").replace("glob.glob('oc/", "glob.glob('src/oc/"))
decl = {'%s;' % v: k for k, v in found.items()}
bad = set(extra)
for log in glob.glob('build/obj/oc/**/*.log', recursive=True):
    for l in open(log, errors='replace'):
        m = re.match(r'(oc\\[^(]+)\((\d+)\) : (?:fatal )?error', l)
        if not m:
            continue
        bad |= {w for w in re.findall(r"'(\w+)'", l) if w in found}
        path = 'src/' + m.group(1).replace('\\', '/')
        try:
            line = open(path, errors='replace').read().split('\n')[int(m.group(2)) - 1].strip()
        except (OSError, IndexError):
            continue
        for d, name in decl.items():
            if line.endswith(d):
                bad.add(name)
print('reverting', sorted(bad))
def refdecl(path, name):
    rel = path[len('src/oc/'):]
    for cand in [REF + 'PC/' + re.sub(r'\.cpp$', '.c', rel), REF + rel]:
        try:
            for l in open(cand, errors='replace'):
                if re.match(r'^\s*(?:extern\s+)?[\w][\w\s\*]*?\b%s\s*\(.*\)\s*;' % name, l):
                    return l.rstrip('\n')
        except OSError:
            pass
for p in glob.glob('src/oc/**/*.[ch]*', recursive=True):
    L = open(p, errors='replace').read().split('\n')
    ch = 0
    for k, l in enumerate(L):
        for name in bad:
            if l.strip().endswith(found[name] + ';'):
                r = refdecl(p, name)
                if r is None:
                    print('no reference line:', p, name)
                    continue
                L[k] = r
                ch += 1
    if ch:
        open(p, 'w').write('\n'.join(L))
