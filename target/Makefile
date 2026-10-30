# CoD2MP_s.exe 1.0, built with Visual C++ .NET 2003 (cl 13.10.3077, /GL + link /LTCG).
#
#   make            build/CoD2MP_s.exe (inside the cod2-msvc71-vs2003 image, see ../toolchain)
#   make measure    functions matching the original client (needs ../../cod2engineold/binaries)

IMAGE  := cod2-msvc71-vs2003-ft
OUT    := build/CoD2MP_s.exe
REL    := build/release/CoD2MP_s.exe
ANCHOR ?= ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe
# retail CodeView PDB GUID {6CEB0951-BCF5-4A4F-9E5B-D3394FBBD38F} in exe byte order; guidpin.c feeds it to UuidCreate
PDB_GUID := 5109eb6cf5bc4f4a9e5bd3394fbbd38f
# retail Direct3DCreate9 import hint is 9 = Summer/Oct 2004 d3d9.lib (dec2004 gives 10)
DXSDK  ?= ../../sdk/sum2004

ifndef IN_IMAGE

$(OUT) all:
	@mkdir -p build && python3 ../rsrc.py $(ANCHOR) build/cod2mp.ico
	@mkdir -p build/lib && cp -p $(DXSDK)/Lib/d3d9.lib build/lib/d3d9.lib
	@gcc -m32 -shared -fPIC -O2 -o build/guidpin.so guidpin.c
	docker run --rm -v $(CURDIR):/w -w /w $(IMAGE) sh -c 'make -j$$(nproc) -k IN_IMAGE=1 $(OUT); s=$$?; chown -R $(shell id -u):$(shell id -g) build; exit $$s'

measure: all
	python3 ../cmp.py $(OUT) build/CoD2MP_s.map $(ANCHOR) build/cmp.json >/dev/null
	python3 ../cov.py build/cmp.json
	@prev=$$(ls ../runs | tail -1); python3 ../delta.py ../runs/$$prev build/cmp.json 10
	@cp build/cmp.json ../runs/r$$(printf %03d $$(($$(ls ../runs | wc -l) + 1))).json

# Retail has a fixed image and a linker-generated CodeView/PDB record.
release:
	@mkdir -p build && python3 ../rsrc.py $(ANCHOR) build/cod2mp.ico
	@mkdir -p build/lib && cp -p $(DXSDK)/Lib/d3d9.lib build/lib/d3d9.lib
	docker run --rm -v $(CURDIR):/w -w /w $(IMAGE) sh -c 'make -j$$(nproc) -k IN_IMAGE=1 $(REL); s=$$?; chown -R $(shell id -u):$(shell id -g) build; exit $$s'
	@sha256sum $(REL) $(ANCHOR)

check: release
	cmp $(REL) $(ANCHOR)

# owner-approved post-link image: copies postlink_ranges.txt from the local anchor; $(REL) stays the pure source result
postlink: release
	python3 postcopy.py $(ANCHOR) $(REL) build/release/CoD2MP_s_postlink.exe postlink_ranges.txt
	cmp build/release/CoD2MP_s_postlink.exe $(ANCHOR)

clean:
	rm -rf build

.PHONY: $(OUT) all measure release check postlink clean

else

TUS    := $(shell sed -e 's/\#.*//' src/tus.txt | awk 'NF{print $$1}')
OBJS   := $(patsubst %,build/obj/%.obj,$(basename $(TUS)))
tuflags = $(shell awk '$$1=="$(1)"{$$1="";print}' src/tus.txt)

CL     := wine /opt/msvc710/Vc7/bin/cl.exe
LINK   := wine /opt/msvc710/Vc7/bin/link.exe
RC     := wine /opt/msvc710/Vc7/bin/rc.exe
CFLAGS := /nologo /c /O2 /GL /MT /DNDEBUG /DWIN32 /FI.\\compat\\msvc71.h /I.\\compat\\include \
	/I.\\universal /I. /I.\\qcommon /I.\\game /I.\\server /I.\\bgame /I.\\script /I.\\xanim \
	/I.\\stringed /I.\\speex /I.\\speex\\speex
# draft client layer from the Mac 1.3 decompile, on its own headers until ported
OCFLAGS := /nologo /c /O2 /GL /MT /TP /DNDEBUG /FIstdint.h /FImsvc_c89.h \
	/FImsvc_gcc_compat.h /FIwin32_compat.h /FIprocess.h /FIfloat.h /I.\\oc\\shims \
	/I.\\oc\\shims\\msvc /I.\\oc\\shims\\posix /I.\\oc /I.\\oc\\headers /DWIN32 /D_WIN32 /DW32_CLIENT /DCOD2_PATCH_10 \
	/D_GNU_SOURCE=1 /DWIN32_LEAN_AND_MEAN /DNOMINMAX /Disfinite=_finite /Dvsnprintf=_vsnprintf /Dsnprintf=_snprintf
LFLAGS := /nologo /LTCG /SAFESEH:NO /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /SUBSYSTEM:WINDOWS /STACK:0x10000,0x1000 /MAP:build\\CoD2MP_s.map

win = $(subst /,\\,$(1))
# header dependencies from /showIncludes (system headers skipped)
deps = awk '/including file:/{sub(/.*including file: */,""); gsub(/\\/,"/"); if ($$0 ~ /^[zZ]:\/opt/) next; \
	sub(/^[zZ]:\/w\/src\//,"src/"); sub(/^(\.\/)+/,"src/"); while (sub(/\/[^\/.][^\/]*\/\.\.\//,"/")); \
	sub(/\/headers\/pc\//,"/headers/PC/"); sub(/\/headers\/mac\//,"/headers/Mac/"); \
	sub(/\/effectscore\//,"/EffectsCore/"); \
	print "$@: " $$0; print $$0 ":"}' $@.log > $@.d

-include $(OBJS:=.d)


build/obj/%.obj: src/%.cpp
	@mkdir -p $(dir $@)
	@cd src && $(CL) $(if $(filter oc/%,$*),$(OCFLAGS),$(CFLAGS)) $(call tuflags,$*.cpp) /showIncludes /Fo..\\$(call win,$@) .\\$(call win,$*.cpp) > ../$@.log 2>&1 || (grep -v including ../$@.log; false)
	@$(deps)

build/obj/%.obj: src/%.c
	@mkdir -p $(dir $@)
	@cd src && $(CL) $(CFLAGS) $(call tuflags,$*.c) /showIncludes /Fo..\\$(call win,$@) .\\$(call win,$*.c) > ../$@.log 2>&1 || (grep -v including ../$@.log; false)
	@$(deps)

IMPLIBS := build/lib/mss32.lib build/lib/d3d9.lib
LIBS   := winmm.lib wsock32.lib $(call win,$(IMPLIBS)) dsound.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib libcmt.lib

build/lib/%.lib: src/lib/%.def src/lib/impname.pl
	@mkdir -p $(dir $@)
	@wine /opt/msvc710/Vc7/bin/lib.exe /nologo /machine:x86 /def:$(call win,$<) /out:$(call win,$@) > $@.log 2>&1 || (cat $@.log; false)
	@perl src/lib/impname.pl $@ 3 >> $@.log

# the retail mss32.lib (MSS_auto_cleanup + imports) was built with VS .NET 2002 (7.00.9466), local copy in vc70/
build/lib/mss_auto.obj: src/lib/mss_auto.c
	@mkdir -p $(dir $@)
	@cd src && wine ../vc70/cl.exe /nologo /c /O2 /MT /DNDEBUG /Fo..\\build\\lib\\mss_auto.obj lib\\mss_auto.c > ../$@.log 2>&1 || (cat ../$@.log; false)

build/lib/mss32.lib: src/lib/mss32.def build/lib/mss_auto.obj src/lib/impname.pl
	@wine vc70/lib.exe /nologo /machine:x86 /def:$(call win,$<) /out:build\\lib\\mss32_imp.lib > $@.log 2>&1 || (cat $@.log; false)
	@perl src/lib/impname.pl build/lib/mss32_imp.lib 1 >> $@.log
	@wine vc70/lib.exe /nologo /out:$(call win,$@) build\\lib\\mss32_imp.lib build\\lib\\mss_auto.obj >> $@.log 2>&1 || (cat $@.log; false)

# icon comes from the local retail exe (host step above); .rsrc is byte-identical to retail
build/cod2mp.res: src/cod2mp.rc build/cod2mp.ico
	@cd src && $(RC) /l 0x409 /fo..\\build\\cod2mp.res cod2mp.rc > ../$@.log 2>&1 || (cat ../$@.log; false)

$(OUT): $(OBJS) $(IMPLIBS) build/cod2mp.res src/tus.txt
	@printf '%s\n' $(call win,$(OBJS)) > build/objs.rsp
	$(LINK) $(LFLAGS) /OUT:$(call win,$@) @build\\objs.rsp build\\cod2mp.res $(LIBS) > build/link.log 2>&1; tail -1 build/link.log

$(REL): $(OBJS) $(IMPLIBS) build/cod2mp.res src/tus.txt Makefile
	@mkdir -p $(dir $@)
	@mkdir -p "$$(winepath -u 'c:\builds\crc6_v1\pc\cod2\pc')"
	@printf '%s\n' $(call win,$(OBJS)) > build/objs.rsp
	wineserver -k 2>/dev/null; export FAKETIME='2005-10-07 02:19:36' GUIDPIN=$(PDB_GUID) LD_PRELOAD="$(wildcard /usr/lib/*/faketime/libfaketime.so.1) $(CURDIR)/build/guidpin.so"; $(LINK) $(subst /FIXED:NO,/FIXED,$(subst build\\CoD2MP_s.map,build\\release\\CoD2MP_s.map,$(LFLAGS))) /DEBUG '/PDB:c:\builds\crc6_v1\pc\cod2\pc\CoD2MP_s.pdb' /OUT:$(call win,$@) @build\\objs.rsp build\\cod2mp.res $(LIBS) > build/release/link.log 2>&1 || { tail -10 build/release/link.log; exit 1; }
	@! grep -E 'LNK2001|LNK2019|fatal error' build/release/link.log
	@perl postlink.pl $@
	@cp "$$(winepath -u 'c:\builds\crc6_v1\pc\cod2\pc\CoD2MP_s.pdb')" build/release/CoD2MP_s.pdb

endif
