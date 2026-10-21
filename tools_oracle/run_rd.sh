#!/bin/bash
# rd.sh TAG RA_FN [RSP]: LTCG link of claude_client/target objs (or RSP from mlw/v) with c2rd.dll; log -> ora/out/TAG.txt
M=/home/nawaf/cod2/test/dev/claude_client/target; D=/home/nawaf/mlw; O=$D/ora/out; VC=/home/nawaf/cod2/test/dev/lab/s6/target/build/vc
mkdir -p $O; TAG=$1; FN=$2; RSP=${3:-}
if [ -n "$RSP" ]; then RS="build\\v\\$RSP.rsp"; else cp $M/build/objs.rsp $O/base.rsp; RS='build\o\base.rsp'; fi
docker run --rm -e RA_FN="$FN" -v $D/src:/w/src -v $M/build/obj:/w/build/obj:ro -v $M/build/lib:/w/build/lib:ro -v $M/build/cod2mp.res:/w/build/cod2mp.res:ro -v $D/v:/w/build/v -v $O:/w/build/o -v $VC/c2rd.dll:/c2rd.dll:ro -v /home/nawaf/dtl/dsound.lib:/w/dsound.lib:ro -w /w cod2-msvc71-vs2003 sh -c "
cp /c2rd.dll /opt/msvc710/Vc7/bin/c2.dll
wine /opt/msvc710/Vc7/bin/link.exe /nologo /LTCG /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /SUBSYSTEM:WINDOWS '/MAP:build\\o\\$TAG.map' '/OUT:build\\o\\$TAG.exe' '@$RS' 'build\cod2mp.res' winmm.lib wsock32.lib 'build\lib\mss32.lib' 'build\lib\d3d9.lib' dsound.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libcmt.lib > build/o/$TAG.log 2> build/o/$TAG.raw
grep -a '^[FPC]' build/o/$TAG.raw > build/o/$TAG.txt; chown -R $(id -u):$(id -g) build/o"
wc -l $O/$TAG.txt
