cp /vc/c2ra.dll /opt/msvc710/Vc7/bin/c2.dll
wine /opt/msvc710/Vc7/bin/link.exe /nologo /LTCG /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /SUBSYSTEM:WINDOWS '/MAP:build\ox.map' '/OUT:build\ox.exe' '@build\'"$2" 'build\cod2mp.res' winmm.lib wsock32.lib 'build\lib\mss32.lib' 'build\lib\d3d9.lib' dsound.lib libcmt.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib > build/ox.log 2> build/ra_raw.txt
grep -a '^[FNP]' build/ra_raw.txt > build/ra.txt
chown $1 build/ox* build/ra*
