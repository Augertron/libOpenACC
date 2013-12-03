
OPENACC_DIR?=../../

all: # executable-108 reference-108

check: # executable-108
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-108

clean:
	rm -f *.o executable-108

include ../rules.mk

executable-108: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-108

reference-108: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-108

