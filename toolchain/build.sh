#!/bin/sh
# Assemble a clean VS2003 /opt/msvc710 from the original discs and build the image.
#   build.sh <disc1 Vc7 root> <disc2 Vc7/PlatformSDK root> [image]
# See README.md for where the discs come from.
set -e
D1=$1; SDK=$2; IMG=${3:-cod2-msvc71-vs2003}
SDKS=$(cd "$(dirname "$0")/../../sdk" && pwd)
T=$(mktemp -d); trap 'rm -rf $T' EXIT
mkdir -p $T/msvc710/Vc7
cp -a "$D1/Vc7/bin" "$D1/Vc7/include" "$D1/Vc7/lib" $T/msvc710/Vc7/
cp -a "$SDK" $T/msvc710/Vc7/PlatformSDK
# in a real install these sit in Common7/IDE; cl/link find them on PATH next to the exe
cp -a "$D1/Common7/IDE/mspdb71.dll" "$D1/Common7/IDE/msvcr71.dll" $T/msvc710/Vc7/bin/
tar -xzf $SDKS/msvc710_payload.tar.gz -C $T msvc710/Vc7/bin/msobj71.dll msvc710/Vc7/bin/msvcp71.dll
tar -C $T -czf $T/msvc710.tar.gz msvc710
cp "$(dirname "$0")/Dockerfile" $T/
docker build -t $IMG $T
