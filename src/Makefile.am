
bin_PROGRAMS=openacc

AM_CFLAGS=-g -O0

openacc_SOURCES= \
  openacc.c
openacc_CFLAGS= \
  -I$(srcdir)/../include \
  -I@opencl_inc_path@ \
  -I@sqlite_inc_path@ \
  -DLIBOPENACC_PATH=\"@prefix@\" \
  -DOPENCL_INC_PATH=\"@opencl_inc_path@\" \
  -DOPENCL_LIB_PATH=\"@opencl_lib_path@\" \
  -DSQLITE_INC_PATH=\"@sqlite_inc_path@\" \
  -DSQLITE_LIB_PATH=\"@sqlite_lib_path@\"
openacc_LDFLAGS= \
  -L@opencl_lib_path@ \
  -L@sqlite_lib_path@
openacc_LDADD= \
  $(builddir)/../lib/libopenacc.la \
  -lOpenCL \
  -lsqlite3 \
  -lrt

check-local: openacc
	./openacc
	./openacc --list
	./openacc -l
	./openacc --number-devices
	./openacc -n
	./openacc --incpath
	./openacc --libpath
	./openacc --libs

