#!/bin/sh
# Tie study probe: rebuild, measure, list functions that flipped vs build/cmp_base.json.
# usage: tieprobe.sh <label>   (apply the source edit first; result kept in build/tie_<label>.json)
cd /home/nawaf/cod2/test/dev/claude_client/target || exit 1
make > build/b.txt 2>&1; grep -i ' error' build/b.txt | head -5
python3 ../cmp.py build/CoD2MP_s.exe build/CoD2MP_s.map ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe build/cmp.json >/dev/null
cp build/cmp.json "build/tie_$1.json"
echo "== $1: $(python3 ../cov.py build/cmp.json | head -1)"
python3 ../delta.py build/cmp_base.json build/cmp.json 40 | grep -v '\$L'
