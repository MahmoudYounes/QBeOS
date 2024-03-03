DIRS =
ifeq ($(ARCH), x86_32)
DIRS += x86/x86_32
endif

.PHONY: build $(DIRS)

build: $(DIRS)
$(DIRS):
	$(MAKE) -C $@
