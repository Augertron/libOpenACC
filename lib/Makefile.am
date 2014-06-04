
AUTOMAKE_OPTIONS = foreign


ACC_VERBOSE ?= 0
ACC_DBG_RUNTIME ?= 0
ACC_DBG_REGION ?= 0
ACC_DBG_KERNEL ?= 0
ACC_DBG_HOST_CTX ?= 0
ACC_DBG_DATA ?= 0
ACC_BUILD_LOG ?= 0
ACC_DBG_TUNING ?= 0
ACC_DBG_PROFILING ?= 0
ACC_DBG_SQLITE_CONNECTION ?= 0

OPTS=-DVERBOSE=$(ACC_VERBOSE) -DDBG_RUNTIME=$(ACC_DBG_RUNTIME) -DDBG_REGION=$(ACC_DBG_REGION) -DDBG_KERNEL=$(ACC_DBG_KERNEL) -DDBG_HOST_CTX=$(ACC_DBG_HOST_CTX) -DDBG_DATA=$(ACC_DBG_DATA) -DBUILD_LOG=$(ACC_BUILD_LOG) -DDBG_TUNING=$(ACC_DBG_TUNING) -DDBG_PROFILING=$(ACC_DBG_PROFILING) -DDBG_SQLITE_CONNECTION=$(ACC_DBG_SQLITE_CONNECTION)


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
  private/memory.o \
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
  utils/containers/bimap.o \
  utils/profiling.o \
  utils/tuning.o \
  utils/sqlite.o

lib_LTLIBRARIES=libopenacc.la

libopenacc_la_SOURCES= \
  $(OPENACC_PUBLIC_LIB) \
  $(OPENACC_PRIVATE_LIB) \
  $(OPENACC_INTERNAL_LIB) \
  $(OPENACC_UTILS_LIB)
  
libopenacc_la_CXXFLAGS= \
  -g -O2 \
  -I$(srcdir)/../include \
  -I@sqlite_path@/include

