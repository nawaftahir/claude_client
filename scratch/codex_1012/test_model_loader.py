"""Exercise the actual count reader and loader; check the linked retail bindings."""
from pathlib import Path
import json
import re
import resource
import subprocess
import tempfile

import capstone
import pefile

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/xanim/xmodel_load.cpp').read_text()
header = (root / 'target/src/xanim/xanim_public.h').read_text()
reader_start = re.search(r'\b(?:struct|union) XModelFileShort \{', header).start()
reader = header[reader_start:].split('\n}\n', 1)[0] + '\n}\n'
loader = source.split('XModelParts *XModelPartsLoadFile(', 1)[1].split('\nbool XModelLoadConfigFile', 1)[0]
program = r'''
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#define __forceinline inline
#define XMODEL_VERSION 20
using byte = unsigned char;
struct DObjAnimMat { float quat[4], trans[3], transWeight; };
struct XBoneHierarchy { unsigned short *names; byte parentList[1]; };
struct XModelParts {
    short numBones, numRootBones;
    XBoneHierarchy *hierarchy;
    short *quats;
    float *trans;
    byte *partClassification;
    DObjAnimMat skel;
};
struct XModel { int memUsage; };
static XModel *observed;
static std::vector<byte> file;
static std::vector<void *> allocations;
static std::vector<int> sizes, accounting;
static int missing, freed, posed, strings;
static void *allocate(int size) {
    assert(size >= 0);
    accounting.push_back(observed->memUsage);
    sizes.push_back(size);
    void *p = std::calloc(size ? size : 1, 1);
    assert(p); allocations.push_back(p); return p;
}
static int Com_sprintf(char *out, int size, const char *format, ...) {
    va_list args; va_start(args, format);
    int n = std::vsnprintf(out, size, format, args);
    va_end(args); return n < size ? n : -1;
}
static int FS_ReadFile(const char *, void **out) {
    *out = file.data(); return missing ? -1 : int(file.size());
}
static void FS_FreeFile(void *p) { assert(p == file.data()); ++freed; }
static void Com_Printf(const char *, ...) {}
static int I_strlen(const char *s) { return int(std::strlen(s)); }
static unsigned short SL_GetStringOfLen(const char *s, int user, int len, int type) {
    assert(!user && type == 10 && len == 2 && *s == 'a' + strings);
    return ++strings;
}
static void XModelCalcBasePose(XModelParts *) { ++posed; }
static float XModelDataReadFloat(const byte **p) {
    float value; std::memcpy(&value, *p, 4); *p += 4; return value;
}
static void XModelConsumeQuat(const byte **p, short *out) {
    for (int i = 0; i < 3; ++i) { std::memcpy(out+i, *p, 2); *p += 2; }
    out[3] = 32767;
}
'''
program += '\n' + reader
program += '\nXModelParts *XModelPartsLoadFile(' + loader
program += r'''
static void put16(unsigned value) { file.push_back(value); file.push_back(value >> 8); }
static void fixture(int children, int roots, int version = 20) {
    file.clear(); put16(version); put16(children); put16(roots);
    for (int i = 0; i < children; ++i) {
        file.push_back(1);
        for (int k = 0; k < 18; ++k) file.push_back(0);
    }
    for (int i = 0; i < children + roots; ++i) { file.push_back('a' + i); file.push_back(0); }
    for (int i = 0; i < children + roots; ++i) file.push_back(i + 4);
}
static void reset() {
    for (void *p : allocations) std::free(p);
    allocations.clear(); sizes.clear(); accounting.clear();
    missing = freed = posed = strings = 0;
}
int main() {
    for (unsigned bits = 0; bits < 65536; ++bits) for (int offset = 0; offset < 2; ++offset) {
        byte data[4] = {0, 0, 0, 0}; data[offset] = bits; data[offset+1] = bits >> 8;
        const byte *p = data + offset;
        static_assert(sizeof(short) == 2 && sizeof(XModelFileShort) == 2, "word size");
        XModelFileShort value = XModelDataReadShort(&p);
        assert(uint16_t(value.value) == bits && p == data + offset + 2);
        assert(value.value == (bits < 32768 ? int(bits) : int(bits) - 65536));
    }
    for (int children = 0; children < 4; ++children) for (int roots = 0; roots < 4; ++roots) {
        reset(); fixture(children, roots); XModel model = {17}; observed = &model;
        XModelParts *parts = XModelPartsLoadFile(&model, "fixture", allocate);
        assert(parts && parts->numBones == children + roots && parts->numRootBones == roots);
        assert(freed == 1 && posed == 1 && strings == children + roots);
        int total = 17;
        for (unsigned i = 0; i < sizes.size(); ++i) { assert(accounting[i] == total); total += sizes[i]; }
        assert(model.memUsage == total);
        assert(bool(parts->quats) == bool(children) && bool(parts->trans) == bool(children));
        for (int i = 0; i < children + roots; ++i) {
            assert(parts->hierarchy->names[i] == i + 1 && parts->partClassification[i] == i + 4);
        }
        for (int i = 0; i < children; ++i) assert(parts->hierarchy->parentList[i] == byte(roots + i - 1));
    }
    for (int failure = 0; failure < 5; ++failure) {
        reset(); fixture(0, failure == 4 ? 128 : 0, failure == 3 ? 19 : 20);
        if (failure == 1) missing = 1;
        if (failure == 2) file.clear();
        XModel model = {17}; observed = &model;
        char longName[80]; std::memset(longName, 'x', 79); longName[79] = 0;
        assert(!XModelPartsLoadFile(&model, failure ? "fixture" : longName, allocate));
        assert(!posed && freed == int(failure >= 2));
    }
    reset();
}
'''
with tempfile.TemporaryDirectory(prefix='cod2_model_loader_') as directory:
    resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    binary = str(Path(directory) / 'test')
    subprocess.run(['g++', '-std=c++11', '-O2', '-x', 'c++', '-', '-o', binary],
                   input=program, text=True, check=True)
    subprocess.run([binary], check=True)
    order = '\t\tmodelParts->quats = (short *)Alloc(size);\n\t\tmodel->memUsage += size;'
    assert program.count(order) == 1
    old_program = program.replace(order,
        '\t\tmodel->memUsage += size;\n\t\tmodelParts->quats = (short *)Alloc(size);', 1)
    subprocess.run(['g++', '-std=c++11', '-O2', '-x', 'c++', '-', '-o', binary],
                   input=old_program, text=True, check=True)
    assert subprocess.run([binary], stderr=subprocess.PIPE).returncode != 0

entries = {e['name']: e for e in json.loads((root / 'target/build/exp.json').read_text())}
symbols = {}
for line in (root / 'target/build/CoD2MP_s.map').read_text().splitlines():
    match = re.match(r'\s*0001:\S+\s+(\S+)\s+([0-9a-f]{8}) f', line)
    if match:
        symbols[int(match[2], 16)] = match[1]
name = '?XModelPartsLoadFile@@YAPAUXModelParts_s@@PAUXModel@@PBDP6APAXH@Z@Z'
entry = entries[name]
assert entry['hits'] == ['0x48dc70'] and entry['size'] == 826, entry
start = next(address for address, symbol in symbols.items() if symbol == name)
ours = pefile.PE(str(root / 'target/build/CoD2MP_s.exe'))
retail = pefile.PE(str(root.parent / 'cod2engineold/binaries/windows/1.0/CoD2MP_s.exe'))
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True
expected = {ins.address - 0x48dc70: ins for ins in decoder.disasm(
    retail.get_data(0x48dc70 - retail.OPTIONAL_HEADER.ImageBase, 826), 0x48dc70)}
calls = literals = 0
for ins in decoder.disasm(ours.get_data(start - ours.OPTIONAL_HEADER.ImageBase, 826), start):
    target = expected[ins.address - start]
    if ins.mnemonic == 'call' and ins.operands[0].type == capstone.x86.X86_OP_IMM:
        callee = symbols[ins.operands[0].imm]
        if callee == '_SL_GetStringOfLen':
            assert target.operands[0].imm == 0x474560
        else:
            assert hex(target.operands[0].imm) in entries[callee]['hits'], callee
        calls += 1
    for operand, counterpart in zip(ins.operands, target.operands):
        if operand.type == capstone.x86.X86_OP_IMM:
            address = operand.imm
            section = ours.get_section_by_rva(address - ours.OPTIONAL_HEADER.ImageBase)
            if section and section.Name.startswith(b'.rdata'):
                actual = ours.get_data(address - ours.OPTIONAL_HEADER.ImageBase, 256).split(b'\0', 1)[0]
                pinned = retail.get_data(counterpart.imm - retail.OPTIONAL_HEADER.ImageBase, 256).split(b'\0', 1)[0]
                assert actual == pinned and actual, (ins.address, actual, pinned)
                literals += 1
assert calls >= 8 and literals == 6, (calls, literals)
print('PASS: 131072 count inputs, 16 loader fixtures, five failures, allocator accounting, linked calls and strings')
