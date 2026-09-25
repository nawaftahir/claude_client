#!/bin/sh
# usage: spx.sh <outdir> <extra flags>
out=$1; shift; mkdir -p $out
for f in libspeex/*.c; do case $f in *test*) continue;; esac
  wine /opt/msvc710/Vc7/bin/cl.exe /nologo /c /O2 /DNDEBUG /DHAVE_CONFIG_H "$@" /Iwin32 /Iinclude /Iinclude/speex /Fo$out/ $f >> $out/build.log 2>&1 || echo "FAIL $f"
done
