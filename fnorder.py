#!/usr/bin/env python3
"""Reorder a TU's function definitions to retail address order.
usage (from target/): fnorder.py <src.cpp> [-n]   (-n = dry run, prints the plan)
Chunks = top-level function defs (with leading comments); other top-level text keeps its relative
order ahead of the moved block. Fns without a retail address stay glued to their predecessor."""
import json, re, sys
p = sys.argv[1]
L = open(p).read().split('\n')
names = {}
for f in ('names_fix.tsv', 'names_k.tsv', 'names_1.0.tsv', 'names_cg.tsv', 'names_loc.tsv'):
    for l in open('../ghidra/' + f):
        r = l.rstrip('\n').split('\t')
        if len(r) >= 2 and re.fullmatch(r'[0-9a-fA-F]+', r[0]):
            names.setdefault(r[1], int(r[0], 16))
for e in json.load(open('build/cmp.json')):
    if len(e['hits']) == 1:
        names[re.sub(r'^[?_]([A-Za-z_0-9]+).*', r'\1', e['name'])] = int(e['hits'][0], 16)
# find function defs: a line '{' at col 0 preceded by a signature ending in ')'
# incom[i]: line i starts inside a /* */ comment (commented-out fns must not be chunked)
incom, c = [], False
for l in L:
    incom.append(c)
    k = 0
    while k < len(l):
        if c:
            j = l.find('*/', k)
            if j < 0:
                break
            c, k = False, j + 2
        else:
            m = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//|/\*').search(l, k)
            if not m or m.group() == '//':
                break
            if m.group() == '/*':
                c = True
            k = m.end()
chunks, i, sigs = [], 0, {}
while i < len(L):
    if L[i] == '{' and i > 0 and not incom[i] and L[i - 1].rstrip().endswith(')') and not L[i - 1].startswith((' ', '\t', '#')):
        s = i - 1
        while s > 0 and L[s - 1] and not L[s - 1].rstrip().endswith((';', '}', ')')) and not L[s - 1].startswith(('//', '#')) and not incom[s - 1] and not L[s - 1].rstrip().endswith('*/') \
                and L[s - 1].startswith((' ', '\t')):
            s -= 1  # multi-line signature continuation
        while s > 0 and not L[s - 1].startswith(' ') and re.match(r'^[A-Za-z_].*[,(]\s*$', L[s - 1]):
            s -= 1
        g = s  # signature start
        while s > 0 and (L[s - 1].startswith('//') or L[s - 1].rstrip().endswith('*/')):
            if L[s - 1].startswith('//'):
                s -= 1
                continue
            s -= 1  # leading /* */ banner
            while s > 0 and '/*' not in L[s]:
                s -= 1
        e, dep, cm = i, 0, False  # brace-match (col-0 '}' of a label block is not the end)
        while True:
            t = re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', '', L[e])
            k = 0
            while k < len(t):
                if cm:
                    j = t.find('*/', k)
                    if j < 0:
                        break
                    cm, k = False, j + 2
                    continue
                if t.startswith('//', k):
                    break
                if t.startswith('/*', k):
                    cm, k = True, k + 2
                    continue
                dep += {'{': 1, '}': -1}.get(t[k], 0)
                k += 1
            if dep == 0:
                break
            e += 1
        m = [x for x in re.finditer(r'([A-Za-z_][A-Za-z_0-9]*)\s*\(', ' '.join(L[g:i]))
             if x.group(1) not in ('__attribute__', '__declspec', 'always_inline', 'noinline')][0]
        chunks.append((s, e, m.group(1)))
        sigs[m.group(1)] = ' '.join(x.strip() for x in L[g:i])
        i = e + 1
    else:
        i += 1
# refuse files where a moved fn sits inside a top-level #if block (would break the conditional)
depth, dl = 0, []
for l in L:
    t = l.strip()
    if t.startswith('#if'):
        depth += 1
    elif t.startswith('#endif'):
        depth -= 1
    dl.append(depth)
if any(dl[s] or dl[e] for s, e, n in chunks):
    sys.exit('skip %s: fn inside #if' % p)
if any(re.match(r'\s*#\s*(undef|pragma)', L[k]) for k in range(chunks[0][0], chunks[-1][1]) if not any(cs <= k <= ce for cs, ce, _ in chunks)):
    sys.exit('skip %s: #undef/#pragma between fns' % p)
start = chunks[0][0]
end = chunks[-1][1] + 1
other, fns, pos = [], [], start
for s, e, n in chunks:
    if pos < s:
        other.append(L[pos:s])
    fns.append([n, L[s:e + 1]])
    pos = e + 1
# glue fns without an address to the previous one
import statistics
med = statistics.median([names[n] for n, _ in fns if names.get(n, 0) > 0])
groups, far = [], []
for n, body in fns:
    a = names.get(n)
    if a is not None and a > 0 and abs(a - med) > 0x30000:
        far.append(body)  # shared helper placed elsewhere in retail: keep it ahead
        continue
    if not a:
        a = None
    if a is None and groups:
        groups[-1][2].append((n, body))
    else:
        groups.append([a if a is not None else -1, n, [(n, body)]])
order = sorted(groups, key=lambda g: g[0])
if '-n' in sys.argv:
    for a, n, mem in order:
        print('%x %s %s' % (a & 0xffffffff, n, ' '.join(m for m, _ in mem[1:])))
    sys.exit()
out = L[:start]
for o in other:
    o = [x for x in o if x.strip()]
    if o:
        out += o + ['']
for body in far:
    out += body + ['']
for a, n, mem in order:
    for m, body in mem:
        out += body + ['']
out += L[end:]
# fns now used before their definition get a prototype ahead of the moved block
protos = []
for n, b in fns:
    d = next(i for i, x in enumerate(out) if x == b[b.index('{') - 1] and out[i + 1] == '{')
    if '::' not in sigs[n] and 'operator' not in sigs[n] and any(re.search(r'\b%s\b' % n, x) for x in out[start:d - 1]):
        protos.append(sigs[n] + ';')
out[start:start] = protos + [''] if protos else []
open(p, 'w').write('\n'.join(out))
