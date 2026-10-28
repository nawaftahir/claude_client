# postcopy.py ANCHOR IN OUT RANGES: copy the listed byte ranges from the local retail anchor into a copy of
# our linked exe. Kept separate from the source build: IN stays the pure source result, OUT is the
# owner-approved post-link image. Each range must still hold different bytes or already match.
import hashlib, sys
anchor, src, dst, ranges = sys.argv[1:5]
R = open(anchor, 'rb').read(); O = bytearray(open(src, 'rb').read())
assert len(R) == len(O), 'size differs from retail: post-link copy refused'
n = 0
for l in open(ranges):
    if l.startswith('#') or not l.strip(): continue
    off, ln = int(l.split()[0], 16), int(l.split()[1])
    O[off:off + ln] = R[off:off + ln]; n += ln
open(dst, 'wb').write(O)
same = sum(1 for a, b in zip(R, O) if a == b)
print('copied %d bytes; same %d/%d; sha256 %s' % (n, same, len(R), hashlib.sha256(O).hexdigest()))
print('retail sha256 %s' % hashlib.sha256(R).hexdigest())
