
OPENACC_PUBLIC_LIB = \
  public/arch.o \
  public/async.o \
  public/device.o \
  public/memory.o \
  public/runtime.o

OPENACC_PRIVATE_LIB = \
  private/data-env.o \
  private/init.o \
  private/kernel.o \
  private/region.o \
  private/debug.o

OPENACC_INTERNAL_LIB = \
  internal/mem-manager.o \
  internal/data-env.o \
  internal/init.o \
  internal/opencl-init.o \
  internal/opencl-debug.o \
  internal/region.o \
  internal/runtime.o \
  internal/kernel.o

OPENACC_UTILS_LIB = \
  utils/utils.o \
  utils/io.o \
  utils/timer.o \
  utils/containers/associative.o \
  utils/containers/set.o \
  utils/containers/map.o \
  utils/containers/bimap.o

CC=gcc
CFLAGS=-g -I../include/ -fPIC
OPTS=
LD=gcc
LDFLAGS+=-Wl,--no-as-needed 

all: libopenacc.so

%.o: %.c 
	$(CC) $(CFLAGS) $(DFLAGS) $(OPTS) -c $? -o $@

libopenacc.a: $(OPENACC_PUBLIC_LIB) $(OPENACC_PRIVATE_LIB) $(OPENACC_INTERNAL_LIB) $(OPENACC_UTILS_LIB)
	ar rcs libopenacc.a $(OPENACC_PUBLIC_LIB) $(OPENACC_PRIVATE_LIB) $(OPENACC_INTERNAL_LIB) $(OPENACC_UTILS_LIB)

libopenacc.so: $(OPENACC_PUBLIC_LIB) $(OPENACC_PRIVATE_LIB) $(OPENACC_INTERNAL_LIB) $(OPENACC_UTILS_LIB)
	$(LD) $(LDFLAGS) -shared $(OPENACC_PUBLIC_LIB) $(OPENACC_PRIVATE_LIB) $(OPENACC_INTERNAL_LIB) $(OPENACC_UTILS_LIB) -o libopenacc.so

clean:
	rm -rf $(OPENACC_PUBLIC_LIB) $(OPENACC_PRIVATE_LIB) $(OPENACC_INTERNAL_LIB) $(OPENACC_UTILS_LIB) libopenacc.a libopenacc.so

