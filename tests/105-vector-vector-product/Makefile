
OPENACC_DIR?=../../

all: # executable-105 reference-105

check: # executable-105
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-105

clean:
	rm -f *.o executable-105

include ../rules.mk

executable-105: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-105

reference-105: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-105

