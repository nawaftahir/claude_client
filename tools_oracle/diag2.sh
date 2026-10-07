#!/bin/sh
# diag2.sh: LTCG link with hooked c2x.dll -> build/ord.txt (F = compile start, X = callee referenced)
SP=$(dirname $0)
cd /home/nawaf/cod2/test/dev/claude_client/target
docker run --rm -v $PWD:/w -v $SP/vc:/vc -w /w cod2-msvc71-build sh /vc/run.sh $(id -u):$(id -g)
wc -l build/ord.txt
