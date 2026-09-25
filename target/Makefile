# CoD2MP_s.exe 1.0, built with Visual C++ .NET 2003 (cl 13.10.3077, /GL + link /LTCG).
#
#   make            build/CoD2MP_s.exe (inside the cod2-msvc71-build image)
#   make measure    functions matching the original client (needs ../../cod2engineold/binaries)

IMAGE  := cod2-msvc71-build
OUT    := build/CoD2MP_s.exe
ANCHOR ?= ../../cod2engineold/binaries/windows/1.0/CoD2MP_s.exe

ifndef IN_IMAGE

$(OUT) all:
	docker run --rm -v $(CURDIR):/w -w /w $(IMAGE) sh -c 'make -j4 IN_IMAGE=1 $(OUT); s=$$?; chown -R $(shell id -u):$(shell id -g) build; exit $$s'

measure: all
	python3 ../cmp.py $(OUT) build/CoD2MP_s.map $(ANCHOR) build/cmp.json >/dev/null
	python3 ../cov.py build/cmp.json

clean:
	rm -rf build

.PHONY: all measure clean

else

TUS    := $(shell sed -e 's/\#.*//' src/tus.txt | awk 'NF{print $$1}')
OBJS   := $(patsubst %,build/obj/%.obj,$(basename $(TUS)))
tuflags = $(shell awk '$$1=="$(1)"{$$1="";print}' src/tus.txt)

CL     := wine /opt/msvc710/Vc7/bin/cl.exe
LINK   := wine /opt/msvc710/Vc7/bin/link.exe
CFLAGS := /nologo /c /O2 /GL /MT /DNDEBUG /DWIN32 /FI.\\compat\\msvc71.h /I.\\compat\\include \
	/I.\\universal /I. /I.\\qcommon /I.\\game /I.\\server /I.\\bgame /I.\\script /I.\\xanim \
	/I.\\stringed /I.\\speex /I.\\speex\\speex
LFLAGS := /nologo /LTCG /FORCE /OPT:NOREF /OPT:NOICF /FIXED:NO /SUBSYSTEM:WINDOWS /MAP:build\\CoD2MP_s.map

win = $(subst /,\\,$(1))

build/obj/%.obj: src/%.cpp
	@mkdir -p $(dir $@)
	@cd src && $(CL) $(CFLAGS) $(call tuflags,$*.cpp) /Fo..\\$(call win,$@) .\\$(call win,$*.cpp) > ../$@.log 2>&1 || (cat ../$@.log; false)

build/obj/%.obj: src/%.c
	@mkdir -p $(dir $@)
	@cd src && $(CL) $(CFLAGS) $(call tuflags,$*.c) /Fo..\\$(call win,$@) .\\$(call win,$*.c) > ../$@.log 2>&1 || (cat ../$@.log; false)

$(OUT): $(OBJS)
	@printf '%s\n' $(call win,$(OBJS)) > build/objs.rsp
	$(LINK) $(LFLAGS) /OUT:$(call win,$@) @build\\objs.rsp libcmt.lib kernel32.lib user32.lib > build/link.log 2>&1; tail -1 build/link.log

endif
