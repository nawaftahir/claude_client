# CoD2MP_s.exe 1.0, built with Visual C++ .NET 2003 (cl 13.10.3077, /GL + link /LTCG).
#
#   make            build/CoD2MP_s.exe (inside the cod2-msvc71-build image)
#   make measure    functions matching the original client (needs ../../cod2engineold/binaries)

IMAGE  := cod2-msvc71-build
OUT    := build/CoD2MP_s.exe
ANCHOR ?= ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe

ifndef IN_IMAGE

$(OUT) all:
	docker run --rm -v $(CURDIR):/w -w /w $(IMAGE) sh -c 'make -j4 -k IN_IMAGE=1 $(OUT); s=$$?; chown -R $(shell id -u):$(shell id -g) build; exit $$s'

measure: all
	python3 ../cmp.py $(OUT) build/CoD2MP_s.map $(ANCHOR) build/cmp.json >/dev/null
	python3 ../cov.py build/cmp.json
	@prev=$$(ls ../runs | tail -1); python3 ../delta.py ../runs/$$prev build/cmp.json 10
	@cp build/cmp.json ../runs/r$$(printf %03d $$(($$(ls ../runs | wc -l) + 1))).json

clean:
	rm -rf build

.PHONY: $(OUT) all measure clean

else

TUS    := $(shell sed -e 's/\#.*//' src/tus.txt | awk 'NF{print $$1}')
OBJS   := $(patsubst %,build/obj/%.obj,$(basename $(TUS)))
tuflags = $(shell awk '$$1=="$(1)"{$$1="";print}' src/tus.txt)

CL     := wine /opt/msvc710/Vc7/bin/cl.exe
LINK   := wine /opt/msvc710/Vc7/bin/link.exe
CFLAGS := /nologo /c /O2 /GL /MT /DNDEBUG /DWIN32 /FI.\\compat\\msvc71.h /I.\\compat\\include \
	/I.\\universal /I. /I.\\qcommon /I.\\game /I.\\server /I.\\bgame /I.\\script /I.\\xanim \
	/I.\\stringed /I.\\speex /I.\\speex\\speex
# draft client layer from the Mac 1.3 decompile, on its own headers until ported
OCFLAGS := /nologo /c /O2 /GL /MT /TP /DNDEBUG /FIstdint.h /FImsvc_c89.h \
	/FImsvc_gcc_compat.h /FIwin32_compat.h /FIprocess.h /FIfloat.h /I.\\oc\\shims \
	/I.\\oc\\shims\\msvc /I.\\oc\\shims\\posix /I.\\oc /I.\\oc\\headers /DWIN32 /D_WIN32 /DW32_CLIENT \
	/D_GNU_SOURCE=1 /DWIN32_LEAN_AND_MEAN /DNOMINMAX /Disfinite=_finite /Dvsnprintf=_vsnprintf /Dsnprintf=_snprintf
LFLAGS := /nologo /LTCG /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /SUBSYSTEM:WINDOWS /MAP:build\\CoD2MP_s.map

win = $(subst /,\\,$(1))

build/obj/%.obj: src/%.cpp
	@mkdir -p $(dir $@)
	@cd src && $(CL) $(if $(filter oc/%,$*),$(OCFLAGS),$(CFLAGS)) $(call tuflags,$*.cpp) /Fo..\\$(call win,$@) .\\$(call win,$*.cpp) > ../$@.log 2>&1 || (cat ../$@.log; false)

build/obj/%.obj: src/%.c
	@mkdir -p $(dir $@)
	@cd src && $(CL) $(CFLAGS) $(call tuflags,$*.c) /Fo..\\$(call win,$@) .\\$(call win,$*.c) > ../$@.log 2>&1 || (cat ../$@.log; false)

IMPLIBS := build/lib/mss32.lib build/lib/d3d9.lib
LIBS   := winmm.lib wsock32.lib $(call win,$(IMPLIBS)) dsound.lib libcmt.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib

build/lib/%.lib: src/lib/%.def
	@mkdir -p $(dir $@)
	@wine /opt/msvc710/Vc7/bin/lib.exe /nologo /machine:x86 /def:$(call win,$<) /out:$(call win,$@) > $@.log 2>&1 || (cat $@.log; false)

$(OUT): $(OBJS) $(IMPLIBS)
	@printf '%s\n' $(call win,$(OBJS)) > build/objs.rsp
	$(LINK) $(LFLAGS) /OUT:$(call win,$@) @build\\objs.rsp $(LIBS) > build/link.log 2>&1; tail -1 build/link.log

endif
