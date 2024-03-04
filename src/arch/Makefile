DIRS =
ifeq ($(ARCH), x86_32)
DIRS += x86/x86_32
endif
SRCS =$(wildcard *.cpp *.asm)

DIRS += x86/asm

.PHONY: build $(DIRS)

build: $(DIRS)
$(DIRS):
	$(MAKE) -C $@

.PHONY: clean
clean:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
