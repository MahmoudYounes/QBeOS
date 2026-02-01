ASM := nasm
OBJCPY := i686-elf-objcopy
OBJCPYELFBINFLAGS := -O binary

GCC_C := i686-elf-gcc
GCC_LD := i686-elf-ld
GCC16FLAGS := -m16
GCCFLAGS := -g -O3 -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -nostdlib -ffunction-sections -fno-builtin

CSRCS := qbe16.c
COUT := qbe16.o

ASMSRCS := kloader.asm
ASMFLAGS := -Werror -w+orphan-labels -f elf  
ASMOUT := kloader.o

build: $(CSRCS)
	$(GCC_C) $(GCC16FLAGS) $(GCCFLAGS) -c $(CSRCS)
	$(ASM) $(ASMFLAGS) -o $(ASMOUT) $(ASMSRCS) 
	$(GCC_LD) -T linker.ld $(ASMOUT) *.o -o qbeloader.elf
	$(OBJCPY) $(OBJCPYELFBINFLAGS) /home/myonaiz/repos/QBeOS/boot/kloader/qbeloader.elf \
		$(BLD_DIR)/qbloader.bin  

# TODO: Remove this rule once the above rul is done. It is kept here for reference
build-legacy: kloader.asm
    # -w+orphan-labels causes any orphan labels to give a warning. an orphan label is <label> without :
	$(ASM) -Werror -w+orphan-labels -g -f bin -o $(BLD_DIR)/kloader.bin kloader.asm

