CPP_SOURCES = $(wildcard *.cpp)
OBJ_SOURCES = $(CPP_SOURCES=.cpp=.obj)
GCC = i686-elf-gcc
LD = i686-elf-ld

build: setupEnvironment $(CPP_SOURCES)
	$(GCC) -c $(CPP_SOURCES) -o build/$(OBJ_SOURCES)
	$(LD) -melf_i386 -T../linker.ld -oformat binary build/$(OBJ_SOURCES);

setupEnvironment:
	if [ ! -d build ]; then mkdir build; fi

clean:
	if [ -d build ]; then rm -r build; fi