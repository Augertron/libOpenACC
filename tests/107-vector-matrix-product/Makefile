
OPENACC_DIR?=../../

all: # executable-107 reference-107

check: # executable-107
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-107

clean:
	rm -f *.o executable-107

include ../rules.mk

executable-107: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-107

reference-107: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-107

