#!/bin/sh
# rebuild changed TUs, relink, diff one function: fx.sh <mangled-prefix> <client-addr> [lines]
cd "$(dirname "$0")/target" && make >build/fx.err 2>&1 || { grep -E " error |error:" build/fx.err | head -20; exit 1; }
python3 ../fdiff.py build/CoD2MP_s.exe build/CoD2MP_s.map ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe "$1" "$2" "${3:-60}"
