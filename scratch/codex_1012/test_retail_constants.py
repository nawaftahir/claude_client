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
color_name = '?CL_LookupColor@@YAXEPAM@Z'
assert entries[color_name]['size'] == 158 and entries[color_name]['hits'] == ['0x403220']
retail_base = next(decoder.disasm(retail.get_data(0x323d, 6), 0x40323d))
actual_base = next(decoder.disasm(ours.get_data(symbols[color_name] - 0x400000 + 0x1d, 6),
                                    symbols[color_name] + 0x1d))
assert retail_base.mnemonic == actual_base.mnemonic == 'add'
assert retail_base.op_str == 'ecx, 0x55bfb8'
assert actual_base.operands[1].type == capstone.CS_OP_IMM
assert ours.get_data(actual_base.operands[1].imm - 0x400000, 128) == retail.get_data(0x15bfb8, 128)
client_clear = next(decoder.disasm(retail.get_data(0xc2c0, 5), 0x40c2c0))
assert client_clear.op_str == 'edi, 0x8dc208'
client_address = int(re.search(r'^\s*0003:\S+\s+\?clients@@\S+\s+([0-9a-f]{8})',
                              (root / 'target/build/CoD2MP_s.map').read_text(), re.M)[1], 16)
for offset, field in [(0x56, 0x863c), (0x5e, 0x8640), (0x67, 0x8644), (0x70, 0x8648),
                      (0x7a, 0x862c), (0x82, 0x8630), (0x8b, 0x8634), (0x94, 0x8638)]:
    left = next(decoder.disasm(ours.get_data(symbols[color_name] - 0x400000 + offset, 6),
                              symbols[color_name] + offset))
    right = next(decoder.disasm(retail.get_data(0x3220 + offset, 6), 0x403220 + offset))
    assert left.mnemonic == right.mnemonic == 'mov'
    assert left.operands[1].mem.disp == client_address + field
    assert right.operands[1].mem.disp == 0x8dc208 + field
client = (root / 'target/src/oc/client_mp/cl_cgame_mp.cpp').read_text()
table = re.search(r'static const vec4_t g_color_table\[8\] = \{(.*?)\n\};', client, re.S)
assert table and len(re.findall(r'\{[^{}]+\}', table[1])) == 8
print('PASS: eight numeric operand checks; 128-byte colour table and eight team-colour fields')
