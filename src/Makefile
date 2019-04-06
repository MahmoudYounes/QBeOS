CPP_SOURCES = $(wildcard *.cpp)
OBJ_SOURCES = $(CPP_SOURCES:.cpp=.obj)
GCC = i686-elf-g++
LD = i686-elf-ld

build: setupEnvironment $(CPP_SOURCES)
	$(GCC) -ffreestanding -c $(CPP_SOURCES) -o build/$(OBJ_SOURCES) -Wall -Wextra -fno-exceptions -fno-rtti
	$(LD) -melf_i386 -T../linker.ld --oformat binary -o build/KERNEL.IMG build/$(OBJ_SOURCES)
	cp build/KERNEL.IMG ../build/

setupEnvironment:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi