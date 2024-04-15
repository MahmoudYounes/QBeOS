SRCS=$(wildcard *.S)
ASM_CMPL_CCFLAGS=-m32 -masm=intel
CONFIGS_DEF=KERN_X86_32
.PHONY:build $(SRCS)

build: $(SRCS)
	mv *.o $(TMP_BLD_DIR)

$(SRCS):
	$(GCC) $(CFG_DEFS) $(ASM_CMPL_CCFLAGS) -c $@
