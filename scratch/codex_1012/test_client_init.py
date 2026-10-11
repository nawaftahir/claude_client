"""Run actual init/state-clear bodies; reject the old skeleton-only init."""
import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/oc/client_mp/cl_main_mp.cpp').read_text()

def function(name):
    signature = 'void ' + name + '(void)\n{'
    assert source.count(signature) == 1
    start = source.index(signature)
    return source[start:source.index('\n}', start) + 2]

clear, init = function('CL_ClearState'), function('CL_Init')
assert init.count('CL_ClearState();') == 1
prefix = '''#include <cstring>
typedef unsigned char byte;
typedef unsigned char Bool;
struct clientActive_t {
    Bool cgameInitialized, cgameInitCalled, displayHUDWithKeycatchUI, active;
    int keyCatchers, voicePacketCount, payload;
};
clientActive_t clients[1];
struct {int state;} clc;
struct {int realtime;} cls;
struct LegacyHacks {byte data[256];} hacks;
LegacyHacks *legacyHacks = &hacks;
enum {CA_DISCONNECTED = 1};
int skeletonClears, mutedClears, executed, ads;
void Com_Printf(const char*) {}
void Com_ClientDObjClearAllSkel() {skeletonClears++;}
void CL_ClearMutedList() {mutedClears++;}
void Cbuf_Execute() {executed++;}
void CL_SetADS(int value) {ads=value;}
'''
fixture = '''
int main() {
    clients[0] = {1,1,1,1,7,17,99};
    memset(&hacks, 0xff, sizeof(hacks));
    clc.state = 8; cls.realtime = 123; ads = 1;
    CL_Init();
    if (clients[0].payload) return 1;
    if (clients[0].voicePacketCount || !clients[0].active) return 2;
    if (!clients[0].cgameInitialized || !clients[0].cgameInitCalled) return 3;
    if (!clients[0].displayHUDWithKeycatchUI || clients[0].keyCatchers != 7) return 4;
    if (clc.state != CA_DISCONNECTED || cls.realtime) return 5;
    if (skeletonClears != 1 || mutedClears != 1 || executed != 1 || ads) return 6;
    if (hacks.data[0x5c] || hacks.data[0x9c] || hacks.data[0xdc]) return 7;
    int active; memcpy(&active, hacks.data + 4, sizeof(active));
    if (active != 1) return 8;
    return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='codex_init_') as tmp:
    old = init.replace('CL_ClearState();', 'Com_ClientDObjClearAllSkel();')
    for label, body, expected in [('fixed', init, 0), ('old', old, 1)]:
        exe = str(Path(tmp) / label)
        subprocess.run(['g++', '-O2', '-x', 'c++', '-', '-o', exe],
                       input=prefix + clear + body + fixture, text=True, check=True)
        result = subprocess.run([exe]).returncode
        assert result == expected, (label, result, expected)
print('PASS: actual init clears state, preserves required flags; old binding rejected')
