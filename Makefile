BLD_DIR = ./build
BIN_DIR = ./bin
ISO_NAME = BeOS.iso

build: setupEnvironment bootloader kernel
	cp build/* iso_root/
	mkisofs -c bootcat -b bootloader.bin -no-emul-boot -boot-load-size 4 -o $(BIN_DIR)/$(ISO_NAME) ./iso_root

setupEnvironment:
	if [ ! -d build ]; then mkdir build; fi
	if [ ! -d bin ]; then mkdir bin; fi
	if [ ! -d iso_root ]; then mkdir iso_root; fi

bootloader:
	$(MAKE) -C bootLoader

kernel:
 	$(MAKE) -C kernel

run: $(BIN_DIR)/$(ISO_NAME)
	bochs -f bochsrc.txt

clean:
	if [ -d build ]; then rm -r build; fi
	if [ -d bin ]; then rm -r bin; fi
	if [ -d iso_root ]; then rm -r iso_root; fi