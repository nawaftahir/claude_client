#!/bin/sh
# fd.sh <prefix> <addr> : diff lines vs retail (run from target/)
python3 ../fdiff.py build/CoD2MP_s.exe build/CoD2MP_s.map ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe "$1" "$2" 400 | grep '^[-+@]'
