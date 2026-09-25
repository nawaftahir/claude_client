#!/bin/sh
# usage: link.sh <objdir> <out>
ls $1/*.obj | sed 's|/|\\|g' > $2.rsp
wine /opt/msvc710/Vc7/bin/link.exe /nologo /DLL /NOENTRY /LTCG /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /MAP:$2.map /NODEFAULTLIB /OUT:$2.dll @$2.rsp > $2.log 2>&1
