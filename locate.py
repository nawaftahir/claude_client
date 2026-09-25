#!/usr/bin/env python3
"""Client address of an unmatched function, from its matched neighbours in the same object (cmp.py result)."""
import json, sys

def locate(cmpfile, name):
    r = json.load(open(cmpfile))
    i = next(k for k, x in enumerate(r) if name in x['name'])
    obj = r[i]['obj']
    j = i - 1
    while j >= 0 and r[j]['obj'] == obj and len(r[j]['hits']) != 1:
        j -= 1
    if j >= 0 and r[j]['obj'] == obj:
        a = int(r[j]['hits'][0], 16) + r[j]['size']
        for x in r[j + 1:i]:
            a += x['size']  # assume unmatched predecessors keep their size
        return (a + 15) & ~15
    k = i + 1
    while k < len(r) and r[k]['obj'] == obj and len(r[k]['hits']) != 1:
        k += 1
    if k < len(r) and r[k]['obj'] == obj:
        a = int(r[k]['hits'][0], 16)
        for x in reversed(r[i:k]):
            a -= (x['size'] + 15) & ~15
        return a
    return None

if __name__ == '__main__':
    a = locate(sys.argv[1], sys.argv[2])
    print(hex(a) if a else 'unplaced')
