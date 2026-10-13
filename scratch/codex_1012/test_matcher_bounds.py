"""Reject linker placeholder functions outside the real code section."""
from pathlib import Path
import json
import re

import pefile

root = Path(__file__).resolve().parents[2]
image = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
section = next(s for s in image.sections if s.Name.startswith(b'.text'))
lower = image.OPTIONAL_HEADER.ImageBase + section.VirtualAddress
upper = lower + len(section.get_data())
valid, invalid = set(), set()
for line in (root / 'target/build/CoD2MP_s.map').read_text().splitlines():
    match = re.match(r'\s*0001:[0-9a-f]+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if match:
        key = (match[1], line.split()[-1])
        (valid if lower <= int(match[2], 16) < upper else invalid).add(key)
assert len(invalid) >= 3
entries = json.loads((root / 'target/build/exp.json').read_text())
for entry in entries:
    assert (entry['name'], entry['obj']) in valid, entry
legacy = json.loads(Path(__file__).with_name('matcher_bounds_legacy.json').read_text())
bad_legacy = [entry for entry in legacy
              if (entry['name'], entry['obj']) in invalid - valid]
assert len(bad_legacy) == 3, bad_legacy
assert not any((entry['name'], entry['obj']) in invalid - valid for entry in entries)
name = '?XModelPartsLoadFile@@YAPAUXModelParts_s@@PAUXModel@@PBDP6APAXH@Z@Z'
entry = next(entry for entry in entries if entry['name'] == name)
assert entry['hits'] == ['0x48dc70'] and entry['size'] == 826
print('PASS: three out-of-text linker entries rejected; genuine 826-byte loader match preserved')
