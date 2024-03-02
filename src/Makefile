export TMP_BLD_DIR := ${SRC_ROOT}/build
export GCC := i686-elf-g++
export LD := i686-elf-g++
export GCCFLAGS := -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -ffunction-sections -fno-builtin
export GCCFLAGS_INTR := -mgeneral-regs-only -mno-red-zone
export INCLUDES := -I $(shell pwd)/include

SUB_DIRS := acpi
ALL_CPP_SOURCES := $(wildcard *.cpp)
INTR_SOURCES := interrupt_32.cpp
CPP_SOURCES := $(filter-out $(INTR_SOURCES),$(ALL_CPP_SOURCES))
OBJ_SOURCES := $(ALL_CPP_SOURCES:.cpp=.o)


.PHONY: build srcs $(CPP_SOURCES) $(INTR_SOURCES) $(SUB_DIRS) setupEnvironment clean

build: setupEnvironment srcs
	mv *.o ${TMP_BLD_DIR}
	$(LD) -nostdlib -Wl,-melf_i386 -Wl,--oformat=binary -T $(ROOT_DIR)/linker.ld -o ${TMP_BLD_DIR}/KERNEL.IMG $(TMP_BLD_DIR)/*.o -lgcc
	cp ${TMP_BLD_DIR}/KERNEL.IMG ${BLD_DIR}

srcs: $(INTR_SOURCES) $(CPP_SOURCES) $(SUB_DIRS)

$(CPP_SOURCES):
	$(GCC) $(INCLUDES) $(GCCFLAGS) -c $@

$(INTR_SOURCES):
	$(GCC) $(INCLUDES) $(GCCFLAGS) $(GCCFLAGS_INTR) -c $@

$(SUB_DIRS):
	$(MAKE) -C $(SUB_DIRS)

setupEnvironment:
	if [ ! -d ${TMP_BLD_DIR} ]; then mkdir ${TMP_BLD_DIR}; fi

clean:
	if [ -d ${TMP_BLD_DIR} ]; then rm -r ${TMP_BLD_DIR}; fi
	rm $(OBJ_SOURCES)
	$(MAKE) -C $(SUB_DIRS) clean
