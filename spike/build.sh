#!/bin/sh
# usage: build.sh <outdir> <list> <extra cl flags...>
out=$1; list=$2; shift 2
mkdir -p $out
export out
cat $list | xargs -P 12 -I{} sh -c '
  tu={}; o=$out/$(echo $tu | tr / _ | sed "s/\.[^.]*$//").obj
  wine /opt/msvc710/Vc7/bin/cl.exe /nologo /c /O2 /DNDEBUG '"$*"' /FIZ:\\w\\msvc_shim.h /Iinc /Isrc/universal /Isrc /Isrc/qcommon /Isrc/game /Isrc/server /Isrc/bgame /Isrc/script /Isrc/xanim /Isrc/stringed /Fo$o src/$tu > $o.log 2>&1
  echo "$? $tu"' > $out/status.txt
