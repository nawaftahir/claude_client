#!/bin/sh
# rasm.sh START END : retail 1.0 client asm in [START,END) (hex, no 0x)
d=$(dirname "$0")
objdump -d --no-show-raw-insn -M intel "$d/../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe" --start-address=0x$1 --stop-address=0x$2 | tail -n +8
