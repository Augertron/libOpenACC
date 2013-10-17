
all:
	make -C lib   all
	make -C tests all

check:
	make -C tests check

clean:
	make -C lib   clean
	make -C tests clean

