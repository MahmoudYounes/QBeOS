export TMP_BLD_DIR := ${SRC_ROOT}/build
export GCC := i686-elf-g++
export LD := i686-elf-g++
export GCCFLAGS := -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -ffunction-sections -fno-builtin
export GCCFLAGS_INTR := -mgeneral-regs-only -mno-red-zone
export INCLUDES := -I $(shell pwd)
export ARCH := x86_32

SUB_DIRS := acpi arch kstdlib
CPP_SRCS := $(wildcard *.cpp)

.PHONY: build srcs $(CPP_SRCS) $(SUB_DIRS) setupEnvironment clean

build: setupEnvironment srcs
	mv *.o ${TMP_BLD_DIR}
	$(LD) -nostdlib -Wl,-melf_i386 -Wl,--oformat=binary -T $(ROOT_DIR)/linker.ld -o ${TMP_BLD_DIR}/KERNEL.IMG $(TMP_BLD_DIR)/*.o -lgcc
	cp ${TMP_BLD_DIR}/KERNEL.IMG ${BLD_DIR}

srcs: $(CPP_SRCS) $(SUB_DIRS)

$(CPP_SRCS):
	$(GCC) $(INCLUDES) $(GCCFLAGS) -c $@

$(SUB_DIRS):
	$(MAKE) -C $@

setupEnvironment:
	if [ ! -d ${TMP_BLD_DIR} ]; then mkdir ${TMP_BLD_DIR}; fi

clean:
	if [ -d ${TMP_BLD_DIR} ]; then rm -r ${TMP_BLD_DIR}; fi
	$(MAKE) -C $(SUB_DIRS) clean
