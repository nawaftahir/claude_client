"""Check screen storage, retail offsets and actual scaling writes with neighbor guards."""
import re
import subprocess
import tempfile
from pathlib import Path

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/oc/client_mp/screen_placement_mp.cpp').read_text()
header = (root / 'target/src/oc/headers/common_types.h').read_text()
struct = 'struct ScreenPlacementGlob {' + header.split('struct ScreenPlacementGlob {', 1)[1].split('};', 1)[0] + '};'
assert 'extern ScreenPlacementGlob spGlob;' in source
assert '#define SP (&spGlob)' in source
assert 'ScreenPlacementGlob spGlob;' in (root / 'target/src/oc/globals.cpp').read_text()

def function(name):
    start = re.search(r'^(?:static )?void ' + name + r'\([^;]+?\)\s*\{', source, re.M).start()
    brace = source.index('{', start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]

fields = [('scaleVirtualToReal', 0), ('scaleVirtualToFull', 8), ('scaleRealToVirtual', 16),
          ('virtualScreenOffsetX', 24), ('virtualViewableMin', 28), ('virtualViewableMax', 36),
          ('realViewportSize', 44), ('realViewableMin', 52), ('realViewableMax', 60), ('subScreenLeft', 68)]
code = '#include <cassert>\n#include <cmath>\n#include <cstddef>\ntypedef float vec2_t[2];\n' + struct
code += '\nstruct Guard { unsigned before; ScreenPlacementGlob value; unsigned after; } guard;\n'
code += '#define spGlob guard.value\n#define SP (&spGlob)\nstruct { struct { int width,height; float aspectRatioPixel; } vidConfig; } cls;\n'
code += function('ScrPlace_CalcSafeAreaOffsets') + '\n' + function('SetScreenScaling')
code += '\nvoid near(float a,float b){assert(fabs(a-b)<0.001f);}\nint main(){\n'
code += 'assert(sizeof(ScreenPlacementGlob)==72);\n'
for name, offset in fields:
    code += f'assert(offsetof(ScreenPlacementGlob,{name})=={offset});\n'
code += '''guard.before=0x12345678; guard.after=0x87654321;
cls.vidConfig.width=640; cls.vidConfig.height=480; cls.vidConfig.aspectRatioPixel=1;
SetScreenScaling(1,1,0,0,640,480);
near(SP->scaleVirtualToReal[0],1); near(SP->scaleVirtualToReal[1],1);
near(SP->realViewableMin[0],0); near(SP->virtualViewableMax[0],640);
near(SP->virtualViewableMax[1],480); near(SP->subScreenLeft,0);
cls.vidConfig.width=1920; cls.vidConfig.height=1080;
SetScreenScaling(0.9f,0.8f,0,0,1920,1080);
near(SP->scaleVirtualToReal[0],2.25f); near(SP->scaleVirtualToFull[0],3);
near(SP->realViewableMin[0],96); near(SP->realViewableMin[1],108);
near(SP->realViewableMax[0],1824); near(SP->realViewableMax[1],972);
near(SP->subScreenLeft,240);
SetScreenScaling(1,1,960,0,960,1080);
near(SP->realViewportSize[0],960); near(SP->realViewableMin[0],0);
near(SP->realViewableMax[0],960); near(SP->subScreenLeft,-240);
assert(guard.before==0x12345678 && guard.after==0x87654321);
}\n'''
with tempfile.TemporaryDirectory(prefix='codex_screen_') as tmp:
    cpp, exe = Path(tmp) / 'check.cpp', Path(tmp) / 'check'
    cpp.write_text(code)
    subprocess.run(['g++', '-O2', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)

retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'), fast_load=True)
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
writes, pointers = set(), set()
for ins in decoder.disasm(retail.get_data(0x449bd0 - 0x400000, 0xfc), 0x449bd0):
    for operand in ins.operands:
        if operand.type == capstone.CS_OP_MEM and operand.mem.base == 0 and 0xb68db0 <= operand.mem.disp < 0xb68df8:
            writes.add(operand.mem.disp - 0xb68db0)
        if operand.type == capstone.CS_OP_IMM and 0xb68db0 <= operand.imm < 0xb68df8:
            pointers.add(operand.imm - 0xb68db0)
assert writes == {0, 4, 8, 12, 16, 20, 24, 44, 48, 68}, writes
assert pointers == {28, 36, 52, 60}, pointers
symbols = (root / 'target/build/CoD2MP_s.map').read_text(errors='replace')
match = re.search(r'^\s*0003:\S+\s+\?spGlob@@3UScreenPlacementGlob@@A\s+([0-9a-f]{8})', symbols, re.M)
assert match, 'linked typed storage missing'
address = int(match[1], 16)
others = [int(m[1], 16) for m in re.findall(r'^\s*0003:\S+\s+(\S+)\s+([0-9a-f]{8})', symbols, re.M) if int(m[1], 16) > address]
assert min(others) - address >= 72, (hex(address), hex(min(others)))
print('PASS: all retail field offsets, 72-byte linked storage, scaling fixtures and neighbor guards')
