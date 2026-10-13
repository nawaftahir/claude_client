"""Test the actual smoothing loop and its retail-pinned numeric operands."""
from pathlib import Path
import json
import re
import struct
import subprocess
import tempfile

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/oc/cgame_mp/cg_weapons.cpp').read_text()
function = source.split('void CalculateWeaponPosition_BasePosition_movement(float *origin)', 1)[1]
loop = function.split('    for (i = 0; i < 3; i++)', 1)[1].split(
    '    if (cg->predictedPlayerState.fWeaponPosFrac', 1)[0]
program = r'''
#include <cassert>
#include <cstdint>
#include <cstring>
struct State {
    struct { float vLastMoveOrg[3]; } playerEntity;
    struct { float viewHeightCurrent; } predictedPlayerState;
    int frametime;
};
struct Weapon { float fPosProneMoveRate, fPosMoveRate; };
struct Dvar { struct { float value; } current; };
static State state, *cg = &state;
static Weapon weapon, *weapDef = &weapon;
static Dvar rate, *cg_gun_move_rate = &rate;
void smooth(float *targetPos) {
    int i; float moveRate, delta;
    for (i = 0; i < 3; i++)
'''
program += loop + r'''
}
int main() {
    uint32_t word = 0x38d1b718; float pinned;
    std::memcpy(&pinned, &word, 4);
    for (int frame : {0, 1, 16, 33, 100}) for (int prone : {0, 1}) {
        state = {}; state.frametime = frame;
        state.predictedPlayerState.viewHeightCurrent = prone ? 11.0f : 40.0f;
        state.playerEntity.vLastMoveOrg[1] = 1.0f;
        state.playerEntity.vLastMoveOrg[2] = 0.5f;
        float target[3] = {1.0f, 0.0f, 0.5f};
        smooth(target);
        float step = float(frame) * pinned;
        assert(state.playerEntity.vLastMoveOrg[0] == step);
        assert(state.playerEntity.vLastMoveOrg[1] == 1.0f - step);
        assert(state.playerEntity.vLastMoveOrg[2] == 0.5f);
        state.playerEntity.vLastMoveOrg[0] = 0.0f;
        state.playerEntity.vLastMoveOrg[1] = 0.0f;
        target[0] = 1e-8f; target[1] = -1e-8f;
        smooth(target);
        assert(state.playerEntity.vLastMoveOrg[0] == (frame ? target[0] : 0.0f));
        assert(state.playerEntity.vLastMoveOrg[1] == (frame ? target[1] : 0.0f));
    }
}
'''
program = '#include <initializer_list>\n' + program
with tempfile.TemporaryDirectory(prefix='cod2_weapon_move_') as directory:
    binary = str(Path(directory) / 'test')
    subprocess.run(['g++', '-std=c++11', '-O2', '-x', 'c++', '-', '-o', binary],
                   input=program, text=True, check=True)
    subprocess.run([binary], check=True)
retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'))
for address, word in ((0x583350, 0x38d1b718), (0x58334c, 0xb8d1b718)):
    assert retail.get_data(address - retail.OPTIONAL_HEADER.ImageBase, 4) == struct.pack('<I', word)
ours = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
line = next(line for line in (root / 'target/build/CoD2MP_s.map').read_text().splitlines()
            if '?CalculateWeaponPosition_BasePosition_movement@@' in line)
start = int(re.search(r'\s([0-9a-f]{8}) f', line)[1], 16)
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
words = []
entry = next(entry for entry in json.loads((root / 'target/build/exp.json').read_text())
             if entry['name'].startswith('?CalculateWeaponPosition_BasePosition_movement@@'))
for ins in decoder.disasm(ours.get_data(start - ours.OPTIONAL_HEADER.ImageBase, entry['size']), start):
    for operand in ins.operands:
        if ins.mnemonic == 'fmul' and operand.type == capstone.x86.X86_OP_MEM and operand.size == 4:
            if not operand.mem.base and not operand.mem.index:
                words.append(ours.get_data(operand.mem.disp - ours.OPTIONAL_HEADER.ImageBase, 4))
assert struct.pack('<I', 0x38d1b718) in words and struct.pack('<I', 0xb8d1b718) in words
assert struct.pack('<I', 0x38d1b717) not in words and struct.pack('<I', 0xb8d1b717) not in words
print('PASS: actual smoothing loop, positive/negative steps, clamping, prone/frame cases and linked retail constants')
