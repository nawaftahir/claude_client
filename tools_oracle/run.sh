cp /vc/c2x.dll /opt/msvc710/Vc7/bin/c2.dll
wine /opt/msvc710/Vc7/bin/link.exe /nologo /LTCG /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /SUBSYSTEM:WINDOWS '/MAP:build\ox.map' '/OUT:build\ox.exe' '@build\objs.rsp' 'build\cod2mp.res' winmm.lib wsock32.lib 'build\lib\mss32.lib' 'build\lib\d3d9.lib' dsound.lib libcmt.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib > build/ox.log 2> build/ord_raw.txt
grep -a '^[FX] ' build/ord_raw.txt > build/ord.txt
chown $1 build/ox* build/ord*
