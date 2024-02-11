ALL_CPP_SOURCES = $(wildcard *.cpp)
INTR_SOURCES = interrupt_32.cpp
CPP_SOURCES = $(filter-out $(INTR_SOURCES),$(ALL_CPP_SOURCES))
OBJ_SOURCES = $(ALL_CPP_SOURCES:.cpp=.o)
GCC = i686-elf-g++
LD = i686-elf-g++

# defining phony rules
.PHONY: build srcs $(CPP_SOURCES) $(INTR_SOURCES) setupEnvironment clean

build: setupEnvironment srcs
	$(LD) -nostdlib -Wl,-melf_i386 -Wl,--oformat=binary -T../linker.ld -o build/KERNEL.IMG $(OBJ_SOURCES) -lgcc
	mv *.o build
	cp build/KERNEL.IMG ../build/

srcs: $(INTR_SOURCES) $(CPP_SOURCES)

$(CPP_SOURCES):
	$(GCC) -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -ffunction-sections -fno-builtin -c $@

$(INTR_SOURCES):
	$(GCC) -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -ffunction-sections -fno-builtin -mgeneral-regs-only -mno-red-zone -c $@

setupEnvironment:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi
	rm -r $(OBJ_SOURCES)
