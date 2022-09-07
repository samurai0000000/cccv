# Makefile
#
# Copyright (C) 2022, Charles Chiou

TARGETS =	cccv

TARGETS :=	$(foreach t,$(TARGETS),build/$(t))

.PHONY: default clean distclean

default: $(TARGETS)

clean:
	@[ -f build/Makefile ] && $(MAKE) --no-print-directory -C build clean

distclean:
	@echo DISTCLEAN
	@rm -rf build

build/cccv: build/Makefile Makefile $(wildcard *.*)
	@$(MAKE) --no-print-directory -C build

build/Makefile: CMakeLists.txt
	@mkdir -p build
	@echo CMAKE $@
	@cd build && cmake -Wno-dev ..

.PHONY: run

run: build/cccv
	./build/cccv -g
