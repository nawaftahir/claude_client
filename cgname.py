#!/usr/bin/env python3
"""Name client 1.0 functions by call-graph alignment with the Mac 1.3 binary (STABS-named).
Known pairs (same name both sides) vote their callee lists onto each other; iterate to a fixpoint.
usage: cgname.py names_1.0.tsv ghidra_out_dir out.tsv"""
import collections, difflib, json, os, re, struct, sys
import capstone
import pefile

MAC = '/home/nawaf/cod2/test/Refrences/CoD2x/bin/mac/Call of Duty 2 Multiplayer 1.3'
PC = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

def plain(s):
    m = re.match(r'_ZN(\d+)(\w+)', s)
    if m:
        n = int(m.group(1))
        cls, rest = m.group(2)[:n], m.group(2)[n:]
        m2 = re.match(r'(\d+)(\w+)', rest)
        return cls + '::' + m2.group(2)[:int(m2.group(1))] if m2 else cls
    m = re.match(r'_Z(\d+)(\w+)', s)
    if m:
        return m.group(2)[:int(m.group(1))]
    return s[1:] if s.startswith('_') else s

def mac_graph():
    d = open(MAC, 'rb').read()
    for i in range(struct.unpack('>I', d[4:8])[0]):
        cpu, _, off, size, _ = struct.unpack('>5I', d[8 + i * 20:28 + i * 20])
        if cpu == 7:
            m = d[off:off + size]
    p = 28
    for _ in range(struct.unpack('<I', m[16:20])[0]):
        cmd, sz = struct.unpack('<II', m[p:p + 8])
        if cmd == 1 and m[p + 8:p + 24].rstrip(b'\0') == b'__TEXT':
            nsects = struct.unpack('<I', m[p + 48:p + 52])[0]
            for k in range(nsects):
                s = p + 56 + k * 68
                sect = m[s:s + 16].rstrip(b'\0')
                addr, size, off = struct.unpack('<III', m[s + 32:s + 44])
                if sect == b'__text':
                    text, base = m[off:off + size], addr
                elif sect == b'__cstring':
                    cstr = (addr, m[off:off + size])
        p += sz
    fns = {}
    for f, lst in json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'mac_stabs.json')))['files'].items():
        for n, a in lst:
            fns[int(a, 16)] = plain(n)
    return graph(text, base, fns, lambda v: cstring(cstr, v))

def cstring(sec, v):
    a, data = sec
    if not a <= v < a + len(data):
        return None
    e = data.find(b'\0', v - a)
    s = data[v - a:e]
    return s.decode('latin1') if len(s) >= 6 else None

def graph(text, base, fns, strat):
    starts = sorted(fns)
    g = {}
    for i, a in enumerate(starts):
        end = starts[i + 1] if i + 1 < len(starts) else base + len(text)
        if not base <= a < base + len(text):
            continue
        calls, strs = [], set()
        for ins in md.disasm(text[a - base:end - base], a):
            if ins.mnemonic in ('call', 'jmp') and ins.size == 5:
                t = int(ins.op_str, 16)
                if t in fns and t != a:
                    calls.append(t)
                continue
            for v in re.findall(r'0x[0-9a-f]{5,}', ins.op_str):
                x = strat(int(v, 16))
                if x:
                    strs.add(x)
        g[a] = calls
        STRS[a] = strs
    return g

STRS = {}

def pc_starts(pe, outdir):
    """Ghidra's starts, plus call targets and code after int3 padding (MSVC pads between functions)."""
    t = pe.sections[0]
    d, base = t.get_data(), pe.OPTIONAL_HEADER.ImageBase + t.VirtualAddress
    out = {int(f[:8], 16) for f in os.listdir(outdir)}
    for k in range(16, len(d), 16):
        if d[k - 1] == 0xcc and d[k] != 0xcc:
            out.add(base + k)
    for k in range(len(d) - 5):
        if d[k] == 0xe8:
            v = base + k + 5 + struct.unpack('<i', d[k + 1:k + 5])[0]
            if v in out or (v - base) % 16 == 0 and base <= v < base + len(d) and d[v - base - 1] in (0xcc, 0xc3, 0xc2):
                out.add(v)
    return out

def pc_graph(outdir):
    pe = pefile.PE(PC)
    t = pe.sections[0]
    ib = pe.OPTIONAL_HEADER.ImageBase
    fns = {a: None for a in pc_starts(pe, outdir)}
    img = pe.get_memory_mapped_image()
    def strat(v):
        if not ib + 0x158000 <= v < ib + len(img):
            return None
        e = img.find(b'\0', v - ib)
        x = img[v - ib:e]
        return x.decode('latin1') if len(x) >= 6 and all(32 <= c < 127 or c in (9, 10) for c in x) else None
    return graph(t.get_data(), ib + t.VirtualAddress, fns, strat)

if __name__ == '__main__':
    known = {}
    weak = set()
    for l in open(sys.argv[1]):
        a, n, src = l.rstrip('\n').split('\t')
        known[int(a, 16)] = n.replace('__', '::')
        if src == 'located':
            weak.add(int(a, 16))
    mg, mname = None, None
    mfns = {}
    mg = mac_graph()
    for f, lst in json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'mac_stabs.json')))['files'].items():
        for n, a in lst:
            mfns[int(a, 16)] = plain(n)
    mby = collections.defaultdict(list)
    for a, n in mfns.items():
        mby[n].append(a)
    pg = pc_graph(sys.argv[2])
    names = dict(known)
    pcs = {a: STRS[a] for a in pg}
    mstr = {a: STRS[a] for a in mg}
    def owners(tbl):
        o = collections.defaultdict(set)
        for a, ss in tbl.items():
            for x in ss:
                o[x].add(a)
        return o
    po, mo = owners(pcs), owners(mstr)
    sv = collections.defaultdict(collections.Counter)
    for x, pa in po.items():
        if len(pa) == 1 and len(mo.get(x, ())) == 1:
            sv[next(iter(pa))][mfns[next(iter(mo[x]))]] += 1
    agree = clash = 0
    snew = {}
    for pa, c in sv.items():
        (y, v), = c.most_common(1)
        if len(c) > 1:
            continue
        if pa in names and pa not in weak:
            agree += names[pa] == y
            clash += names[pa] != y
        else:
            snew[pa] = y
    cnt = collections.Counter(snew.values())
    strong = {n for a, n in names.items() if a not in weak}
    snew = {a: y for a, y in snew.items() if cnt[y] == 1 and y not in strong}
    for a in [a for a in weak if names.get(a) in set(snew.values()) and a not in snew]:
        del names[a]  # a weak (located) name the strings place elsewhere
    names.update(snew)
    print('strings: named', len(snew), '| vs known: agree', agree, 'clash', clash)
    for rnd in range(10):
        used = set(names.values())
        votes = collections.defaultdict(collections.Counter)
        for pa, n in names.items():
            if pa not in pg or len(mby.get(n, ())) != 1 or mby[n][0] not in mg:
                continue
            pc = pg[pa]
            mc = [mfns[x] for x in mg[mby[n][0]]]
            pl = [names.get(x, '?%x' % x) for x in pc]
            sm = difflib.SequenceMatcher(None, pl, mc, autojunk=False)
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag == 'replace' and i2 - i1 == j2 - j1:
                    for x, y in zip(pc[i1:i2], mc[j1:j2]):
                        if x not in names and y not in used:
                            votes[x][y] += 1
        new = {}
        claimed = collections.Counter()
        for x, c in votes.items():
            (y, v), = c.most_common(1)
            if v >= 2 or len(c) == 1:
                new[x] = y
                claimed[y] += 1
        new = {x: y for x, y in new.items() if claimed[y] == 1}
        if not new:
            break
        names.update(new)
        print('round', rnd, 'named', len(new))
    with open(sys.argv[3], 'w') as f:
        for a in sorted(names):
            f.write('%x\t%s\t%s\n' % (a, names[a].replace('::', '__'), 'known' if a in known else 'callgraph'))
    print('total', len(names), 'new', len(names) - len(known))
