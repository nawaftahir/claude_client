#!/bin/sh
# usage: measure.sh <objdir> <name> : link objs in the msvc image, compare with the client
set -e
cd /home/nawaf/cod2/test/dev/claude_client
docker run --rm -v $PWD:/w -w /w cod2-msvc71 sh -c "./link.sh $1 $2; chown $(id -u):$(id -g) $2.*" >/dev/null 2>&1
python3 cmp.py $2.dll $2.map ../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe cmp_$2.json > /dev/null
python3 cov.py cmp_$2.json
