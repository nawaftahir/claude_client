"""Check the full save/load name table against retail and the linked source table."""
import re
import struct
from pathlib import Path

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'), fast_load=True)
image = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'), fast_load=True)

def names(pe, address):
    base = pe.OPTIONAL_HEADER.ImageBase
    pointers = struct.unpack('<29I', pe.get_data(address - base, 29 * 4))
    return [pe.get_string_at_rva(pointer - base).decode('ascii') for pointer in pointers]

expected = names(retail, 0x596310)
source = (root / 'target/src/oc/cgame_mp/cg_shellshock.cpp').read_text()
body = source.split('static const char *cg_shock_dvar_names[29] = {', 1)[1].split('};', 1)[0]
assert re.findall(r'"([^"]+)"', body) == expected
symbols = (root / 'target/build/CoD2MP_s.map').read_text(errors='replace')
match = re.search(r'^\s*0001:\S+\s+\?CG_SaveShellShockDvars@@\S*\s+([0-9a-f]{8}) f', symbols, re.M)
assert match
address = int(match[1], 16)
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
refs = []
base = image.OPTIONAL_HEADER.ImageBase
for instruction in decoder.disasm(image.get_data(address - base, 208), address):
    for operand in instruction.operands:
        if operand.type == capstone.CS_OP_IMM and base <= operand.imm < base + image.OPTIONAL_HEADER.SizeOfImage:
            try:
                if names(image, operand.imm) == expected:
                    refs.append(operand.imm)
            except (UnicodeError, struct.error, pefile.PEFormatError):
                pass
assert len(set(refs)) == 1, refs
print('PASS: all 29 actual source and linked save/load names in retail order')
