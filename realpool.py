#!/usr/bin/env python3
"""Our __real constant pool vs the client's, as a 4-byte-word alignment. run in target/ after a build.
usage: realpool.py [client_start client_end]  -> summary + first differing runs"""
import difflib, re, struct, sys
import pefile
ours = pefile.PE('build/CoD2MP_s.exe'); cli = pefile.PE('../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe')
def words(pe, a, b):
    o = pe.get_offset_from_rva(a - pe.OPTIONAL_HEADER.ImageBase)
    return list(struct.unpack('<%dI' % ((b - a) // 4), pe.__data__[o:o + b - a]))
reals = sorted((int(m.group(2), 16), m.group(1), m.group(3)) for m in
               (re.match(r'\s*0002:\S+\s+(__real@(\w+))\s+([0-9a-f]{8})', l) for l in open('build/CoD2MP_s.map')) if m)
reals = sorted((int(a, 16), n, v) for n, v, a in [(r[1], r[2], r[0]) for r in []]) or \
        sorted((int(m.group(3), 16), m.group(1)) for m in
               (re.match(r'\s*0002:\S+\s+(__real@(\w+))\s+([0-9a-f]{8})', l) for l in open('build/CoD2MP_s.map')) if m)
pool = [r for r in reals if r[0] > 0x570000]
a0, a1 = pool[0][0], pool[-1][0] + 8
A = words(ours, a0, a1)
c0, c1 = (int(sys.argv[1], 16), int(sys.argv[2], 16)) if len(sys.argv) > 2 else (0x582e00, 0x583900)
B = words(cli, c0, c1)
sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
same = sum(b.size for b in sm.get_matching_blocks())
print('ours %#x-%#x (%d words, %d consts), client %#x-%#x: %d words aligned' % (a0, a1, len(A), len(pool), c0, c1, same))
for op, i1, i2, j1, j2 in sm.get_opcodes():
    if op != 'equal':
        print('%-7s ours %#x %s | client %#x %s' % (op, a0 + 4 * i1, ' '.join('%08x' % w for w in A[i1:i2][:6]), c0 + 4 * j1, ' '.join('%08x' % w for w in B[j1:j2][:6])))
