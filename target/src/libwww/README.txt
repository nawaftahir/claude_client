W3C libwww, upstream source, used by the 1.3 client's auto-update download code (dl_main).

Version: libwww 5.3.2 release tree (github.com/w3c/libwww commit 8c731531, Dec 2000). Its Windows
config.h reports W3C_VERSION "5.3.1", the string found in the 1.3 binary.

Files: the 75 Library/src/*.c named by __FILE__ strings in the 1.3 binary, unmodified.

Headers: generated from the upstream *.html sources (the code inside <PRE> blocks), the way libwww's own
"www -to text/x-c" rule does it (lab/v13/html2h.py).

windows/config.h: upstream, with HT_POSIX_REGEX turned off (the 1.3 binary has no regex code).

Check: plain /O2 compiles of HTAtom.c, HTList.c and HTString.c match 25 of 36 functions in the 1.3 binary.
