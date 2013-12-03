
OPENACC_DIR?=../../

all: # executable-106 reference-106

check: # executable-106
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-106

clean:
	rm -f *.o executable-106

include ../rules.mk

executable-106: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-106

reference-106: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-106

