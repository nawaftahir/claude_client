#!/bin/sh
# rebuild changed TUs, relink, diff one function: fx.sh <mangled-prefix> <client-addr> [lines]
cd "$(dirname "$0")/target" && make >/dev/null 2>build/fx.err || { tail -20 build/fx.err; exit 1; }
python3 ../fdiff.py build/CoD2MP_s.exe build/CoD2MP_s.map ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe "$1" "$2" "${3:-60}"
