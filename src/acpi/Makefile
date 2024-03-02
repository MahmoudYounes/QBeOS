CWD := $(shell pwd)
CPP_SRCS := $(wildcard *.cpp)
OBJ_SRCS := $(CPP_SRCS:.cpp=.o)
LOCAL_INCLUDE := -I $(CWD)/include


.PHONY: build $(CPP_SRCS)
build: $(CPP_SRCS)
	mv ${CWD}/*.o $(TMP_BLD_DIR)

$(CPP_SRCS): $(LOCAL_BLD)
	$(GCC) $(INCLUDES) $(LOCAL_INCLUDE) $(GCCFLAGS) -c $@

$(LOCAL_BLD):
	if [ ! -d $(LOCAL_BLD) ]; then mkdir $(LOCAL_BLD); fi

.PHONY: clean
clean:
	$(info nothing to do)
