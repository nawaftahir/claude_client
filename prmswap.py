#!/usr/bin/env python3
"""Param-order lead finder: near-miss fns whose prologue moves incoming eax/ecx/edx into
different registers than retail (LTCG custom conv; fix = swap param order).
usage (in target/): prmswap.py scored.txt [max_diff=200]"""
import os, re, subprocess, sys
A = '../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
MV = re.compile(r'^mov (e[a-d]x|e[sd]i|ebp), (eax|ecx|edx)$')
for l in open(sys.argv[1]):
    r = l.split()
    if len(r) < 4 or not 0 < int(r[0]) <= int(sys.argv[2] if len(sys.argv) > 2 else 200): continue
    o = subprocess.run(['python3', '../fdiff.py', 'build/CoD2MP_s.exe', 'build/CoD2MP_s.map', A, r[3].split('@@')[0] + '@@', r[2], '400'],
                       capture_output=True, text=True).stdout.split('\n')
    ours, ret = {}, {}
    n = 0
    for x in o[3:]:
        if x.startswith('@@'): 
            if n: break
            continue
        n += 1
        if n > 16: break
        s = x[1:].strip()
        m = MV.match(s)
        if m and x[0] in '- ': ours.setdefault(m.group(2), m.group(1))
        if m and x[0] in '+ ': ret.setdefault(m.group(2), m.group(1))
    sw = any(ours.get(a) == ret.get(b) and ours.get(b) == ret.get(a) and ours.get(a) for a in ('eax', 'ecx', 'edx') for b in ('eax', 'ecx', 'edx') if a < b)
    if sw or (os.environ.get('PRM_ANY') and ours and ret and ours != ret):
        print(r[0], r[1], r[2], r[3][:70], 'ours', ours, 'retail', ret)
