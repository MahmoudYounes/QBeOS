ASM = nasm
BLD_DIR = ../build

build: bootloader.asm
	$(ASM) -g -f bin -o $(BLD_DIR)/bootloader.bin bootloader.asm
