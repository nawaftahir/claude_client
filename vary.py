#!/usr/bin/env python3
"""Try source variants of one line against the client, keep the best.
usage: vary.py <mangled-prefix> <client-addr> <src-file> <line> [variant ...]
No variants given: auto-generate compare/ternary flips of that line.
Score = diff lines from fdiff (0 = match). Original is restored unless a variant scores strictly better."""
import os, re, subprocess, sys
D = os.path.dirname(os.path.abspath(__file__))
T = D + '/target'
CL = D + '/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
FLIP = {'<': '>', '>': '<', '<=': '>=', '>=': '<=', '==': '==', '!=': '!='}
NEG = {'<': '>=', '>': '<=', '<=': '>', '>=': '<', '==': '!=', '!=': '=='}
OPND = r'(-?[\w\.\->\[\]\(\)]+?)'
CMP = re.compile(OPND + r'\s*(<=|>=|==|!=|<|>)\s*' + OPND + r'(?=\s*(\)|&&|\|\||\?|;|$))')

def score(name, addr):
    r = subprocess.run(['make'], cwd=T, capture_output=True, text=True)
    if r.returncode:
        return 10**6, 'build error'
    out = subprocess.run(['python3', D + '/fdiff.py', 'build/CoD2MP_s.exe', 'build/CoD2MP_s.map', CL, name, addr, '400'],
                         cwd=T, capture_output=True, text=True).stdout
    n = sum(1 for l in out.splitlines() if l[:1] in '+-' and not l.startswith(('+++', '---')))
    return n, out.splitlines()[0] if out else ''

def intval(s):
    try:
        return int(s, 0)
    except ValueError:
        return None

def auto(line):
    vs = set()
    for m in CMP.finditer(line):
        a, op, b = m.group(1), m.group(2), m.group(3)
        pre, post = line[:m.start()], line[m.end():]
        vs.add(pre + '%s %s %s' % (b, FLIP[op], a) + post)            # swap operands
        k = intval(b)
        if k is not None:                                               # integer bound shift
            if op == '>': vs.add(pre + '%s >= %d' % (a, k + 1) + post)
            if op == '>=': vs.add(pre + '%s > %d' % (a, k - 1) + post)
            if op == '<': vs.add(pre + '%s <= %d' % (a, k - 1) + post)
            if op == '<=': vs.add(pre + '%s < %d' % (a, k + 1) + post)
        if b.startswith('-'):                                           # move negation
            vs.add(pre + '-%s %s %s' % (a, FLIP[op], b[1:]) + post)
            vs.add(pre + '%s %s -%s' % (b[1:], FLIP[op], a) + post)
        elif a.startswith('-'):
            vs.add(pre + '%s %s -%s' % (a[1:], FLIP[op], b) + post)
    t = re.search(r'(.*?)\(?\s*(.+?)\s*(<=|>=|==|!=|<|>)\s*(.+?)\s*\)?\s*\?\s*(.+?)\s*:\s*(.+?)\s*;\s*$', line)
    if t:                                                               # ternary: negate test, swap arms
        pre, a, op, b, x, y = t.groups()
        vs.add('%s%s %s %s ? %s : %s;' % (pre, a, NEG[op], b, y, x))
    lit = re.compile(r'(?<![\w.])(\d+\.\d*(?:e-?\d+)?)(f?)(?![\w.])')
    if lit.search(line):                                                # float/double literal suffix
        vs.add(lit.sub(lambda m: m.group(1) + ('' if m.group(2) else 'f'), line))
    vs.discard(line)
    return sorted(vs)

def main():
    name, addr, path, ln = sys.argv[1], sys.argv[2], os.path.join(T, sys.argv[3]), int(sys.argv[4])
    src = open(path).read().split('\n')
    orig = src[ln - 1]
    indent = orig[:len(orig) - len(orig.lstrip())]
    vs = [indent + v.strip() for v in (sys.argv[5:] or auto(orig))]
    best, bline = score(name, addr)[0], orig
    print('%5d  (original) %s' % (best, orig.strip()))
    for v in vs:
        src[ln - 1] = v
        open(path, 'w').write('\n'.join(src))
        n, info = score(name, addr)
        print('%5s  %s' % (n if n < 10**6 else 'ERR', v.strip()))
        if n < best:
            best, bline = n, v
        if n == 0:
            break
    src[ln - 1] = bline
    open(path, 'w').write('\n'.join(src))
    if bline != orig:
        score(name, addr)
    print('kept (%d): %s' % (best, bline.strip()))

if __name__ == '__main__':
    main()
