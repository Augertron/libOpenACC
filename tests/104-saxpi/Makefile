
OPENACC_DIR?=../../

all: # executable-104 reference-104

check: # executable-104
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-104

clean:
	rm -f *.o executable-104

include ../rules.mk

executable-104: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-104

reference-104: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-104

