
OPENACC_DIR?=../../

all: executable-001

check: executable-001
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-001

clean:
	rm -f *.o executable-001

include ../rules.mk

executable-001: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-001

reference-001: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-001

