export TMP_BLD_DIR := ${SRC_ROOT}/build
export GCC := i686-elf-g++
export LD := i686-elf-g++
export OBJCPY := i686-elf-objcopy
export OBJCPYFLAGS := -O binary

export GCCFLAGS := -g -masm=intel -ffreestanding -lgcc -Wall -Wextra -fno-exceptions -fno-rtti -nostdlib -ffunction-sections -fno-builtin
export GCCFLAGS_INTR := -mgeneral-regs-only -mno-red-zone
# set of configs fixated for now until we can allow the user to configure them.
export CFG_DEFS := -D KERN_X86_32
export INCLUDES := -I $(shell pwd)
export ARCH := x86_32

SUB_DIRS := acpi pci arch kstdlib
CPP_SRCS := $(wildcard *.cpp)

.PHONY: build srcs $(CPP_SRCS) $(SUB_DIRS) setupEnvironment clean

build: setupEnvironment srcs
	mv *.o ${TMP_BLD_DIR}
	$(LD) -nostdlib -T $(ROOT_DIR)/linker.ld -o ${TMP_BLD_DIR}/kernel.elf $(TMP_BLD_DIR)/*.o -lgcc
	$(OBJCPY) $(OBJCPYFLAGS) ${TMP_BLD_DIR}/kernel.elf ${TMP_BLD_DIR}/KERNEL.IMG

	cp ${TMP_BLD_DIR}/KERNEL.IMG ${BLD_DIR}
	cp ${TMP_BLD_DIR}/kernel.elf ${BLD_DIR}

srcs: $(CPP_SRCS) $(SUB_DIRS)

$(CPP_SRCS):
	$(GCC) $(INCLUDES) $(GCCFLAGS) -c $@

$(SUB_DIRS):
	$(MAKE) -C $@

setupEnvironment:
	if [ ! -d ${TMP_BLD_DIR} ]; then mkdir ${TMP_BLD_DIR}; fi

clean:
	if [ -d ${TMP_BLD_DIR} ]; then rm -r ${TMP_BLD_DIR}; fi
	-rm *.o
	for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done
