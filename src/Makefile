CPP_SOURCES = $(wildcard *.cpp)
OBJ_SOURCES = $(CPP_SOURCES:.cpp=.o)
GCC = i686-elf-g++
LD = i686-elf-ld

# defining phony rules
.PHONY: build srcs $(CPP_SOURCES) setupEnvironment clean

build: setupEnvironment srcs
	$(LD) -nostdlib -melf_i386 -T../linker.ld --oformat binary -o build/KERNEL.IMG $(OBJ_SOURCES)
	mv *.o build
	cp build/KERNEL.IMG ../build/

srcs: $(CPP_SOURCES)

$(CPP_SOURCES):
	$(GCC) -masm=intel -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -O0 -c $@

setupEnvironment:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi
	rm -r $(OBJ_SOURCES)
