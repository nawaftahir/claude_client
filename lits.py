#!/usr/bin/env python3
"""Source forms gcc canonicalized away, recovered from the client, per function body:
- float literal suffixes: our x87 constant operand is a double where the client's is a float (or
  the reverse); gcc narrows exact double constants in float expressions.
- comparison constants: gcc turns 'x < 63' into 'x <= 62'; MSVC keeps the source form."""
import difflib, glob, json, os, re, struct, sys
import pefile, capstone

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
CONST = re.compile(r'^(f\w+) (qword|dword) ptr \[(0x[0-9a-f]+)\]$')

class Img:
    def __init__(self, path):
        self.pe = pefile.PE(path)
        self.ib = self.pe.OPTIONAL_HEADER.ImageBase
        t = self.pe.sections[0]
        self.text, self.base = t.get_data(), self.ib + t.VirtualAddress

    def insns(self, va, n):
        return list(md.disasm(self.text[va - self.base:va - self.base + n], va))

    def value(self, va, kind):
        b = self.pe.get_data(va - self.ib, 8 if kind == 'qword' else 4)
        return struct.unpack('<d' if kind == 'qword' else '<f', b)[0]

def norm(i):
    op = i.op_str
    if i.mnemonic == 'call' or (i.mnemonic.startswith('j') and i.size >= 5):
        op = 'X'
    elif i.mnemonic.startswith('j'):
        op = 'x'
    return '%s %s' % (i.mnemonic, re.sub(r'0x[0-9a-f]{5,}', 'A', op))

def wants(ours, client, name, va, size, caddr):
    a, b = ours.insns(va, size), client.insns(caddr, size + 256)
    na, nb = [norm(i) for i in a], [norm(i) for i in b][:len(a) + 40]
    out = {}
    for t, i1, i2, j1, j2 in difflib.SequenceMatcher(None, na, nb, autojunk=False).get_opcodes():
        if t not in ('equal', 'replace') or i2 - i1 != j2 - j1:
            continue
        for x, y in zip(a[i1:i2], b[j1:j2]):
            cx, cy = CMP.match('%s %s' % (x.mnemonic, x.op_str)), CMP.match('%s %s' % (y.mnemonic, y.op_str))
            if cx and cy and cx.group(1) == cy.group(1):
                va_, vb_ = int(cx.group(2), 0), int(cy.group(2), 0)
                if abs(va_ - vb_) == 1:
                    out.setdefault(('cmp', va_), set()).add(vb_)
                elif va_ == vb_:
                    out.setdefault(('cmp', va_), set()).add(va_)
                continue
            mx, my = CONST.match('%s %s' % (x.mnemonic, x.op_str)), CONST.match('%s %s' % (y.mnemonic, y.op_str))
            if not mx or not my or mx.group(1) != my.group(1):
                continue
            if mx.group(2) == my.group(2):
                continue
            try:
                v = ours.value(int(mx.group(3), 16), mx.group(2))
            except Exception:
                continue  # a global, not a constant
            w = 'f' if my.group(2) == 'dword' else 'd'
            out.setdefault(('lit', v), set()).add(w)
    return {k: w.pop() for k, w in out.items() if len(w) == 1 and not (k[0] == 'cmp' and k[1] in w)}

CMP = re.compile(r'^cmp (.+), (-?(?:0x[0-9a-f]+|\d+))$')
INT = re.compile(r'(?<![<>=!-])(<=|>=|<(?![<=])|>(?![>=]))(\s*)(0x[0-9a-fA-F]+|\d+)\b(?![.\w])')

def cmpsub(line, want, count):
    def sub(m):
        op, sp, lit = m.group(1), m.group(2), m.group(3)
        v = int(lit, 0)
        b = want.get(('cmp', v))
        if b is None:
            return m.group(0)
        new = {(1, '<='): '<', (1, '>'): '>=', (-1, '<'): '<=', (-1, '>='): '>'}.get((b - v, op))
        if not new:
            return m.group(0)
        count[0] += 1
        return new + sp + (hex(b) if lit.lower().startswith('0x') else str(b))
    return INT.sub(sub, line)

LIT = re.compile(r'(?<![\w.])((?:\d+\.\d*|\.\d+)(?:[eE][+-]?\d+)?|\d+[eE][+-]?\d+)([fF]?)(?![\w.])')

def body(lines, fname):
    pat = re.compile(r'^[\w].*\b%s\s*\(' % re.escape(fname))
    for k, l in enumerate(lines):
        if pat.match(l) and not l.rstrip().endswith(';'):
            e = k
            while e < len(lines) and lines[e].rstrip() != '}':
                e += 1
            return k, e
    return None

def edit(path, fname, want):
    lines = open(path, errors='replace').read().split('\n')
    r = body(lines, fname)
    if not r:
        return 0
    n = 0
    for k in range(r[0], r[1] + 1):
        def sub(m):
            nonlocal n
            v = float(m.group(1))
            w = want.get(('lit', v))
            if w == 'f' and not m.group(2) and struct.unpack('<f', struct.pack('<f', v))[0] == v:
                n += 1
                return m.group(1) + 'f'
            if w == 'd' and m.group(2):
                n += 1
                return m.group(1)
            return m.group(0)
        lines[k] = LIT.sub(sub, lines[k])
        c = [0]
        lines[k] = cmpsub(lines[k], want, c)
        n += c[0]
    if n:
        open(path, 'w').write('\n'.join(lines))
    return n

if __name__ == '__main__':
    img, mapf, target, cls, src = sys.argv[1:6]
    ours, client = Img(img), Img(target)
    syms = {m.group(1): int(m.group(2), 16) for m in
            (re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l) for l in open(mapf, errors='replace')) if m}
    files = {os.path.splitext(os.path.basename(p))[0].lower(): p for p in glob.glob(src + '/**/*.c*', recursive=True)}
    total = fns = 0
    for x in json.load(open(cls)):
        if not x['addr'] or x['name'] not in syms:
            continue
        w = wants(ours, client, x['name'], syms[x['name']], x['size'], int(x['addr'], 16))
        path = files.get(os.path.splitext(x['obj'])[0].lower())
        m = re.match(r'\?(\w+)@', x['name'])
        if not w or not path or not m:
            continue
        n = edit(path, m.group(1), w)
        if n:
            total += n
            fns += 1
            print('%-50s %2d  %s' % (x['name'][:50], n, w))
    print(total, 'edits in', fns, 'functions')
