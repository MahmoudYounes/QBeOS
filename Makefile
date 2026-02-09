export ROOT_DIR := $(shell pwd)
export BLD_DIR := ${ROOT_DIR}/build
export BIN_DIR := ${ROOT_DIR}/bin
export SRC_ROOT := ${ROOT_DIR}/src
export ISO_ROOT_DIR := ${ROOT_DIR}/iso_root
export ISO_NAME := QBeOS.iso
export IMG_NAME := QBeOS.IMG
export HDD_IMG_NAME := QBeOS.hdd
SUBDIRS := src boot/bootloader boot/earlyloader boot/qbekloader

.PHONY: build subdirs $(SUBDIRS)
build: setupEnvironment subdirs
	cp $(BLD_DIR)/* $(ISO_ROOT_DIR)/
	mkisofs -b bootloader.bin -no-emul-boot -boot-load-size 1 -o $(BIN_DIR)/$(ISO_NAME) $(ISO_ROOT_DIR)

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: setupEnvironment
setupEnvironment:
	if [ ! -d $(BLD_DIR) ]; then mkdir $(BLD_DIR); fi
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi
	if [ ! -d $(ISO_ROOT_DIR) ]; then mkdir $(ISO_ROOT_DIR); fi

run-qemu: $(BIN_DIR)/$(ISO_NAME)
	qemu-system-i386                                      \
	-machine q35																					\
  -enable-kvm									                          \
	-m 4096                                               \
	-smp 4                                                \
	-device i8042           															\
  -usb                                                  \
  -device usb-ehci,id=ehci                              \
  -drive if=none,id=usbdisk,format=raw,file=QBeOS.hdd   \
  -device usb-storage,bus=ehci.0,drive=usbdisk          \
	-vga std                                              \
  -no-shutdown -no-reboot                               \
	-monitor stdio                                        \
  -L /usr/share/qemu/sgabios.bin                        

run: $(BIN_DIR)/$(ISO_NAME)	
	virtualboxvm --startvm QBeOS	

run-bochs:
	bochs -debugger -q -f bochsrc.txt

debug: $(BIN_DIR)/$(ISO_NAME)
	qemu-system-i386                                 	   \
	-cpu host                                  			     \
	-enable-kvm											                     \
	-m 4096                                         	   \
	-no-reboot                                     		   \
	-drive format=raw,file=QBeOS.hdd                     \
	-serial stdio                                  		   \
	-smp 1                                         		   \
	-vga std                                       		   \
	-L /usr/share/qemu/sgabios.bin                       \
  -d int -no-shutdown -no-reboot -S -s

.PHONY: clean
clean:
	if [ -d $(BLD_DIR) ]; then rm -r $(BLD_DIR); fi
	if [ -d $(BIN_DIR) ]; then rm -r $(BIN_DIR); fi
	if [ -d $(ISO_ROOT_DIR) ]; then rm -r $(ISO_ROOT_DIR); fi
	-$(MAKE) -C src clean

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) build
