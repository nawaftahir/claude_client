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
    match = re.match(r'\s*000[13]:\S+\s+(\S+)\s+([0-9a-f]{8})', line)
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
stream = '?SND_StartAliasStreamOnChannel@@YAHPBUsnd_alias_t@@0MHPBMMMHMHEHW4snd_alias_system_t@@@Z'
assert entries[stream]['size'] == 1089 and entries[stream]['hits'] == ['0x4b36f0']
stream_callees = {
    '?Com_DPrintf@@YAXPBDZZ': 0x430590,
    '_sprintf': 0x541c66,
    '?FS_ShortOSFilePath@@YAPBDPBD@Z': 0x421480,
    '?Com_Printf@@YAXPBDZZ': 0x430540,
    '?I_fround@@YAHM@Z': 0x403620,
    '?SND_SetChannelInfo@@YAXHHPBUsnd_alias_t@@0MPBMMMHHHHHEW4snd_alias_system_t@@@Z': 0x44a940,
    '?SND_IsAliasChannel3D@@YAEH@Z': 0x44a910,
    '?MSS_SpatializeStreamImpl@@YAXHPAM0@Z': 0x4b2cb0,
}
actual = direct(ours, symbols[stream], 1089)
expected = direct(retail, 0x4b36f0, 1089)
assert len(actual) == len(expected) == 9
for offset, destination in actual.items():
    callee = next(name for name in stream_callees if symbols[name] == destination)
    assert expected[offset] == stream_callees[callee], (offset, callee)

def stream_imports(image, address, size=1089):
    imports = {imp.address: (dll.dll.lower(), imp.name, imp.ordinal)
               for dll in image.DIRECTORY_ENTRY_IMPORT for imp in dll.imports}
    result = {}
    for ins in decoder.disasm(image.get_data(address - 0x400000, size), address):
        if ins.mnemonic == 'call' and ins.operands[0].type == capstone.CS_OP_MEM:
            result[ins.address - address] = imports[ins.operands[0].mem.disp]
    return result

actual = stream_imports(ours, symbols[stream])
assert len(actual) == 13 and actual == stream_imports(retail, 0x4b36f0)
getter = '?XModelGetSkins@@YAPBUtrXSkin_t@@PBUXModel@@@Z'
assert entries[getter]['size'] == 11 and entries[getter]['hits'] == ['0x48d410']
assert ours.get_data(symbols[getter] - 0x400000, 11) == retail.get_data(0x48d410 - 0x400000, 11)
init_ref = '?CL_InitRef@@YAXXZ'
assert entries[init_ref]['hits'] == ['0x40ead0']

def getter_store(image, address, destination):
    found = []
    for ins in decoder.disasm(image.get_data(address - 0x400000, entries[init_ref]['size']), address):
        if ins.mnemonic == 'mov' and len(ins.operands) == 2:
            left, right = ins.operands
            if (left.type == capstone.x86.X86_OP_MEM and
                    left.mem.base == capstone.x86.X86_REG_ESP and
                    right.type == capstone.x86.X86_OP_IMM and right.imm == destination):
                found.append((ins.address - address, left.mem.disp))
    assert len(found) == 1, found
    return found

assert getter_store(ours, symbols[init_ref], symbols[getter]) == getter_store(retail, 0x40ead0, 0x48d410)
try:
    getter_store(ours, symbols[init_ref], symbols[getter] + 1)
except AssertionError:
    pass
else:
    raise AssertionError('skin callback accepted a wrong target')
surface_getter = '?XModelGetSurfaces@@YAHPBUXModel@@PAPAPAUXSurface_s@@HPAPAH@Z'
assert entries[surface_getter]['size'] == 44 and entries[surface_getter]['hits'] == ['0x48ef00']
assert ours.get_data(symbols[surface_getter] - 0x400000, 44) == retail.get_data(0x48ef00 - 0x400000, 44)
assert getter_store(ours, symbols[init_ref], symbols[surface_getter]) == getter_store(retail, 0x40ead0, 0x48ef00)
try:
    getter_store(ours, symbols[init_ref], symbols[surface_getter] + 1)
except AssertionError:
    pass
else:
    raise AssertionError('surface callback accepted a wrong target')
renderer_base = symbols['?re@@3Urefexport_t@@A']
for prefix, address, slot in [
        ('?XModelReadSurface_48ea60@@', 0x48ea60, 96),
        ('?XModelLoadXSkins_48ea80@@', 0x48ea80, 97),
        ('?XModelReadSurfaces@@', 0x48eaa0, 96)]:
    name = next(name for name in entries if name.startswith(prefix))
    assert hex(address) in entries[name]['hits']
    bindings = []
    for image, start, base in [(ours, symbols[name], renderer_base),
                                (retail, address, 0x60f080)]:
        calls = [ins for ins in decoder.disasm(image.get_data(start - 0x400000, entries[name]['size']), start)
                 if ins.mnemonic == 'call' and ins.operands[0].type == capstone.x86.X86_OP_MEM]
        assert len(calls) == 1
        operand = calls[0].operands[0].mem
        assert operand.base == operand.index == 0 and operand.disp == base + slot * 4
        assert operand.disp != base + (slot + 1) * 4
        bindings.append(calls[0].address - start)
    assert bindings[0] == bindings[1]
loader = '?SND_LoadFromBuffer@@YAPAUMssSound@@PAXPBDPBX@Z'
assert entries[loader]['size'] == 427
actual = stream_imports(ours, symbols[loader], 427)
expected = stream_imports(retail, 0x4b46e0, 427)
assert len(actual) == len(expected) == 3 and list(actual.values()) == list(expected.values())
print('PASS: init/HUD bindings; stream/Miles imports; skin/surface getters and renderer slots 96/97')
