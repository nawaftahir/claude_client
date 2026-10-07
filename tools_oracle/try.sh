#!/bin/sh
# try.sh <rsp in target/build> : oracle link in that obj order -> build/ox.exe, ord.txt, cmp_ox.json, coverage, cut count
# needs target/build/vc/{c2y.dll,runr.sh} (see README)
cd /home/nawaf/cod2/test/dev/claude_client/target
docker run --rm -v $PWD:/w -v $PWD/build/vc:/vc -w /w cod2-msvc71-build sh /vc/runr.sh $(id -u):$(id -g) $1
python3 ../cmp.py build/ox.exe build/ox.map ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe build/cmp_ox.json >/dev/null
python3 ../cov.py build/cmp_ox.json | head -1
python3 ../tools_oracle/dfs.py build/ord.txt cuts > build/cuts_ox.txt; wc -l < build/cuts_ox.txt
