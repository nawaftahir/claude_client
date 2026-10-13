"""Execute the actual scoreboard decision and camera bounds from source."""
import re
import struct
import subprocess
import tempfile
from pathlib import Path

import pefile
import capstone

root = Path(__file__).resolve().parents[2]
score = (root / 'target/src/oc/cgame_mp/cg_scoreboard_mp.cpp').read_text()
view = (root / 'target/src/oc/cgame_mp/cg_view_mp.cpp').read_text()
score = score.split('void CG_DrawScoreboard_ScoresList(float alpha)\n{', 1)[1]
initial = re.search(r'static int lastLeadTeam = \d+;', score).group()
decision = score[score.index('if (alliesScore < axisScore)'):]
decision = decision[:decision.index('lastLeadTeam = team;') + len('lastLeadTeam = team;')]
view = view.split('static void CG_OffsetThirdPersonView(void)\n{', 1)[1]
bounds = '\n'.join(re.findall(r'static vec3_t (?:mins|maxs) = \{[^}]+\};', view))
assert len(bounds.splitlines()) == 2
retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'), fast_load=True)
for va, fmt, expected in [(0x59b110, '<i', (2,)), (0x59b0f8, '<3f', (4.,4.,4.)),
                          (0x59b104, '<3f', (-4.,-4.,-4.))]:
    assert struct.unpack(fmt, retail.get_data(va - retail.OPTIONAL_HEADER.ImageBase,
                                             struct.calcsize(fmt))) == expected
code = '#include <assert.h>\ntypedef float vec3_t[3];\n'
code += 'int choose(int axisScore, int alliesScore) {\n' + initial
code += '\nint team;\n' + decision + '\nreturn team;\n}\nint main(){\n'
code += bounds + '\n'
code += '''assert(choose(0,0)==2); assert(choose(3,1)==1);
assert(choose(3,3)==1); assert(choose(1,3)==2); assert(choose(3,3)==2);
assert(choose(-1,-2)==1); assert(choose(-2,-1)==2);
for(int i=0;i<3;i++){assert(mins[i]==-4); assert(maxs[i]==4);}
}\n'''
with tempfile.TemporaryDirectory(prefix='codex_client_statics_') as tmp:
    source = Path(tmp) / 'test.cpp'
    binary = Path(tmp) / 'test'
    source.write_text(code)
    subprocess.run(['g++', '-O2', str(source), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
image = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'), fast_load=True)
symbols = (root / 'target/build/CoD2MP_s.map').read_text(errors='replace')
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
found = []
for name, size, patterns in [('CG_DrawScoreboard_ScoresList', 623, [struct.pack('<i', 2)]),
                             ('CG_OffsetThirdPersonView', 922,
                              [struct.pack('<3f', 4,4,4), struct.pack('<3f', -4,-4,-4)])]:
    match = re.search(r'^\s*0001:\S+\s+\?' + name + r'@@\S*\s+([0-9a-f]{8}) f',
                      symbols, re.M)
    assert match, name
    address = int(match[1], 16)
    base = image.OPTIONAL_HEADER.ImageBase
    refs = set()
    for instruction in decoder.disasm(image.get_data(address - base, size), address):
        for operand in instruction.operands:
            if operand.type == capstone.CS_OP_IMM:
                refs.add(operand.imm)
            elif operand.type == capstone.CS_OP_MEM and not operand.mem.base and not operand.mem.index:
                refs.add(operand.mem.disp)
    for pattern in patterns:
        addresses = [ref for ref in refs if base <= ref < base + image.OPTIONAL_HEADER.SizeOfImage
                     and image.get_data(ref - base, len(pattern)) == pattern]
        assert len(addresses) == 1, (name, addresses)
        found += addresses
assert found[2] == found[1] + 12 and found[0] == found[2] + 12, found
print('PASS: actual scoreboard decisions, camera bounds and linked initialized statics')
