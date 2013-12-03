
OPENACC_DIR?=../
OPENACC_INC_DIR?=$(OPENACC_DIR)/include
OPENACC_LIB_DIR?=$(OPENACC_DIR)/lib
OPENACC_LIB_NAME?=openacc
OPENACC_LIB_FILE=$(OPENACC_LIB_DIR)/lib$(OPENACC_LIB_NAME).so

OPENCL_LIB_DIR?=
OPENCL_LIB_NAME?=OpenCL

INCLUDES+=-I$(OPENACC_INC_DIR)

LIBS+=-L$(OPENACC_LIB_DIR) -l$(OPENACC_LIB_NAME) -l$(OPENCL_LIB_NAME) -lrt
DEPS+=$(OPENACC_LIB_FILE)

CC=gcc
CFLAGS+=-g

CXX=g++
CXXFLAGS+=

LD=g++
LDFLAGS+=

MAKE=make

all: test_00 test_01 test_02

# C file compilation

test_%.o: test_%.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $? -o $@

data_%.o: data_%.c
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=1 -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c $? -o $@

# LibOpenACC dependency

$(OPENACC_LIB_FILE):
	$(MAKE) -C $(OPENACC_LIB_DIR) lib$(OPENACC_LIB_NAME).so

$(OPENACC_LIB_DIR)/utils/timer.o:
	$(MAKE) -C $(OPENACC_LIB_DIR) utils/timer.o

# Test #0 : Verify gang/worker ID for simplest case

test_00: test_00.o data_00.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_00.o data_00.o -o test_00

# Test #1 : Add a constant to every elements in an array

test_01: test_01.o data_01.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_01.o data_01.o -o test_01

# Test #2 : Vector Addition

test_02.o: test_02.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $? -o $@

test_02: test_02.o data_02.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_02.o data_02.o -o test_02

# Check rule...

check: test_00 test_01 test_02
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./test_00
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./test_01
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./test_02 1048576 8 64

# Tiling space exploration

## Sequential CPU reference for Vector Addition (TODO OpenMP ref)

cpu_ref_02.o: ref_02.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $? -o $@

cpu_ref_02: cpu_ref_02.o $(OPENACC_LIB_DIR)/utils/timer.o
	$(LD) $(LDFLAGS) -lrt cpu_ref_02.o $(OPENACC_LIB_DIR)/utils/timer.o -o cpu_ref_02

## Versions of kernel descriptors

data_02_v1.o: data_02.c
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=1 -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c $? -o $@

test_02_v1: test_02.o data_02_v1.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_02.o data_02_v1.o -o test_02_v1

data_02_v2.o: data_02.c
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=2 -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c $? -o $@

test_02_v2: test_02.o data_02_v2.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_02.o data_02_v2.o -o test_02_v2

data_02_v3.o: data_02.c
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=3 -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c $? -o $@

test_02_v3: test_02.o data_02_v3.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_02.o data_02_v3.o -o test_02_v3

## Running Performance Evaluation

perf: cpu_ref_02 test_02_v1 test_02_v2 test_02_v3
	./eval_02.sh

# test of associative container

container.o: container.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $? -o $@

container: container.o data_empty.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) container.o data_empty.o -o container

# Cleanning...

clean:
	rm -f *.o test_00 test_01 test_02 cpu_ref_02
	rm -f test_02_v1 test_02_v2 test_02_v3
	rm -f ref.log acc*-*-*.*
	rm -f container

