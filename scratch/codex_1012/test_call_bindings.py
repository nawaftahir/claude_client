"""Check four corrected call targets at the same PCs as pinned retail hits."""
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

def direct(image, address, size):
    result = {}
    for ins in decoder.disasm(image.get_data(address - image.OPTIONAL_HEADER.ImageBase, size), address):
        if ins.mnemonic == 'call' and ins.operands[0].type == capstone.x86.X86_OP_IMM:
            result[ins.address - address] = ins.operands[0].imm
    return result

checks = [
    ('?CL_Init@@YAXXZ', 0x410350, ['?CL_ClearState@@YAXXZ']),
    ('?CG_SetNextSnap@@YAXPAUsnapshot_t@@@Z', 0x4d9130, ['?CG_GameMessage@@YAXPBD@Z']),
    ('?DrawSingleHudElem2d@@YAXPBUhudelem_s@@@Z', 0x4d7900,
     ['?DrawHudElemClock@@YAXPBUhudelem_s@@PAUcg_hudelem_t@@@Z',
      '?DrawHudElemMaterial@@YAXPBUhudelem_s@@PAUcg_hudelem_t@@@Z']),
]
for caller, target, callees in checks:
    entry = entries[caller]
    assert hex(target) in entry['hits'], caller
    actual = direct(ours, symbols[caller], entry['size'])
    expected = direct(retail, target, entry['size'])
    for callee in callees:
        hits = entries[callee]['hits']
        assert len(hits) == 1, (callee, hits)
        offsets = [offset for offset, destination in actual.items() if destination == symbols[callee]]
        assert len(offsets) == 1, (caller, callee, offsets)
        assert expected[offsets[0]] == int(hits[0], 16), (caller, callee)

def hud_dispatch(image, address, size):
    instructions = list(decoder.disasm(image.get_data(
        address - image.OPTIONAL_HEADER.ImageBase, size), address))
    remaps = [ins for ins in instructions if ins.mnemonic == 'movzx'
              and ins.operands[1].type == capstone.x86.X86_OP_MEM
              and ins.operands[1].size == 1
              and (ins.operands[1].mem.base or ins.operands[1].mem.index)]
    jumps = [ins for ins in instructions if ins.mnemonic == 'jmp'
             and ins.operands[0].type == capstone.x86.X86_OP_MEM
             and ins.operands[0].mem.scale == 4 and ins.operands[0].mem.index]
    assert len(remaps) == len(jumps) == 1
    # Retail tests type-1 <= 11: the twelve 2D HUD types.
    table = image.get_data(remaps[0].operands[1].mem.disp - image.OPTIONAL_HEADER.ImageBase, 12)
    assert len(table) == 12 and max(table) == 2
    pointer_bytes = image.get_data(jumps[0].operands[0].mem.disp - image.OPTIONAL_HEADER.ImageBase, 12)
    targets = [int.from_bytes(pointer_bytes[k:k + 4], 'little') - address for k in (0, 4, 8)]
    assert all(0 <= offset < size for offset in targets)
    return [targets[index] for index in table]

caller = '?DrawSingleHudElem2d@@YAXPBUhudelem_s@@@Z'
assert hud_dispatch(ours, symbols[caller], entries[caller]['size']) == hud_dispatch(
    retail, 0x4d7900, entries[caller]['size'])
print('PASS: init reset, rename message and both HUD call bindings agree with retail')
