
OPENACC_DIR?=../../

all: executable-101

check: executable-101
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-101

clean:
	rm -f *.o executable-101

include ../rules.mk

executable-101: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-101

reference-101: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-101

