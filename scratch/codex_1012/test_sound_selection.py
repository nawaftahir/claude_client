"""Exercise the actual alias selector and reject the old negative scale."""
import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = (root / 'target/src/universal/com_sndalias.cpp').read_text()
signature = 'snd_alias_t *Com_PickSoundAliasFromList( snd_alias_list_t *aliasList )\n{'
assert source.count(signature) == 1
start = source.index(signature)
end = source.index('\n}', start) + 2
selector = source[start:end]
prefix = '''#include <cstddef>
#include <cstdint>
struct snd_alias_t { float probability; int sequence; };
struct snd_alias_list_t { snd_alias_t *head; int count; };
static uint32_t seed;
static int Com_SoundAliasRandom() {
    seed = seed * 214013u + 2531011u;
    return (seed >> 16) & 0x7fff;
}
'''
fixture = '''
int main() {
    if (Com_PickSoundAliasFromList(NULL) != NULL) return 1;
    snd_alias_t a[2] = {{1, 0}, {1, 0}};
    snd_alias_list_t list = {a, 2};
    seed = 1;
    if (Com_PickSoundAliasFromList(&list) != a + 1) return 2;
    if (a[1].sequence != 1) return 3;
    a[0].sequence = a[1].sequence = 0;
    seed = 6000;
    if (Com_PickSoundAliasFromList(&list) != a) return 4;
    list.count = 1;
    if (Com_PickSoundAliasFromList(&list) != a) return 5;
    snd_alias_t b[3] = {{1, 4}, {0, 0}, {0, 0}};
    snd_alias_list_t three = {b, 3};
    seed = 1;
    if (Com_PickSoundAliasFromList(&three) != b) return 6;
    if (b[0].sequence != 5) return 7;
    snd_alias_t c[3] = {{0, 4}, {1, 0}, {0, 0}};
    three.head = c;
    seed = 1;
    if (Com_PickSoundAliasFromList(&three) != c + 1) return 8;
    if (c[1].sequence != 5) return 9;
    return 0;
}
'''
assert selector.count('32768.0f') == 2
negative = selector.replace('32768.0f', '-2147483648.0f')
with tempfile.TemporaryDirectory(prefix='codex_alias_') as tmp:
    for label, body, expected in [('fixed', selector, 0), ('negative', negative, 2)]:
        exe = str(Path(tmp) / label)
        subprocess.run(['g++', '-O2', '-x', 'c++', '-', '-o', exe],
                       input=prefix + body + fixture, text=True, check=True)
        actual = subprocess.run([exe]).returncode
        assert actual == expected, (label, actual, expected)
print('PASS: actual selector, both choices, null/single inputs; old scale rejected')
