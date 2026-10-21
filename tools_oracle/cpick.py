#!/usr/bin/env python3
# cpick.py log.txt: cave_rd.s colour choices in pop order: node, priority, free regs with their cost, pick
import sys
R = {1: 'eax', 2: 'ecx', 3: 'edx', 4: 'ebx', 6: 'ebp', 7: 'esi', 8: 'edi'}
ORDER = [1, 2, 3, 7, 8, 4, 6]  # class-0 try order, table 0x108691a0
s = lambda v: v - (1 << 32) if v & 0x80000000 else v
last = {}
k = 0
for line in open(sys.argv[1], errors='replace'):
    w = line.split()
    if not w: continue
    if w[0] == 'F': print(line.strip()); continue
    nm = line.split(' = ')[1].strip() if ' = ' in line else ''
    if w[0] == 'P':
        n = [int(x, 16) for x in w[1:19]]
        o = [int(x, 16) for x in w[20:34]] if len(w) > 33 else [0] * 14
        last[n[7]] = (s(n[3]), s(n[16]), n[17], nm or ('temp' if o[1] & 0xff == 0x0d else 'kind%02x val=%d' % (o[1] & 0xff, s(o[7]))))
        continue
    if w[0] != 'C': continue
    v = [int(x, 16) for x in w[1:13]]
    nid, pick, best, mask, pref = v[0], v[1], s(v[2]), v[3], [s(x) for x in v[4:12]]
    k += 1
    n3, n16, n17, desc = last.get(nid, (0, 0, 0, nm))
    free = ' '.join('%s%s:%d' % ('*' if r == pick else '', R[r], pref[r - 1]) for r in ORDER if mask >> r & 1)
    print('%3d id=%3d n3=%5d n16=%4d n17=%4d %-28s -> %-4s  %s' % (k, nid, n3, n16, n17, (nm or desc)[:28], R.get(pick, str(pick)), free))
