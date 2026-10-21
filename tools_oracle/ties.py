#!/usr/bin/env python3
# ties.py log.txt: colour picks whose pop order was decided by an n3 tie (equal n3 with the previous pop)
# and picks decided by the order table (several free regs at the same lowest cost)
import subprocess, sys, os
out = subprocess.check_output([sys.executable, os.path.join(os.path.dirname(__file__), 'cpick.py'), sys.argv[1]], text=True).splitlines()
prev = None
for l in out:
    if l.startswith('F'): print(l); prev = None; continue
    n3 = int(l.split('n3=')[1].split()[0])
    free = l.split('->')[1].split()[1:]
    costs = [int(f.split(':')[1]) for f in free]
    multi = costs.count(min(costs)) > 1 if costs else False
    tag = []
    if prev and prev[0] == n3: tag.append('N3TIE with #%s' % prev[1].split()[0])
    if multi: tag.append('ORDER')
    if tag: print('%s   [%s]' % (l, ', '.join(tag)))
    prev = (n3, l)
