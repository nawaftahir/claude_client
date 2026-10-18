#!/usr/bin/env python3
"""Inputs for ltcgsim.py: every TU compiled non-/GL /Ob0 /Gy (call graph) plus /E (declaration order).
usage (from target/): nogl.py <outdir> [--i-only]   (runs in cod2-msvc71-vs2003, ~80 s per pass)"""
import os, re, subprocess, sys
out = os.path.abspath(sys.argv[1])
CF = r'/nologo /O2 /Ob0 /Gy /MT /DNDEBUG /DWIN32 /FI.\compat\msvc71.h /I.\compat\include /I.\universal /I. /I.\qcommon /I.\game /I.\server /I.\bgame /I.\script /I.\xanim /I.\stringed /I.\speex /I.\speex\speex'
OF = r'/nologo /O2 /Ob0 /Gy /MT /TP /DNDEBUG /FIstdint.h /FImsvc_c89.h /FImsvc_gcc_compat.h /FIwin32_compat.h /FIprocess.h /FIfloat.h /I.\oc\shims /I.\oc\shims\msvc /I.\oc\shims\posix /I.\oc /I.\oc\headers /DWIN32 /D_WIN32 /DW32_CLIENT /DCOD2_PATCH_10 /D_GNU_SOURCE=1 /DWIN32_LEAN_AND_MEAN /DNOMINMAX /Disfinite=_finite /Dvsnprintf=_vsnprintf /Dsnprintf=_snprintf'
q = lambda s: "'" + s.replace(' ', "' '") + "'"
cmds = []
for l in open('src/tus.txt'):
    l = l.split('#')[0].split()
    if not l: continue
    f, rest = l[0], ' '.join(x for x in l[1:] if x != '/GL')
    o = '/out/obj/' + re.sub(r'\.[^.]*$', '', f)
    os.makedirs(out + '/obj/' + os.path.dirname(f), exist_ok=True)
    fl = q(OF if f.startswith('oc/') else CF)
    src = "'.\\%s'" % f.replace('/', '\\')
    if '--i-only' not in sys.argv:
        cmds.append("wine /opt/msvc710/Vc7/bin/cl.exe /c %s %s '/FoZ:%s.obj' %s > %s.obj.log 2>&1 || echo FAIL %s" % (fl, rest, o.replace('/', '\\'), src, o, f))
    cmds.append("rm -f %s.i; wine /opt/msvc710/Vc7/bin/cl.exe /E %s %s %s > %s.i 2>/dev/null" % (o, fl, rest, src, o))
open(out + '/cmds.txt', 'w').write('\n'.join(cmds) + '\n')
subprocess.run(['docker', 'run', '--rm', '-v', os.path.abspath('src') + ':/w/src', '-v', out + ':/out', '-w', '/w/src',
                'cod2-msvc71-vs2003', 'sh', '-c', 'xargs -P $(nproc) -d "\\n" -I CMD sh -c "CMD" < /out/cmds.txt; chown -R %d:%d /out' % (os.getuid(), os.getgid())])
