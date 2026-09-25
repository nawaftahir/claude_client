#!/usr/bin/env python3
"""Replace draft (oc/) declarations of aliased functions with the engine's own one-line prototype.
usage (from target/src): ocdecl.py [name...]   (default: every aliased name with a one-line decl)"""
import collections, glob, re, sys
al = [l.split('alternatename:')[1].split('"')[0] for l in open('oc/alias.cpp') if 'alternatename' in l]
names = {re.match(r'\?(\w+)@', a.split('=')[0]).group(1) for a in al}
found = {}
for p in glob.glob('**/*.[ch]*', recursive=True):
    if p.startswith('oc/'):
        continue
    for l in open(p, errors='replace'):
        m = re.match(r'^(?:extern\s+)?(?:"C"\s+)?([\w][\w\s\*]*?\b(\w+)\s*\([^;{]*\))\s*;', l)
        if m and m.group(2) in names and m.group(2) not in found:
            found[m.group(2)] = m.group(1)
# the draft's call sites need these as they are (enum/int args, const-ness, other return types)
SKIP = {'BG_GetEmptySlotForWeapon', 'BG_Player_DoControllers', 'Com_Error', 'Com_PrintMessage', 'Com_UnloadSoundAliases',
        'DObjCalcSkel', 'DObjCreateSkel', 'DObjSkelExists', 'Dvar_SetBool', 'Dvar_SetInt', 'Dvar_SetString',
        'FS_FOpenFileByMode', 'FS_FreeFileList', 'FS_GetFileList', 'FS_ListFiles', 'FX_GetEffectLength', 'FX_RegisterEffect',
        'Mantle_CreateAnims', 'MatrixMultiply', 'PC_ReadTokenHandle', 'PM_UpdateViewAngles', 'ReplaceStringInternal',
        'SEH_GetLanguageIndexForName', 'XAnimCreateAnims', 'XAnimFreeTree', 'XAnimPrecache', 'XAnimSetCompleteGoalWeightKnobAll',
        'YawToAxis', 'MSG_ReadDeltaEntity', 'MSG_ReadDeltaClient'}
want = set(sys.argv[1:]) or set(found) - SKIP
n = collections.Counter()
for p in glob.glob('oc/**/*.[ch]*', recursive=True):
    s = open(p, errors='replace').read()
    def sub(m):
        name = m.group(3)
        if name not in want or name not in found:
            return m.group(0)
        n[name] += 1
        return '%s%s;' % (m.group(1), found[name])
    # one line only, and never a statement such as 'return f(x);'
    t = re.sub(r'(?m)^([ \t]*(?:extern[ \t]+)?)((?!return\b|else\b|case\b|goto\b)(?:const[ \t]+)?[\w][\w \t\*]*?\b(\w+)[ \t]*\([^;{()\n]*(?:\([^)\n]*\)[^;{()\n]*)*\))[ \t]*;', sub, s)
    if t != s:
        open(p, 'w').write(t)
print(len(n), 'names,', sum(n.values()), 'declarations')
