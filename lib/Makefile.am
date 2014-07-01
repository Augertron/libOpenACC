
AUTOMAKE_OPTIONS = foreign subdir-objects

ACLOCAL_AMFLAGS=-I ../m4

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
  public/arch.c \
  public/async.c \
  public/device.c \
  public/memory.c \
  public/runtime.c

OPENACC_PRIVATE_LIB = \
  private/data-env.c \
  private/init.c \
  private/kernel.c \
  private/region.c \
  private/runtime.c \
  private/memory.c \
  private/debug.c

OPENACC_INTERNAL_LIB = \
  internal/mem-manager.c \
  internal/data-env.c \
  internal/init.c \
  internal/opencl-init.c \
  internal/opencl-debug.c \
  internal/region.c \
  internal/runtime.c \
  internal/kernel.c

OPENACC_UTILS_LIB = \
  utils/utils.c \
  utils/io.c \
  utils/timer.c \
  utils/containers/associative.c \
  utils/containers/set.c \
  utils/containers/map.c \
  utils/containers/bimap.c \
  utils/profiling.c \
  utils/sqlite/sqlite.c \
  utils/sqlite/region.c \
  utils/sqlite/region-parameter.c \
  utils/sqlite/region-scalar.c \
  utils/sqlite/region-data.c \
  utils/sqlite/kernel-group.c \
  utils/sqlite/kernel.c \
  utils/sqlite/kernel-deps.c \
  utils/sqlite/kernel-parameter.c \
  utils/sqlite/kernel-scalar.c \
  utils/sqlite/kernel-data.c \
  utils/sqlite/version.c \
  utils/sqlite/loop.c \
  utils/sqlite/tile.c \
  utils/sqlite/event.c 

lib_LTLIBRARIES=libopenacc.la

libopenacc_la_SOURCES= \
  $(OPENACC_PUBLIC_LIB) \
  $(OPENACC_PRIVATE_LIB) \
  $(OPENACC_INTERNAL_LIB) \
  $(OPENACC_UTILS_LIB)

libopenacc_la_CFLAGS= \
  -g -O0 \
  -I$(top_srcdir)/include \
  -I@sqlite_inc_path@ \
  -I@opencl_inc_path@

install-exec-local:
	mkdir -p $(prefix)/lib/opencl/
	cp $(srcdir)/opencl/libopenacc.cl $(prefix)/lib/opencl/

