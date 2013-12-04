
OPENACC_DIR?=../../

all: reference-103 executable-103-tile_0 executable-103-tile_1 executable-103-tile_2

include ../make-environment.mk

clean:
	rm -f *.o executable-103 reference-103 executable-103-tile_0 executable-103-tile_1 executable-103-tile_2

reference-103: reference.o $(OPENACC_LIB_DIR)/utils/timer.o
	$(LD) $(LDFLAGS) -lrt reference.o $(OPENACC_LIB_DIR)/utils/timer.o -o reference-103

include ../test-vars.cg

check: reference-103 executable-103-tile_0 executable-103-tile_1 executable-103-tile_2
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./reference-103 $(SIZE_N_103)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-103-tile_0 $(SIZE_N_103) $(NBR_GANG_103) $(NBR_WORKER_103)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-103-tile_1 $(SIZE_N_103) $(NBR_GANG_103) $(NBR_WORKER_103)
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-103-tile_2 $(SIZE_N_103) $(NBR_GANG_103) $(NBR_WORKER_103)

eval: reference-103 executable-103-tile_0 executable-103-tile_1 executable-103-tile_2
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../reference.sh $(OPENACC_DIR)/tests reference-103
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../eval.sh $(OPENACC_DIR)/tests executable-103-tile_0
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../eval.sh $(OPENACC_DIR)/tests executable-103-tile_1
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../eval.sh $(OPENACC_DIR)/tests executable-103-tile_2

include ../rules.mk

host-data-v1.o: host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=1 $(HOST_ENV_MACRO) -c host-data.c -o $@

executable-103-tile_0: transformed.o host-data-v1.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data-v1.o -o executable-103-tile_0

host-data-v2.o: host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=2 $(HOST_ENV_MACRO) -c host-data.c -o $@

executable-103-tile_1: transformed.o host-data-v2.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data-v2.o -o executable-103-tile_1

host-data-v3.o: host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=3 $(HOST_ENV_MACRO) -c host-data.c -o $@

executable-103-tile_2: transformed.o host-data-v3.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data-v3.o -o executable-103-tile_2




