
OPENACC_DIR?=../
OPENACC_INC_DIR?=$(OPENACC_DIR)/include
OPENACC_LIB_DIR?=$(OPENACC_DIR)/lib
OPENACC_LIB?=$(OPENACC_LIB_DIR)/libopenacc.o

OPENCL_LIB?=-lOpenCL

INCLUDES+=-I$(OPENACC_INC_DIR)

LIBS+=$(OPENACC_LIB) $(OPENCL_LIB)
DEPS+=$(OPENACC_LIB_DIR)/libopenacc.o

CC=gcc
CFLAGS=
LD=g++
LDFLAGS=
MAKE=make

all: acc_init

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $? -o $@

acc_compiler_data.o: acc_compiler_data.c
	$(CC) $(CFLAGS) $(INCLUDES) -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c acc_compiler_data.c -o acc_compiler_data.o

$(OPENACC_DIR)/lib/libopenacc.o:
	$(MAKE) -C $(OPENACC_DIR)/lib/ libopenacc.o

acc_init: acc_init.o acc_compiler_data.o $(DEPS)
	$(LD) $(LDFLAGS) $(INCLUDES) $(LIBS) acc_init.o acc_compiler_data.o -o acc_init

acc_data: acc_data.o acc_compiler_data.o $(DEPS)
	$(LD) $(LDFLAGS) $(INCLUDES) $(LIBS) acc_data.o acc_compiler_data.o -o acc_data

acc_kernel: acc_kernel.o acc_compiler_data.o $(DEPS)
	$(LD) $(LDFLAGS) $(INCLUDES) $(LIBS) acc_kernel.o acc_compiler_data.o -o acc_kernel

check: acc_init
	./acc_init

clean:
	rm -f *.o acc_init acc_data acc_kernel

