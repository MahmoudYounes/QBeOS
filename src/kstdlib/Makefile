CPP_SRCS := $(wildcard *.cpp)

.PHONY: build $(CPP_SRCS)

build: $(CPP_SRCS)
	mv *.o $(TMP_BLD_DIR)

$(CPP_SRCS):
	$(GCC) $(INCLUDES) $(GCCFLAGS) -c $@

.PHONY: clean
clean:
	-rm *.o
