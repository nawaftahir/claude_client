#!/bin/sh
# usage (in cod2-msvc71, /w = dev root): ocbuild.sh <outdir> <list of opencod2/src-relative .c files>
out=$1; list=$2
mkdir -p $out; export out
O='Z:\w\refrences\opencod2\src'
S='Z:\w\cod2engineold\targets\cod2mp_s\shim'
export FLAGS="/nologo /c /O2 /GL /DNDEBUG /FI$S\\stdint.h /FI$S\\msvc_c89.h /FI$S\\msvc_gcc_compat.h /FI$O\\headers\\win32_compat.h /I$S /I$O\\win32\\shims-msvc /I$O\\win32\\shims /I$O /I$O\\headers /DWIN32 /D_WIN32 /DW32_CLIENT /D_GNU_SOURCE=1 /DWIN32_LEAN_AND_MEAN /DNOMINMAX $EXTRA"
cat $list | xargs -P 12 -I{} sh -c '
  f={}; o=$out/$(echo $f | tr / _ | sed "s/\.[^.]*$//").obj
  wine /opt/msvc710/Vc7/bin/cl.exe $FLAGS /Fo$o "Z:\\w\\refrences\\opencod2\\src\\$(echo $f | tr / "\\\\")" > $o.log 2>&1
  echo "$? $f"' > $out/status.txt
