# Client toolchain (CoD2MP_s.exe 1.0)

How to rebuild the compiler image from nothing. Only this file, the Dockerfile and
build.sh are committed; the payloads are Microsoft files and stay out of git.

## Sources (all original VS.NET 2003 bits)

| What | Kept at | Origin |
|---|---|---|
| VS.NET 2003 Pro disc 1 | `dev/sdk/vs2003/VSPROD1.7z` (+ `.sha256`) | archive.org item `X09-46364`, `2003-05_X09-46364_X09-17449_VSPROD1.7z` |
| VS2003 Platform SDK, restored | `dev/sdk/vs2003/PlatformSDK_vs2003.tar.gz` | merge-module CABs inside disc 1 `vs_setup.msi` |
| `msobj71.dll`, `msvcp71.dll` | `dev/sdk/msvc710_payload.tar.gz` | the 10-15 image (see below); not in disc 1's tree |
| DirectX 9.0c SDKs | `dev/sdk/{sum2003,sum2004,oct2004,dec2004}` | `d3d9.lib` for the link |
| `mss32.lib` | `target/build/lib/mss32.lib` | Implib for the Miles DLL |

Disc 1 is a raw CD image (`image/VSPROD1.mdf`, 2448-byte sectors). Convert it to
an ISO by keeping bytes 16..2064 of each sector, then `7z x` it. The VC tree is
plain files under `Program Files/Microsoft Visual Studio .NET 2003/` (`Vc7/bin`,
`Vc7/include`, `Vc7/lib`, `Common7/IDE/mspdb71.dll`, `msvcr71.dll`).

The Platform SDK is not in that tree. It sits in the cabinet streams of
`vs_setup.msi` (`7z x vs_setup.msi`; the streams that start with `MSCF` are CABs,
e.g. `_1162_zWin32_b.common.msm`). Files inside are named by MSI File key
(`WinBase_h.95CE...`). To restore real paths: `cabextract` every CAB into one
flat dir, `msiinfo export vs_setup.msi File|Component|Directory`, resolve File ->
Component -> Directory chain, and keep paths under `Vc7/PlatformSDK` (1,482
files). Then merge `include` into `Include` and `lib` into `Lib`: the MSI uses both
spellings for the same Windows dir.

Disc 2 (`X09-46368`, alt `vs.-net-2003/VS.NET2003_Disc2.iso`) has no SDK; skip it.

Key binaries (identical on disc 1 and in the 10-15 payload):

```
2ecf86a3edfd3deae498e08298e210e984537ce9e11759930561e43f40bd2515  Vc7/bin/cl.exe    13.10.3077
bcd28f39b1798b07989c30a1df471cf87fc20e3802d130901a08f83800e6b81e  Vc7/bin/c2.dll
0d5f9712d0da843d787bf4ce4e678abc6b737940996706b2c86101eac9df3e3c  Vc7/bin/link.exe  7.10.3077
```

## Build the image

Quick path: `dev/sdk/vs2003/msvc710_vs2003.tar.gz` is the assembled `/opt/msvc710`
(29 MB, sha256 in `dev/sdk/vs2003/SHA256SUMS`).

```
mkdir ctx && cp claude_client/toolchain/Dockerfile ctx/ && cp dev/sdk/vs2003/msvc710_vs2003.tar.gz ctx/msvc710.tar.gz
docker build -t cod2-msvc71-vs2003 ctx
cd claude_client/target && make          # Makefile IMAGE defaults to cod2-msvc71-vs2003
```

From the discs: `claude_client/toolchain/build.sh "<disc1>/Program Files/Microsoft Visual Studio .NET 2003" <restored>/PlatformSDK`.

Verified 10-18: clean build of main = 1,273,258 B (= the accepted good.json, no
tie lost) and all 286 import hints equal retail (`hints.py`).

## What was wrong with the 10-15 image `cod2-msvc71-build` (do not reuse)

1. **Wine 8 (bookworm).** cl.exe spins forever at 100% CPU on any compile
   error. Wine 5.0.3 (bullseye) prints the error and exits. Wine 5 and Wine 8
   produce byte-identical exes.
2. **Server 2003 SP1 Platform SDK (2005-04-14)** instead of the VS2003 one,
   flattened into `Vc7/include` and `Vc7/lib` (`PlatformSDK/Include -> ../include`).
   It overwrote the VS2003 `xmmintrin.h`, `mmintrin.h`, `libcp.lib`, `nmake.exe`
   and RC tools, and added `specstrings.h`.
3. **No dsound.h/dsound.lib.** VS2003's Platform SDK ships both (dsound.h
   2002-06-20); the SP1 copy didn't.

Effect: header content is embedded in /GL objects and re-rolls LTCG ties.
Identical source built clean on that image = 1,266,593 B; main's objs from the
old image (10-13/14) relink to 1,273,258 B. The import libs don't match retail
either.

## Retail evidence for the SDK choice

Import hints in the IAT come straight from the import libs:

| Import | retail | VS2003 PSDK | PSDK Feb 2003 | SP1 (10-15 image) |
|---|---|---|---|---|
| kernel32 VirtualProtect | 889 | 889 | 891 | 902 |
| kernel32 GetACP | 245 | 245 | 245 | 253 |
| kernel32 RaiseException | 667 | 667 | 669 | 679 |
| d3d9 Direct3DCreate9 | 9 | - | - | dec2004 = 10, sum/oct2004 = 9 |

Checker: `python3 claude_client/hints.py <retail exe> <our exe>`.
