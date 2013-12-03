
TESTS=001-iteration\
 101-vector-constant-addition\
 102-vector-constant-product\
 103-vector-vector-addition\
 104-saxpi\
 105-vector-vector-product\
 106-matrix-vector-product\
 107-vector-matrix-product\
 108-matrix-matrix-addition\
 109-matrix-matrix-product

all:
	for test in $(TESTS) ; do \
		make -C $$test; \
	done

check:
	for test in $(TESTS) ; do \
		make -C $$test check ; \
	done

eval:
	for test in $(TESTS) ; do \
		make -C $$test eval ; \
	done

clean:
	for test in $(TESTS) ; do \
		make -C $$test clean ; \
	done

