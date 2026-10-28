# postlink_ranges.py RETAIL OURS MAP > postlink_ranges.txt
# Lists every byte range where our linked exe still differs from retail, with the owning symbol.
# The ranges are copied from the local retail anchor after link (postlink.pl); retail bytes are never stored.
import re, sys, bisect
retail, ours, mp = sys.argv[1:4]
R = open(retail, 'rb').read(); O = open(ours, 'rb').read()
assert len(R) == len(O), 'size differs: fix layout in source first'
syms = []
for l in open(mp, errors='ignore'):
    m = re.match(r'\s*000[0-9]:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s', l)
    if m: syms.append((int(m.group(2), 16) - 0x400000, m.group(1)))
syms.sort(); addr = [a for a, _ in syms]
def owner(off):
    if off < 0x1000: return 'header'
    i = bisect.bisect_right(addr, off) - 1
    return syms[i][1] if i >= 0 else '?'
i, n = 0, len(R)
print('# file_offset length owner  (generated; source-reconstructed bytes are everything NOT listed here)')
while i < n:
    if R[i] == O[i]: i += 1; continue
    j = i
    while j < n and (R[j] != O[j] or (j + 1 < n and R[j + 1] != O[j + 1])): j += 1  # merge 1-byte gaps
    print('%08x %d %s' % (i, j - i, owner(i)))
    i = j
