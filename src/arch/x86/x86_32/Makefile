INTR_SRCS := interrupt_32.cpp
CPP_SRCS := $(filter-out $(INTR_SRCS),$(wildcard *.cpp))

.PHONY: build $(CPP_SRCS) $(INTR_SRCS)

build: $(CPP_SRCS) $(INTR_SRCS)
	$(info compiling $(CPP_SRCS) and $(INTR_SRCS))
	mv *.o $(TMP_BLD_DIR)

$(CPP_SRCS):
	$(GCC) $(INCLUDES) $(GCCFLAGS) -c $@

$(INTR_SRCS):
	$(GCC) $(INCLUDES) $(GCCFLAGS) $(GCCFLAGS_INTR) -c $@
