"""Exercise the actual loader and declarations in a native 32-bit harness."""
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
header = (root / 'target/src/xanim/xanim_public.h').read_text()
types = header[header.index('union XAnimDynamicIndices'):header.index('struct DObjAnimMat')]
reader_start = re.search(r'\b(?:struct|union) XModelFileShort \{', header).start()
readers = header[reader_start:header.index('inline int XModelDataReadInt')]
source = (root / 'target/src/xanim/xanim_load.cpp').read_text()
source = '\n'.join(line for line in source.splitlines() if not line.startswith('#include'))
program = r'''
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <vector>
#define __forceinline inline __attribute__((always_inline))
typedef unsigned char byte;
typedef float vec3_t[3];
using std::floor;
using std::sqrt;
'''+types+readers+r'''
static std::vector<byte> file;
static std::vector<void *> blocks;
static int freed, missing;
static void *allocate(int size) {
    assert(size > 0);
    void *p = calloc(size, 1); assert(p); blocks.push_back(p); return p;
}
static int Com_sprintf(char *out, int size, const char *format, ...) {
    va_list args; va_start(args, format);
    int n = vsnprintf(out, size, format, args); va_end(args);
    return n < size ? n : -1;
}
static void Com_Printf(const char *, ...) {}
static int FS_ReadFile(const char *, void **out) {
    *out = file.data(); return missing ? -1 : int(file.size());
}
static void FS_FreeFile(void *p) { assert(p == file.data()); ++freed; }
void FS_FreeFileInl(void *p) { FS_FreeFile(p); }
static unsigned short SL_GetString_(const char *s, int user, int type) {
    assert(!user && type == 3 && !strcmp(s, "end")); return 17;
}
static unsigned short SL_GetStringOfLen(const char *s, int user, int len, int type) {
    assert(!user && type == 9 && len == 2 && !strcmp(s, "b")); return 23;
}
static void VectorCopy(const float *a, float *b) { memcpy(b, a, 12); }
'''+source+r'''
static void word(int value) { file.push_back(byte(value)); file.push_back(byte(value >> 8)); }
static void release() { for (void *p : blocks) free(p); blocks.clear(); }
static void fixture(int kind, int count, int flip) {
    file.clear(); freed = missing = 0;
    word(14); word(count ? count : 1); word(kind ? 1 : 0);
    file.push_back(kind ? 0 : 2); word(30);
    if (kind) {
        file.push_back(flip); file.push_back(kind == 1 ? 1 : 0);
        file.push_back('b'); file.push_back(0);
    }
    word(count);
    short expected[3][4] = {};
    int components = kind == 2 ? 4 : 2;
    for (int frame = 0; frame < count; ++frame) {
        int x = (frame & 1) ? -30000 : 30000;
        if (kind == 2) x = (frame & 1) ? -26000 : 26000;
        int y = kind == 2 ? ((frame & 1) ? -9000 : 9000) : 0;
        int z = kind == 2 ? 5000 : 0;
        word(x); if (kind == 2) { word(y); word(z); }
        int remainder = 1073676289 - (x*x + y*y + z*z);
        short w = short(floor(double(0.5f + float(sqrt(double(remainder))))));
        expected[frame][0] = short(x);
        if (kind == 2) {
            expected[frame][1] = short(y); expected[frame][2] = short(z);
        }
        expected[frame][components-1] = w;
        if (kind && flip && !frame)
            for (int axis = 0; axis < components; ++axis) expected[frame][axis] = -expected[frame][axis];
        if (frame) {
            long long dot = 0;
            for (int axis = 0; axis < components; ++axis)
                dot += int(expected[frame][axis]) * int(expected[frame-1][axis]);
            if (dot < 0)
                for (int axis = 0; axis < components; ++axis) expected[frame][axis] = -expected[frame][axis];
        }
    }
    word(0); file.push_back(0);
    XAnimParts *parts = XAnimLoadFile("fixture", allocate);
    assert(parts && freed == 1 && parts->numframes == (count ? count-1 : 0));
    assert(parts->notifyCount == 1 && parts->notify[0].name == 17 && parts->notify[0].time == 1);
    if (kind) assert(parts->names[0] == 23 && parts->boneCount == 1);
    else assert(parts->boneCount == 0 && parts->bDelta && parts->deltaPart);
    short *actual = 0;
    if (!count) {
        assert(kind ? !parts->parts[0].quat : !parts->deltaPart->quat);
    } else if (!kind) {
        auto q = parts->deltaPart->quat;
        assert(q->size == count-1);
        actual = count == 1 ? q->u.frame0 : q->u.frames.frames[0];
    } else {
        auto q = parts->parts[0].quat;
        assert(q->size == count-1);
        actual = count == 1 ? q->u.frame0 : q->u.frames.u.frames[0];
    }
    for (int frame = 0; frame < count; ++frame)
        for (int axis = 0; axis < components; ++axis)
            assert(actual[frame*components+axis] == expected[frame][axis]);
    release();
}
int main() {
    static_assert(sizeof(void *) == 4 && sizeof(XAnimParts) == 44, "target-sized layouts");
    for (int kind = 0; kind < 3; ++kind)
        for (int count = 0; count <= 3; ++count)
            for (int flip = 0; flip <= (kind ? 1 : 0); ++flip) fixture(kind, count, flip);
    file.clear(); freed = 0; missing = 1;
    assert(!XAnimLoadFile("missing", allocate) && !freed);
    missing = 0;
    assert(!XAnimLoadFile("empty", allocate) && freed == 1);
    file.clear(); word(13); freed = 0;
    assert(!XAnimLoadFile("old", allocate) && freed == 1);
    char longname[100]; memset(longname, 'a', 99); longname[99] = 0;
    assert(!XAnimLoadFile(longname, allocate));
}
'''
with tempfile.TemporaryDirectory(prefix='cod2-animation-') as directory:
    binary = str(Path(directory) / 'test')
    subprocess.run(['g++', '-m32', '-std=c++11', '-O2', '-fno-strict-aliasing',
                    '-x', 'c++', '-', '-o', binary], input=program, text=True, check=True)
    subprocess.run([binary], check=True)
print('PASS: actual 32-bit loader/types, 20 quaternion/flip fixtures and four failure paths')
