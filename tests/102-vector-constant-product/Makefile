
OPENACC_DIR?=../../

all: # executable-102

check: # executable-102
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-102

clean:
	rm -f *.o executable-102

include ../rules.mk

executable-102: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-102

reference-102: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-102

