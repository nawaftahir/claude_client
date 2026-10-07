#!/usr/bin/env python3
"""Classify each function's arg convention from its own body (objdump -d -M intel).

  stack  : reads incoming stack args ([esp+N] past its frame, [ebp+8+]) and no reg live-in
  reg    : a GPR is read before written on some path (LTCG custom convention);
           ecx is ignored for thiscall (@@QAE etc), ecx/edx for fastcall (@@YI)
  noargs : neither seen
  unknown: body not analysable (no extent, indirect jmp before any evidence)

usage:
  rconv.py DIS MAP -o out.json [--ord ord_b.txt]             # our build, names from link map
  rconv.py DIS - --cmp cmp.json [--names tsv ...] -o out.json [--base ours.json]
                                                             # retail, names via cmp single hits
  --ord : validate vs ord_b early set (X before own F => must be stack)
  --base: print fns whose class differs from base json, by size (needs --cmp for sizes)
Output json: {name: class}; a side file <out>.detail.json has live-in regs / addrs / source.
"""
import json, re, sys, bisect, argparse

R32 = ['eax', 'ecx', 'edx', 'ebx', 'esi', 'edi', 'ebp']
SUB = {r: r for r in R32}
for r in 'abcd':
    for s in (r + 'l', r + 'h', r + 'x'): SUB[s] = 'e' + r + 'x'
for s, r in (('si', 'esi'), ('di', 'edi'), ('bp', 'ebp')): SUB[s] = r
REG_RE = re.compile(r'\b(' + '|'.join(sorted(SUB, key=len, reverse=True)) + r')\b')
DEST_ONLY = {'mov', 'movzx', 'movsx', 'lea', 'pop'}
SAVED = {'ebx', 'esi', 'edi', 'ebp'}
CALLEE_SAVED = SAVED | {'ecx'}  # push ecx = 4-byte alloca idiom
HEX = re.compile(r'0x[0-9a-f]+')


def load_dis(p):
    ins = {}
    for l in open(p, errors='replace'):
        if l[:2] != '  ' or '\t' not in l: continue
        a, _, rest = l.partition(':')
        try: a = int(a, 16)
        except ValueError: continue
        rest = rest.split('\t')[-1].strip()
        m, _, ops = rest.partition(' ')
        if m in ('rep', 'repz', 'repnz', 'lock'):  # keep prefix glued
            m2, _, ops = ops.strip().partition(' '); m = m + ' ' + m2
        ins[a] = (m, ops.strip())
    return ins


def regs(s): return {SUB[r] for r in REG_RE.findall(s)}


def split_ops(ops):
    # split on top-level commas (none inside [] for intel syntax except none)
    return [o.strip() for o in ops.split(',')] if ops else []


def target(ops):
    m = re.fullmatch(r'0x([0-9a-f]+)(?: <.*>)?', ops)
    return int(m.group(1), 16) if m else None


def retn(ins, addrs, s, e):
    # bytes popped by callee (ret N), None if no ret seen
    i = bisect.bisect_left(addrs, s)
    while i < len(addrs) and addrs[i] < e:
        m, o = ins[addrs[i]]
        if m == 'ret': return int(o, 16) if o else 0
        i += 1
    return None


def analyse(ins, addrs, s, e, rpop, iat_lo, iat_hi):
    """walk CFG from s within [s,e). returns (live_in set, stack_arg bool, ok)."""
    live, stk = set(), False
    best = {}  # addr -> must-written set (smallest seen)
    work = [(s, frozenset(), 0, None, True)]
    steps = 0
    while work:
        a, w, d, ebp0, pro = work.pop()
        pend = 0
        while True:
            steps += 1
            if steps > 200000 or a not in ins or not (s <= a < e): break
            if a in best and best[a] <= w: break
            best[a] = w if a not in best else best[a] & w
            w = best[a]
            m, ops = ins[a]
            op = split_ops(ops)
            nxt = addrs[bisect.bisect_right(addrs, a)] if a != addrs[-1] else None
            # stack-arg reads
            for o in op:
                if '[' not in o: continue
                mm = re.search(r'\[esp(?:\+(0x[0-9a-f]+))?\]', o)
                if mm and int(mm.group(1) or '0', 16) >= d + 4: stk = True
                mm = re.search(r'\[ebp\+(0x[0-9a-f]+)\]', o)
                if mm and ebp0 is not None and int(mm.group(1), 16) >= ebp0 + 4: stk = True
            rd, wr = set(), set()
            if (m == 'lea' and len(op) == 2 and op[1] in ('[%s+0x0]' % op[0], '[%s]' % op[0])) or \
                    (m == 'mov' and len(op) == 2 and op[0] == op[1]):
                pass  # alignment nop
            elif m == 'push':
                # callee-saved push anywhere = save (not a read); push ecx only in prologue
                if not (ops in SAVED or (pro and ops == 'ecx')): rd |= regs(ops)
                d += 4
            elif m == 'pop':
                wr |= regs(ops); d -= 4
            elif m in ('sub', 'add') and op and op[0] == 'esp':
                n = int(op[1], 16) if op[1].startswith('0x') else 0
                d += n if m == 'sub' else -n
            elif m in ('xor', 'sub', 'sbb') and len(op) == 2 and op[0] == op[1] and op[0] in SUB:
                wr |= regs(op[0])  # xor/sub/sbb r,r: no reg read
            elif m in DEST_ONLY or m.startswith('set') or (m == 'imul' and len(op) == 3):
                if op and '[' not in op[0]: wr |= regs(op[0])
                else: rd |= regs(op[0]) if op else set()
                for o in op[1:]: rd |= regs(o)
                if m == 'mov' and op[:2] == ['ebp', 'esp']: ebp0 = d
            elif m == 'call':
                rd |= regs(ops) if 'PTR' in ops or ops in SUB else set()
                t = target(ops)
                if t is not None and rpop.get(t): d -= rpop[t]
                elif 'ds:' in ops:
                    mm = HEX.search(ops)
                    if mm and iat_lo <= int(mm.group(0), 16) < iat_hi:
                        d -= pend  # stdcall import pops its pushes
                wr |= {'eax', 'ecx', 'edx'}
            elif m == 'cdq':
                rd.add('eax'); wr.add('edx')
            elif m in ('div', 'idiv', 'mul') or (m == 'imul' and len(op) == 1):
                rd |= {'eax'} | regs(ops) | ({'edx'} if m in ('div', 'idiv') else set()); wr |= {'eax', 'edx'}
            elif m.startswith('rep') or m.startswith('stos') or m.startswith('movs') or m.startswith('scas') or m.startswith('cmps'):
                rd |= {'edi'} | ({'ecx'} if m.startswith('rep') else set())
                if 'movs' in m or 'cmps' in m: rd.add('esi')
                if 'stos' in m or 'scas' in m: rd.add('eax')
                wr |= {'edi', 'esi'} | ({'ecx'} if m.startswith('rep') else set())
            elif m == 'leave':
                d = ebp0 - 4 if ebp0 is not None else d; wr.add('ebp')
            elif m.startswith('fnstsw'):
                wr.add('eax')
            elif m in ('ret', 'int3', 'hlt'):
                for o in op: rd |= regs(o)
            else:
                for o in op: rd |= regs(o)
                if op and '[' not in op[0] and m not in ('cmp', 'test', 'push') and not m.startswith('j'):
                    wr |= regs(op[0])
                if m == 'xchg' and len(op) == 2: wr |= regs(op[1])
            live |= (rd - w) - {'esp'}
            w = w | frozenset(wr)
            # track pushes since last call for stdcall import pop
            if m == 'push': pend += 4
            elif m == 'call': pend = 0
            if not ((m == 'push' and ops in CALLEE_SAVED) or (m in ('sub', 'and') and op[0] == 'esp')
                    or (m == 'mov' and op == ['ebp', 'esp'])): pro = False
            if m == 'ret' or m in ('int3', 'hlt'): break
            if m == 'jmp':
                t = target(ops)
                if t is None or not (s <= t < e):
                    break  # tail call / switch table
                a = t; continue
            if m.startswith('j') or m == 'loop':
                t = target(ops)
                if t is not None: work.append((t, w, d, ebp0, False))
            if nxt is None: break
            a = nxt
    return live, stk


def classify(name, live, stk):
    drop = set()
    if name and re.search(r'@@[QAIUM][AEI]E', name): drop |= {'ecx'}  # thiscall
    if name and '@@YI' in name: drop |= {'ecx', 'edx'}  # fastcall
    live = live - drop - {'ebp'}
    if live: return 'reg', sorted(live)
    return ('stack' if stk else 'noargs'), []


def iat_range(ins):
    # IAT = most-called ds: pointer region (lowest/highest call DWORD PTR ds:X targets)
    xs = [int(HEX.search(o).group(0), 16) for m, o in ins.values()
          if m == 'call' and o.startswith('DWORD PTR ds:') and HEX.search(o)]
    if not xs: return 0, 0
    xs.sort(); med = xs[len(xs) // 2]
    xs = [x for x in xs if abs(x - med) < 0x2000]
    return min(xs), max(xs) + 4


def mangled_args(n):
    # True if mangled says args, False if none, None if unknown
    if re.search(r'@@[QAIUM][AEI]E', n): return True  # thiscall: implicit this
    if n.endswith('XZ') and '@@' in n: return False
    if n.startswith('?') and '@@' in n: return True
    return None


def early_set(p):
    comp, e = set(), set()
    for l in open(p, errors='replace'):
        t, n = l[:1], l[2:].rstrip('\n')
        if t == 'F': comp.add(n)
        elif t == 'X' and n not in comp: e.add(n)
    return e, comp


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('dis'); ap.add_argument('map')
    ap.add_argument('--cmp'); ap.add_argument('--names', nargs='*', default=[])
    ap.add_argument('--ord'); ap.add_argument('--base'); ap.add_argument('-o', required=True)
    a = ap.parse_args()
    ins = load_dis(a.dis); addrs = sorted(ins)
    funcs = {}  # name -> (addr, size or None, source)
    if a.map != '-':
        for l in open(a.map, errors='replace'):
            t = l.split()
            if len(t) >= 4 and t[0].startswith('0001:') and t[3] == 'f':
                funcs.setdefault(t[1], (int(t[2], 16), None, 'map'))
    cmp = json.load(open(a.cmp)) if a.cmp else []
    size = {f['name']: f['size'] for f in cmp}
    if a.map == '-':
        for f in cmp:
            if len(f['hits']) == 1: funcs[f['name']] = (int(f['hits'][0], 16), f['size'], 'cmp')
        have = {v[0] for v in funcs.values()}
        base = {}  # plain name -> our mangled name (unique only), for tsv rows
        for f in cmp:
            b = re.match(r'\?([A-Za-z_]\w*)@@', f['name'])
            if b: base[b.group(1)] = f['name'] if b.group(1) not in base else None
        for p in a.names:  # lower confidence: plain names keyed as-is
            for l in open(p, errors='replace'):
                t = l.rstrip('\n').split('\t')
                if len(t) >= 2:
                    ad = int(t[0], 16)
                    k = base.get(t[1]) or t[1]
                    i = bisect.bisect_left(addrs, ad)  # start must follow padding/ret/jmp
                    if i == 0 or i >= len(addrs) or addrs[i] != ad or \
                            ins[addrs[i - 1]][0] not in ('int3', 'nop', 'ret', 'jmp'): continue
                    if ad not in have and k not in funcs: funcs[k] = (ad, size.get(k), 'tsv'); have.add(ad)
    # extents: next known start (fn starts + direct call targets)
    starts = {v[0] for v in funcs.values()}
    for m, o in ins.values():
        if m == 'call':
            t = target(o)
            if t is not None: starts.add(t)
    starts = sorted(starts)
    def end(ad, sz):
        i = bisect.bisect_right(starts, ad)
        e = starts[i] if i < len(starts) else ad + 0x10000
        return min(e, ad + sz) if sz else e
    rpop = {}
    for ad in starts:
        r = retn(ins, addrs, ad, end(ad, None))
        if r: rpop[ad] = r
    lo, hi = iat_range(ins)
    out, det = {}, {}
    for n, (ad, sz, src) in funcs.items():
        if ad not in ins: out[n] = 'unknown'; continue
        live, stk = analyse(ins, addrs, ad, end(ad, sz), rpop, lo, hi)
        c, lv = classify(n, live, stk)
        if c == 'reg' and mangled_args(n) is False: c = 'unknown'  # live-in on a no-arg fn: bad start
        out[n] = c; det[n] = {'addr': hex(ad), 'live': lv, 'stk': stk, 'src': src}
    json.dump(out, open(a.o, 'w'), indent=0, sort_keys=True)
    json.dump(det, open(a.o + '.detail.json', 'w'), indent=0, sort_keys=True)
    from collections import Counter
    print('classes:', dict(Counter(out.values())), 'by source:', dict(Counter((d['src'], out[n]) for n, d in det.items())))
    if a.ord:
        e, comp = early_set(a.ord)
        called, taken = set(), set()  # direct call targets; addrs used as text operands
        for m, o in ins.values():
            if m == 'call' or m.startswith('j'):
                t = target(o)
                if t is not None: called.add(t)
            else: taken.update(int(x, 16) for x in HEX.findall(o))
        cm = Counter()
        for n in comp:
            if n not in out: continue
            ma = mangled_args(n)
            if ma is not True: continue
            ad = funcs[n][0]
            exp = 'stack' if (n in e or 'ZZ' in n[-3:]) else \
                'late' if ad in called and ad not in taken else 'late-ptr/uncalled'
            cm[(exp, out[n])] += 1
        print('validation (fns with args per mangling, compiled in ord):')
        for k in sorted(cm): print(' expected', k[0], '-> got', k[1], cm[k])
        es = sum(v for (x, g), v in cm.items() if x == 'stack')
        print(' early->stack precision of "reg" (reg among early = false reg):',
              cm[('stack', 'reg')], '/', es)
        rg = sum(v for (x, g), v in cm.items() if g == 'reg')
        print(' "reg" predictions:', rg, 'of which late (correct-ish):', cm[('late', 'reg')],
              'precision', round(cm[('late', 'reg')] / max(rg, 1), 4))
        late = sum(v for (x, g), v in cm.items() if x == 'late')
        print(' late fns with args:', late, 'classified reg:', cm[('late', 'reg')],
              'recall', round(cm[('late', 'reg')] / max(late, 1), 4))
    if a.base:
        b = json.load(open(a.base))
        diff = [(size.get(n, 0), n, b[n], c) for n, c in out.items() if n in b and b[n] != c
                and 'unknown' not in (b[n], c)]
        diff.sort(reverse=True)
        print(len(diff), 'differ from base')
        for sz, n, bc, c in diff[:40]: print(f'{sz:6d} {bc:7s} -> {c:7s} {det[n]["src"]} {n}')


if __name__ == '__main__':
    main()
