"""Verify corrected numeric operands in fresh linked code and its retail hits."""
import json
import re
from pathlib import Path

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
ours = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'))
entries = {e['name']: e for e in json.loads((root / 'target/build/exp.json').read_text())}
symbols = {}
for line in (root / 'target/build/CoD2MP_s.map').read_text().splitlines():
    match = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if match:
        symbols[match[1]] = int(match[2], 16)
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True

def reads(image, address, size, value):
    result = 0
    base = image.OPTIONAL_HEADER.ImageBase
    for ins in decoder.disasm(image.get_data(address - base, size), address):
        for operand in ins.operands:
            if operand.type != capstone.x86.X86_OP_MEM or operand.size != len(value):
                continue
            if operand.mem.base or operand.mem.index or operand.mem.disp < base:
                continue
            if image.get_data(operand.mem.disp - base, len(value)) == value:
                result += 1
    return result

checks = [
    ('?Com_PickSoundAliasFromList@@', '00000047', 2),
    ('?RotationToYaw@@', 'e02e6542', 1),
    ('?GScr_asin@@', 'e02e6542', 1),
    ('?GScr_acos@@', 'e02e6542', 1),
    ('?GScr_atan@@', 'e02e6542', 1),
    ('?turret_behind@@', 'e02e6542', 1),
    ('?AdjustCamera@FxHelper@@', '000000601857e63f', 1),
    ('?CG_DrawScoreboard_ListColumnHeaders@@', 'f5283c41', 1),
]
for prefix, encoded, count in checks:
    names = [n for n in entries if n.startswith(prefix)]
    assert len(names) == 1, names
    name = names[0]
    entry, value = entries[name], bytes.fromhex(encoded)
    assert entry['hits'], name
    assert reads(ours, symbols[name], entry['size'], value) == count, name
    for hit in entry['hits']:
        assert reads(retail, int(hit, 16), entry['size'], value) == count, (name, hit)
print('PASS: eight linked functions use the pinned retail numeric operands')
