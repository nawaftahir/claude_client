#!/usr/bin/env python3
"""Replace draft imp_ data indirections with direct globals (run from target/src)."""
import re, glob
SUB = [
    (r'\(clientConnection_t \*\)\*\(void \*\*\)imp_clc', 'clientConnections'),
    (r'\*\(clientActive_t \*\*\)imp_cl\b', 'clients'),
    (r'\*\(void \*\*\)imp_cl\b', '((void *)clients)'),
    (r'\*\(clientConnection_t \*\*\)imp_clc\b', 'clientConnections'),
    (r'\*\(void \*\*\)imp_clc\b', '((void *)clientConnections)'),
    (r'\*\(int \*\*\)imp_clc\b', '((int *)clientConnections)'),
    (r'\(displayContextDef_t \*\)imp_cgDC\b', '(&cgDC)'),
    (r'\(void \*\)imp_cgDC\b', '((void *)&cgDC)'),
    (r'\*\(snd_local_t \*\*\)imp_g_snd\b', '(&g_snd)'),
    (r'\(clientStatic_t \*\)imp_cls\b', '(&cls)'),
    (r'\(byte \*\)imp_cls\b', '((byte *)&cls)'),
    (r'\*\(int \*\)imp_dvar_modifiedFlags\b', 'dvar_modifiedFlags'),
    (r'\(int \*\)imp_dvar_modifiedFlags\b', '(&dvar_modifiedFlags)'),
    (r'\*\(int \*\)imp_g_console_field_width\b', 'g_console_field_width'),
    (r'\*\(float \*\)imp_g_console_char_height\b', 'g_console_char_height'),
    (r'\(field_t \*\)imp_g_consoleField\b', '(&g_consoleField)'),
]
DECL = {
    'clients': 'extern clientActive_t clients[1];',
    'clientConnections': 'extern clientConnection_t clientConnections[1];',
    'cgDC': 'extern displayContextDef_t cgDC;',
    'g_snd': 'extern snd_local_t g_snd;',
    'cls': 'extern clientStatic_t cls;',
    'dvar_modifiedFlags': 'extern int dvar_modifiedFlags;',
    'g_console_field_width': 'extern int g_console_field_width;',
    'g_console_char_height': 'extern float g_console_char_height;',
    'g_consoleField': 'extern field_t g_consoleField;',
}
for f in glob.glob('oc/**/*.cpp', recursive=True):
    if f.endswith('globals.cpp'):
        continue
    s = open(f, encoding='latin1').read()
    t = s
    for a, b in SUB:
        t = re.sub(a, b, t)
    if t == s:
        continue
    need = [d for n, d in DECL.items() if re.search(r'\b%s\b' % n, t) and not re.search(r'\b%s\b' % re.escape(d), s)
            and re.search(r'\b%s\b' % n, re.sub(r'.*', '', '')) is None]
    need = [d for n, d in DECL.items() if n in re.findall(r'\w+', ' '.join(b for a, b in SUB if re.search(a, s)))]
    inc = [m.end() for m in re.finditer(r'^#include[^\n]*\n', t, re.M)]
    at = inc[-1] if inc else 0
    t = t[:at] + ''.join(d + '\n' for d in need if d not in t) + t[at:]
    open(f, 'w', encoding='latin1').write(t)
    print(f, len(need))
