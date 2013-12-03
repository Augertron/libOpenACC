
OPENACC_DIR?=../../

all: # executable-109 reference-109

check: # executable-109
#	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ./executable-109

clean:
	rm -f *.o executable-109

include ../rules.mk

executable-109: transformed.o host-data.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) transformed.o host-data.o -o executable-109

reference-109: reference.o
	$(LD) $(LDFLAGS) $(LIBS) reference.o -o reference-109

