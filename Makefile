export ROOT_DIR := $(shell pwd)
export BLD_DIR := ${ROOT_DIR}/build
export BIN_DIR := ${ROOT_DIR}/bin
export SRC_ROOT := ${ROOT_DIR}/src
export ISO_ROOT_DIR := ${ROOT_DIR}/iso_root
export ISO_NAME := QBeOS.iso
SUBDIRS := src boot

.PHONY: build subdirs $(SUBDIRS)
build: setupEnvironment subdirs
	cp $(BLD_DIR)/* $(ISO_ROOT_DIR)/
	mkisofs -c bootcat -b bootloader.bin -no-emul-boot -boot-load-size 4 -o $(BIN_DIR)/$(ISO_NAME) $(ISO_ROOT_DIR)

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: setupEnvironment
setupEnvironment:
	if [ ! -d $(BLD_DIR) ]; then mkdir $(BLD_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi
	if [ ! -d $(ISO_ROOT_DIR) ]; then mkdir $(ISO_ROOT_DIR); fi

run: $(BIN_DIR)/$(ISO_NAME)
	bochs -f bochsrc.txt

.PHONY: clean
clean:
	if [ -d $(BLD_DIR) ]; then rm -r $(BLD_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -r $(BIN_DIR); fi
	if [ -d $(ISO_ROOT_DIR) ]; then rm -r $(ISO_ROOT_DIR); fi
	$(MAKE) -C src clean

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) build
