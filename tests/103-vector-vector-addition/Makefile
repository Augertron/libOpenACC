
OPENACC_DIR?=../../

all: executable-103 reference-103

check: executable-103
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-103 1048576 8 64

clean:
	rm -f *.o executable-103

include ../rules.mk

executable-103: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-103

reference-103: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-103

