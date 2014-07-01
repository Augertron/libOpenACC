
bin_PROGRAMS=openacc

AM_CFLAGS=-g -O0

openacc_SOURCES= \
  openacc.c
openacc_CFLAGS= \
  -I$(srcdir)/../include \
  -I@sqlite_inc_path@ \
  -I@opencl_inc_path@
openacc_LDFLAGS= \
  -L@sqlite_lib_path@ \
  -L@opencl_lib_path@
openacc_LDADD= \
  $(builddir)/../lib/libopenacc.la \
  -lsqlite3 \
  -lOpenCL \
  -lrt

check-local: openacc
	./openacc --list

