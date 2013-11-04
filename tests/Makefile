
OPENACC_DIR?=../
OPENACC_INC_DIR?=$(OPENACC_DIR)/include
OPENACC_LIB_DIR?=$(OPENACC_DIR)/lib
OPENACC_LIB_NAME?=openacc
OPENACC_LIB_FILE=$(OPENACC_LIB_DIR)/lib$(OPENACC_LIB_NAME).so

OPENCL_LIB_DIR?=
OPENCL_LIB_NAME?=OpenCL

INCLUDES+=-I$(OPENACC_INC_DIR)

LIBS+=-L$(OPENACC_LIB_DIR) -l$(OPENACC_LIB_NAME) -l$(OPENCL_LIB_NAME)
DEPS+=$(OPENACC_LIB_FILE)

CC=gcc
CFLAGS+=

CXX=g++
CXXFLAGS+=

LD=g++
LDFLAGS+=

MAKE=make

all: acc_init acc_kernel

# we need to compile 'main' with g++ as OpenCL Intel uses C++

%.o: %.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $? -o $@

acc_compiler_data.o: acc_compiler_data.c
	$(CC) $(CFLAGS) $(INCLUDES) -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c acc_compiler_data.c -o acc_compiler_data.o

$(OPENACC_LIB_FILE):
	$(MAKE) -C $(OPENACC_LIB_DIR) lib$(OPENACC_LIB_NAME).so

acc_init: acc_init.o acc_compiler_data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) acc_init.o acc_compiler_data.o -o acc_init

acc_data: acc_data.o acc_compiler_data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) acc_data.o acc_compiler_data.o -o acc_data

acc_kernel: acc_kernel.o acc_compiler_data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) acc_kernel.o acc_compiler_data.o -o acc_kernel

check: acc_init acc_kernel
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./acc_init
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./acc_kernel

clean:
	rm -f *.o acc_init acc_data acc_kernel

