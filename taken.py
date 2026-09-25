#!/usr/bin/env python3
"""Address-taken functions (a relocated pointer to a function start), client vs our build, via names_1.0.tsv."""
import re, struct, sys
import pefile

def taken(path, starts):
    # the client has no relocations: any 4-byte occurrence of a function start counts
    d = open(path, 'rb').read()
    return {a for a in starts if d.find(struct.pack('<I', a)) >= 0}

if __name__ == '__main__':
    img, mapf, target, names = sys.argv[1:5]
    cname = {int(l.split('\t')[0], 16): l.split('\t')[1] for l in open(names)}
    ours = {}
    for l in open(mapf, errors='replace'):
        m = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', l)
        if m:
            g = re.match(r'\?(\w+)@(\w+)?@', m.group(1))
            ours[int(m.group(2), 16)] = (g.group(2) + '::' + g.group(1) if g and g.group(2) else g.group(1) if g else m.group(1).lstrip('_'))
    ct = {cname[a] for a in taken(target, set(cname))}
    ot = {ours[a] for a in taken(img, set(ours))}
    known = set(cname.values())
    print('client-taken', len(ct), 'ours-taken', len(ot))
    print('TAKEN IN CLIENT ONLY:', ' '.join(sorted(ct - ot)))
    print('TAKEN IN OURS ONLY (named in client):', ' '.join(sorted((ot - ct) & known)))
