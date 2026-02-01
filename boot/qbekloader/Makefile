ASM := nasm
OBJCPY := i686-elf-objcopy
OBJCPYELFBINFLAGS := -O binary

GCC_C := i686-elf-gcc
GCC_LD := i686-elf-ld
GCCFLAGS := -g -O0 -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -nostdlib -ffunction-sections -fno-builtin -fno-pie -fno-pic

CSRCS := $(wildcard *.c) 

QBEKL := qbekloader

build: $(CSRCS)
	$(GCC_C) $(GCCFLAGS) -I /home/myonaiz/repos/QBeOS/boot/qbekloader -c $(CSRCS)
	$(GCC_LD) -no-pie -T linker.ld *.o -o $(QBEKL).elf 
	$(OBJCPY) $(OBJCPYELFBINFLAGS) /home/myonaiz/repos/QBeOS/boot/$(QBEKL)/$(QBEKL).elf \
		$(BLD_DIR)/$(QBEKL).bin  

