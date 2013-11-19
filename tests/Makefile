
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
CFLAGS+=-g

CXX=g++
CXXFLAGS+=

LD=g++
LDFLAGS+=

MAKE=make

all: container test_00 test_01 test_02

# we need to compile 'main' with g++ as OpenCL Intel uses C++

test_%.o: test_%.c
	$(CXX) $(CFLAGS) $(INCLUDES) -c $? -o $@

data_%.o: data_%.c
	$(CC) $(CFLAGS) $(INCLUDES) -DACC_RUNTIME_DIR=\"$(OPENACC_DIR)\" -c $? -o $@

container.o: container.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $? -o $@

$(OPENACC_LIB_FILE):
	$(MAKE) -C $(OPENACC_LIB_DIR) lib$(OPENACC_LIB_NAME).so

container: container.o data_empty.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) container.o data_empty.o -o container

test_00: test_00.o data_00.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_00.o data_00.o -o test_00

test_01: test_01.o data_01.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_01.o data_01.o -o test_01

test_02: test_02.o data_02.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) test_02.o data_02.o -o test_02

check: container test_00 test_01 test_02
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./container
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./test_00
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./test_01
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./test_02

clean:
	rm -f *.o test_00 test_01 test_02

