#!/bin/sh
# bjmp.sh <mangled-substr> <retail-hex-addr> <size>: list jump/call target mismatches (fdiff hides them)
a=$(grep -m1 "$1" build/CoD2MP_s.map | awk '{print $3}')
objdump -d -M intel --no-show-raw-insn --start-address=0x$a --stop-address=$(printf '0x%x' $((0x$a+$3))) build/CoD2MP_s.exe | tail -n +8 | awk -v b=$((0x$a)) '{o=strtonum("0x" substr($1,1,length($1)-1))-b; if($2 ~ /^j/ && $3 ~ /^0x/) t=sprintf("%x",strtonum($3)-b); else t=""; print o, $2, t}' > build/bj1
objdump -d -M intel --no-show-raw-insn --start-address=0x$2 --stop-address=$(printf '0x%x' $((0x$2+$3))) ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe | tail -n +8 | awk -v b=$((0x$2)) '{o=strtonum("0x" substr($1,1,length($1)-1))-b; if($2 ~ /^j/ && $3 ~ /^0x/) t=sprintf("%x",strtonum($3)-b); else t=""; print o, $2, t}' > build/bj2
diff build/bj1 build/bj2 | head -${4:-20}
