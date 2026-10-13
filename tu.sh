#!/bin/sh
# tu.sh src/path.cpp: compile one TU in the build image to /tmp (compile-error check only), run from target/
f=$(echo "$1" | sed 's|^src/||')
docker run --rm -v "$PWD":/w -w /w cod2-msvc71-build sh -c "
F=\$(make -s IN_IMAGE=1 --eval 'pv: ; @echo \$(if \$(filter oc/%,$f),\$(OCFLAGS),\$(CFLAGS)) \$(call tuflags,$f)' pv)
cd src && wine /opt/msvc710/Vc7/bin/cl.exe \$F /Foz:\\\\tmp\\\\tu.obj .\\\\\$(echo $f | tr / '\\\\\\\\')" 2>&1 | grep -v "C4114\|^$f\|wine:\|fixme" | tr -d '\r' | grep .
