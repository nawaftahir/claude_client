"""Exercise the actual lighting helper and check its retail match."""
import json
from pathlib import Path
import re
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/oc/cgame_mp/cg_ents_mp.cpp').read_text()
copy = re.search(r'static __forceinline void EntVectorCopy\([^;\n]+\)\n\{[\s\S]*?\n\}', source).group()
helper = re.search(r'static void CG_LockLightingOrigin\([^;\n]+\)\n\{[\s\S]*?\n\}', source).group()
program = r'''
#include <cassert>
#include <cstring>
#define __forceinline inline
typedef float vec_t;
struct centity_t {
    struct { unsigned eFlags; } nextState;
    float lightingOrigin[3], lerpOrigin[3];
};
struct GfxEntity {
    struct { float origin[3]; } lighting;
    unsigned renderFxFlags;
};
'''
program += copy + '\n' + helper + r'''
int main() {
    for (unsigned flags : {0u, 0x80u, 0x400u, 0x480u}) {
        for (int cached = 0; cached < 8; ++cached) {
            centity_t cent = {};
            GfxEntity ent = {};
            cent.nextState.eFlags = flags;
            ent.renderFxFlags = 0x1234;
            for (int i = 0; i < 3; ++i) {
                cent.lightingOrigin[i] = cached & (1 << i) ? float(i + 4) : 0.0f;
                cent.lerpOrigin[i] = float(i + 1);
                ent.lighting.origin[i] = float(100 + i);
            }
            CG_LockLightingOrigin(&cent, &ent);
            for (int i = 0; i < 3; ++i) {
                if (flags & 0x400) {
                    float expected = cached ? (cached & (1 << i) ? float(i + 4) : 0.0f) : float(i + 1);
                    assert(cent.lightingOrigin[i] == expected);
                    assert(ent.lighting.origin[i] == expected);
                } else {
                    assert(cent.lightingOrigin[i] == 0.0f);
                    assert(ent.lighting.origin[i] == float(100 + i));
                }
            }
            assert(ent.renderFxFlags == ((flags & 0x400) ? (0x1234 | 0x80) : 0x1234));
        }
    }
}
'''
program = '#include <initializer_list>\n' + program
with tempfile.TemporaryDirectory(prefix='cod2_lighting_') as directory:
    binary = str(Path(directory) / 'test')
    subprocess.run(['g++', '-std=c++11', '-O2', '-x', 'c++', '-', '-o', binary],
                   input=program, text=True, check=True)
    subprocess.run([binary], check=True)
rows = json.loads((root / 'target/build/exp.json').read_text())
row = next(row for row in rows if row['name'].startswith('?CG_LockLightingOrigin@@'))
assert row['size'] == 170 and row['hits'] == ['0x4c5cc0'], row
print('PASS: actual helper, 32 lighting/cache/flag fixtures and exact 170-byte retail match')
