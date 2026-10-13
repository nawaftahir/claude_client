"""Compare literal loads at corresponding PCs of uniquely matched functions.

This is a diagnostic, not a coverage gate: arithmetic can commute a literal
and a variable between different instructions without changing masked bytes.
"""
import json
import re
from pathlib import Path

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
original = root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'
ours = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
retail = pefile.PE(str(original))
assert ours.OPTIONAL_HEADER.ImageBase == retail.OPTIONAL_HEADER.ImageBase
base = ours.OPTIONAL_HEADER.ImageBase
oi, ri = ours.get_memory_mapped_image(), retail.get_memory_mapped_image()
functions, literals = {}, {}
for line in (root / 'target/build/CoD2MP_s.map').read_text().splitlines():
    match = re.match(r'\s*000[123]:\S+\s+(\S+)\s+([0-9a-f]{8})', line)
    if not match:
        continue
    name, address = match[1], int(match[2], 16)
    if ' f ' in line:
        functions[name] = address
    if name.startswith('__real@'):
        literals[address] = name

decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
checked, differences = 0, []
for entry in json.loads((root / 'target/build/exp.json').read_text()):
    if len(entry['hits']) != 1 or entry['name'] not in functions:
        continue
    address, target = functions[entry['name']], int(entry['hits'][0], 16)
    size = entry['size']
    target_insns = {i.address - target: i for i in decoder.disasm(
        ri[target - base:target - base + size], target)}
    for ins in decoder.disasm(oi[address - base:address - base + size], address):
        other = target_insns.get(ins.address - address)
        for index, operand in enumerate(ins.operands):
            if operand.type != capstone.x86.X86_OP_MEM or operand.mem.disp not in literals:
                continue
            assert other and ins.mnemonic == other.mnemonic, (
                entry['name'], hex(ins.address), ins.mnemonic,
                None if other is None else (hex(other.address), other.mnemonic))
            counterpart = other.operands[index]
            assert counterpart.type == capstone.x86.X86_OP_MEM
            assert counterpart.size == operand.size
            first, second, width = operand.mem.disp, counterpart.mem.disp, operand.size
            actual = oi[first - base:first - base + width]
            expected = ri[second - base:second - base + width]
            assert len(actual) == len(expected) == width
            checked += 1
            if actual != expected:
                differences.append(dict(name=entry['name'], pc=hex(ins.address),
                    retail_pc=hex(other.address), literal=literals[first],
                    retail_data=hex(second), actual=actual.hex(), expected=expected.hex()))
print(json.dumps(dict(checked=checked, differences=differences), indent=2))
