CPP_SOURCES = $(wildcard *.cpp)
OBJ_SOURCES = $(CPP_SOURCES:.cpp=.o)
GCC = i686-elf-g++
LD = i686-elf-g++

# defining phony rules
.PHONY: build srcs $(CPP_SOURCES) setupEnvironment clean

build: setupEnvironment srcs
	$(LD) -nostdlib -Wl,-melf_i386 -Wl,--oformat=binary -T../linker.ld -o build/KERNEL.IMG $(OBJ_SOURCES) -lgcc
	mv *.o build
	cp build/KERNEL.IMG ../build/

srcs: $(CPP_SOURCES)

$(CPP_SOURCES):
	$(GCC) -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -ffunction-sections -fno-builtin -c $@

setupEnvironment:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi
	rm -r $(OBJ_SOURCES)
