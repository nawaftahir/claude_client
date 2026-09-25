#!/usr/bin/env python3
"""Cast draft call-site arguments that no longer convert after ocdecl.py (C2664).
usage (from target/): occast.py   (reads build/obj/oc/**.log, edits src/oc in place)"""
import collections, glob, re
fix = collections.defaultdict(list)
for log in glob.glob('build/obj/oc/**/*.log', recursive=True):
    for l in open(log, errors='replace'):
        m = re.match(r"(oc\\[^(]+)\((\d+)\) : error C2664: '(\w+)' : cannot convert parameter (\d+) from '[^']+' to '([^']+)'", l)
        if m:
            fix['src/' + m.group(1).replace('\\', '/')].append((int(m.group(2)), m.group(3), int(m.group(4)), m.group(5)))

def cast_arg(line, name, n, ty):
    for m in re.finditer(r'\b%s\s*\(' % name, line):
        i, depth, start, k = m.end(), 0, m.end(), 1
        while i < len(line):
            c = line[i]
            if c in '([':
                depth += 1
            elif c in ')]' and depth:
                depth -= 1
            elif (c == ',' or c == ')') and depth == 0:
                if k == n:
                    arg = line[start:i].strip()
                    lead = line[start:i][:len(line[start:i]) - len(line[start:i].lstrip())]
                    return line[:start] + lead + '(%s)%s' % (ty, arg if re.fullmatch(r'[\w.\->\[\]&]+', arg) else '(%s)' % arg) + line[i:]
                if c == ')':
                    break
                k, start = k + 1, i + 1
            i += 1
    return None

n = 0
for p, items in fix.items():
    L = open(p, errors='replace').read().split('\n')
    for ln, name, arg, ty in sorted(set(items)):
        r = cast_arg(L[ln - 1], name, arg, ty)
        if r is None:
            print('skip', p, ln, name, arg)
        else:
            L[ln - 1], n = r, n + 1
    open(p, 'w').write('\n'.join(L))
print(n, 'casts')
